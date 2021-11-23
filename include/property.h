#include <any_container/decode.h>

namespace spiritsaway::serialize
{
	// 每个变量在当前定义内只能是uint8的整数索引 所以一个类里面只能定义255个变量 
	// 0号变量保留不使用 以后可以扩充为std::uint16_t 或者与var_mutate_cmd合并为一个std::uint16_t 
	using var_idx_type = std::uint8_t;
	using var_cmd_type = std::uint8_t;// 对于变量的改变操作类型 全量赋值 清空 等等
	const static std::uint8_t depth_max = 8;
	class type_hash
	{
		static std::size_t last_used_id;
	public:
		template <typename T>
		static std::size_t hash()
		{
			static const std::size_t value = last_used_id++;
			return value;
		}
	};

	using var_prefix_idx_type = std::vector<var_idx_type>;
	static var_prefix_idx_type concat_prefix(const var_prefix_idx_type pre, 
		var_idx_type offset)
	{
		auto temp = pre;
		temp.push_back(offset);
		return temp;
	}
	enum class var_mutate_cmd : var_cmd_type
	{
		clear = 0,
		set = 1,
		mutate_item = 2,
		vector_push_back = 10,
		vector_idx_mutate = 11,
		vector_idx_delete = 12,
		vector_insert_head = 13,
		vector_pop_back = 14,
		map_insert = 20,
		map_erase = 21,
		set_add = 30,
		set_erase = 31,
		item_update = 41,

	};
	enum class notify_kind
	{
		no_notify = 0,
		self_notify = 1,
		all_notify = 2
	};
	
	struct mutate_msg
	{
		var_prefix_idx_type prefix;
		var_idx_type idx;
		var_mutate_cmd cmd;
		json data;
	};
	class msg_queue_base
	{
	public:
		virtual void add(const var_idx_type& offset, var_mutate_cmd _cmd, const json& m_data) = 0;
	};
	class msg_queue : public msg_queue_base
	{
		std::deque<mutate_msg>& m_queue;
		const var_prefix_idx_type& m_parentm_idxes;
	public:
		msg_queue(std::deque<mutate_msg>& msg_queue,
			const var_prefix_idx_type& parentm_idxes)
			: m_queue(msg_queue)
			, m_parentm_idxes(parentm_idxes)
		{

		}
		msg_queue(const msg_queue& other) = default;
		void add(const var_idx_type& offset, var_mutate_cmd cmd, const json& data)
		{
			m_queue.emplace_back(mutate_msg{ m_parentm_idxes, offset, cmd, data });
			return;
		}

	};
	template <typename T>
	class item_msg_queue : public msg_queue_base
	{
		std::deque<mutate_msg>& m_queue;
		const var_prefix_idx_type& m_parentm_idxes;
		const T& m_item_key;
	public:
		item_msg_queue(std::deque<mutate_msg>& msg_queue,
			const var_prefix_idx_type& parentm_idxes,
			const T& key)
			: m_queue(msg_queue)
			, m_parentm_idxes(parentm_idxes)
			, m_item_key(key)
		{

		}
		item_msg_queue(const item_msg_queue& other) = default;
		void add(const var_idx_type& offset, var_mutate_cmd cmd, const json& data)
		{
			m_queue.emplace_back(mutate_msg{ m_parentm_idxes, 0, cmd, encode_multi(m_item_key, offset, cmd, data) });
			return;
		}
	};
    template <typename T, typename B = void>
	class prop_proxy;
	template <>
	class prop_proxy<void*>
	{
	protected:
		void* m_data = nullptr;
		const std::size_t m_data_type_id;
		msg_queue_base& m_msg_queue;
		const var_idx_type& m_offset;
		const notify_kind m_notify_kind;
		prop_proxy(void* data, std::size_t data_type_id,
			msg_queue_base& msg_queue,
			const var_idx_type& offset,
			notify_kind in_notify_kind)
			: m_data(data)
			, m_data_type_id(data_type_id)
			, m_msg_queue(msg_queue)
			, m_notify_kind(in_notify_kind)
			, m_offset(offset)
		{

		}
	public:

		template <typename T>
		static prop_proxy* construct(T* data, msg_queue_base& msg_queue,
			const var_idx_type& offset,
			notify_kind in_notify_kind)
		{
			return new prop_proxy(reinterpret_cast<void*>(data), type_hash::hash<T>(), msg_queue, offset, in_notify_kind);
		}
		template <typename T>
		prop_proxy<T>* rebind() const
		{
			if (!m_data)
			{
				return nullptr;
			}
			if (m_data_type_id != type_hash::hash<T>())
			{
				return nullptr;
			}
			return new prop_proxy<T>(*reinterpret_cast<T*>(m_data), m_msg_queue, m_offset, m_notify_kind);
		}
	};
	template <typename T>
    class prop_proxy<T, std::enable_if_t<
		std::is_pod_v<T> || std::is_same_v<T, std::string>, void>
	>
    {
    public:
        prop_proxy(T& data, 
			msg_queue_base& msg_queue, 
			const var_idx_type& offset,
			notify_kind in_notify_kind = notify_kind::self_notify)
			: m_data(data),
		m_msg_queue(msg_queue),
		m_notify_kind(in_notify_kind),
		m_offset(offset)

        {

        }
		T& get()
		{
			return m_data;
		}
		operator const T&() const
		{
			return m_data;
		}
		void set(const T& data)
		{
			m_data = data;
			if (m_notify_kind != notify_kind::no_notify)
			{
				m_msg_queue.add(m_offset,
					var_mutate_cmd::set, encode(m_data));
			}
			
		}
		
		void clear()
		{
			m_data = {};
			if (m_notify_kind != notify_kind::no_notify)
			{
				m_msg_queue.add(m_offset,
					var_mutate_cmd::clear, json());
			}
		}
		
		bool replay(var_mutate_cmd cmd, const json& data)
		{
			switch (cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(data);
			case var_mutate_cmd::set:
				return replay_set(data);
			default:
				return false;
			}
		}
	private:
		bool replay_set(const json& data)
		{
			return decode(data, m_data);
		}
		bool replay_clear(const json& data)
		{
			m_data = {};
			return true;
		}
    private:
        T& m_data;
		msg_queue_base& m_msg_queue;
		const var_idx_type& m_offset;
		const notify_kind m_notify_kind;
	};

	template<typename T>
	class prop_proxy<std::vector<T>>
	{
	public:
		prop_proxy(std::vector<T>& data, 
			msg_queue_base& msg_queue, 
			const var_idx_type& offset) :
			m_data(data),
			m_msg_queue(msg_queue),
			m_offset(offset)
		{

		}
		std::vector<T>& get()
		{
			return m_data;
		}
		operator const std::vector<T>&() const
		{
			return m_data;
		}
		void set(const std::vector<T>& data)
		{
			m_data = data;
			m_msg_queue.add(m_offset, var_mutate_cmd::set, encode(m_data));
		}
		
		void clear()
		{
			m_data.clear();
			m_msg_queue.add(m_offset, var_mutate_cmd::clear, json());
		}
		
		void push_back(const T& new_data)
		{
			m_data.push_back(new_data);
			m_msg_queue.add(m_offset, var_mutate_cmd::vector_push_back, encode(new_data));
		}
		
		void pop_back()
		{
			if (m_data.size())
			{
				m_data.pop_back();
			}
			m_msg_queue.add(m_offset, var_mutate_cmd::vector_pop_back, json());
		}
		
		void idx_mutate(std::size_t idx, const T& new_data)
		{
			if (idx < m_data.size())
			{
				m_data[idx] = new_data;
			}
			m_msg_queue.add(m_offset, var_mutate_cmd::vector_idx_mutate, encode_multi(idx, new_data));
		}
		
		void idx_delete(std::size_t idx)
		{
			if (idx < m_data.size())
			{
				m_data.erase(m_data.begin() + idx);
			}
			m_msg_queue.add(m_offset, var_mutate_cmd::vector_idx_mutate, encode(idx));
		}
		
		bool replay(var_mutate_cmd cmd, const json& data)
		{
			switch (cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(data);
			case var_mutate_cmd::set:
				return replay_set(data);
			case var_mutate_cmd::vector_push_back:
				return replay_push_back(data);
			case var_mutate_cmd::vector_pop_back:
				return replay_pop_back(data);
			case var_mutate_cmd::vector_idx_mutate:
				return replaym_idx_mutate(data);
			case var_mutate_cmd::vector_idx_delete:
				return replaym_idx_delete(data);
			default:
				return false;
			}
		}
	private:
		bool replay_set(const json& data)
		{
			return decode(data, m_data);
		}
		bool replay_clear(const json& data)
		{
			m_data.clear();
			return true;
		}
		bool replay_push_back(const json& data)
		{
			T temp;
			if (decode(data, temp))
			{
				m_data.push_back(temp);
				return true;
			}
			else
			{
				return false;
			}
		}
		bool replay_pop_back(const json& data)
		{
			if (m_data.size())
			{
				m_data.pop_back();
			}
			return true;
		}
		bool replaym_idx_mutate(const json& data)
		{
			std::size_t idx;
			T temp;
			if (!decode_multi(data, idx, temp))
			{
				return false;
			}
			if (idx < m_data.size())
			{
				m_data[idx] = temp;
			}
			return true;
		}
		bool replaym_idx_delete(const json& data)
		{
			std::size_t idx;
			if (!decode(data, idx))
			{
				return false;
			}
			if (idx < m_data.size())
			{
				m_data.erase(m_data.begin() + idx);
			}
			return true;
		}
	private:
		std::vector<T>& m_data;
		msg_queue_base& m_msg_queue;
		const var_idx_type m_offset;
	};

	template <typename T1, typename T2>
	class prop_proxy<std::unordered_map<T1, T2>>
	{
	public:
		prop_proxy(std::unordered_map<T1, T2>& data,
			msg_queue_base& msg_queue,
			const var_idx_type& offset) :
			m_data(data),
			m_msg_queue(msg_queue),
			m_offset(offset)
		{

		}
		std::unordered_map<T1, T2>& get()
		{
			return m_data;
		}
		operator const std::unordered_map<T1, T2>&() const
		{
			return m_data;
		}
		void set(const std::unordered_map<T1, T2>& data)
		{
			m_data = data;
			m_msg_queue.add(m_offset, var_mutate_cmd::set, encode(m_data));
		}

		void clear()
		{
			m_data.clear();
			m_msg_queue.add(m_offset, var_mutate_cmd::clear, json());
		}

		void insert(const T1& key, const T2& value)
		{
			m_data[key] = value;
			m_msg_queue.add(m_offset, var_mutate_cmd::map_insert, encode_multi(key, value));
		}

		void erase(const T1& key)
		{
			m_data.erase(key);
			m_msg_queue.add(m_offset, var_mutate_cmd::map_erase, encode(key));
		}

		bool replay(var_mutate_cmd cmd, const json& data)
		{
			switch (cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(data);
			case var_mutate_cmd::set:
				return replay_set(data);
			case var_mutate_cmd::map_insert:
				return replay_insert(data);
			case var_mutate_cmd::map_erase:
				return replay_erase(data);
			default:
				return false;
			}
		}
	private:
		bool replay_set(const json& data)
		{
			return decode(data, m_data);
		}
		bool replay_clear(const json& data)
		{
			m_data.clear();
			return true;
		}
		bool replay_insert(const json& data)
		{
			T1 key;
			T2 value;
			if (!decode_multi(data, key, value))
			{
				return false;
			}
			m_data[key] = value;
			return true;
		}
		bool replay_erase(const json& data)
		{
			T1 key;
			if (!decode(data, key))
			{
				return false;
			}
			m_data.erase(key);
			return true;
		}
	private:
		std::unordered_map<T1, T2>& m_data;
		msg_queue_base& m_msg_queue;
		const var_idx_type m_offset;
	};

	
	class property_item_base;
	class property_bag_base;
	class property_item_base
	{
	private:
		property_bag_base* m_container;
	public:
		property_item_base(property_bag_base* container)
			: m_container(container)
		{

		}
		virtual json encode() const = 0;
		virtual std::string type_name() = 0;
		virtual bool replay_mutate_msg(std::size_t field_index,
			var_mutate_cmd cmd, const json& data) = 0;
	};
	class property_bag_base
	{
	public:
		var_prefix_idx_type m_depth;
		virtual json encode() const = 0;
		virtual bool decode(const json& data) = 0;
		virtual const std::string& type_name() const = 0;
		std::deque<mutate_msg>& m_dest_buffer;
		property_bag_base(var_prefix_idx_type depth,
			std::deque<mutate_msg>& cmd_queue) :
			m_depth(depth),
			m_dest_buffer(cmd_queue)
		{

		}
		virtual bool replay_mutate_msg(std::size_t field_index,
			var_mutate_cmd cmd, const json& data) = 0;
		virtual prop_proxy<void*>* get(const std::string& member_name) = 0;

	};
	template <typename T>
	class property_item : public property_item_base
	{
	protected:
		T m_id;
		item_msg_queue<T> m_cmd_buffer;

	public:
		const T& id() const
		{
			return m_id;
		}
		property_item(property_bag_base* container,
			std::deque<mutate_msg>& _dest_queue,
			const T& id):
			property_item_base(container),
			m_id(id),
			m_cmd_buffer(_dest_queue, container->m_depth, m_id)

		{

		}
		json encode() const
		{
			json result;
			result["id"] = m_id;
			return result;
		}
		bool decode(const json& data)
		{
			if (!data.is_object())
			{
				return false;
			}
			auto iter = data.find("id");
			if (iter == data.end())
			{
				return false;
			}
			if (!spiritsaway::serialize::decode(*iter, m_id))
			{
				return false;
			}
			return true;
		}
	};
	
	template <typename K, typename Item>
	class property_bag:public property_bag_base
	{

		static_assert(std::is_base_of<property_item<K>, Item>::value,
			"item should be derived from property_item<K>");
		std::vector<Item> m_data;
		std::unordered_map<K, std::size_t> m_index;
	public:
		using key_type = K;
		using value_type = Item;
		property_bag(var_prefix_idx_type depth,
			std::deque<mutate_msg>& cmd_queue) :
			property_bag_base(depth, cmd_queue)
		{

		}
		property_bag& operator=(const property_bag& other)
		{
			m_data.clear();
			m_data.shrink_to_fit();
			m_data.reserve(other.m_data.size());
			for (const auto& one_item : other.m_data)
			{
				insert(one_item.encode());
			}
			return *this;
		}

		json encode() const
		{
			return spiritsaway::serialize::encode(m_data);
		}
		bool decode(const json& data)
		{
			if (m_data.size())
			{
				std::cout << "the bag is not empty while decode data " << data.dump() << " to property_bag " << type_name() << std::endl;
				return false;
			}
			json::array_t temp_array;
			if (!data.is_array())
			{
				return false;
			}
			m_data.reserve(data.size());
			temp_array = data.get<json::array_t>();
			for (const auto& one_json_item : temp_array)
			{
				value_type temp_item(this, m_dest_buffer, K());
				if (!temp_item.decode(one_json_item))
				{
					m_data.clear();
					return false;
				}
				m_data.push_back(temp_item);
			}
			for (std::size_t i = 0; i < m_data.size(); i++)
			{
				m_index[m_data[i].id()] = i;
			}
			return true;
		}
		bool has_item(const K& _key) const
		{
			auto cur_iter = m_index.find(_key);
			if (cur_iter == m_index.end())
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		bool create(const json& data)
		{
			Item temp_item(this, m_dest_buffer, K());
			if (!spiritsaway::serialize::decode(data, temp_item))
			{
				std::cout<<"property_bag "<<type_name()<<" fail to create item with data {}"<< data.dump()<<std::endl;
				return false;
			}
			m_index[temp_item.id()] = m_data.size();
			m_data.emplace_back(std::move(temp_item));
			return true;
		}
		Item& create(const K& key)
		{
			Item temp_item(this, m_dest_buffer, key);
			m_index[temp_item.id()] = m_data.size();
			m_data.emplace_back(std::move(temp_item));
			return m_data.back();
		}
		bool operator==(const property_bag& other)
		{
			return m_data == other.m_data;
		}
		void clear()
		{
			m_index.clear();
			m_data.clear();
		}
		bool erase(const K& key)
		{
			auto cur_iter = m_index.find(key);
			if (cur_iter == m_index.end())
			{
				return false;
			}
			else
			{
				if (cur_iter->second == m_data.size())
				{
					m_index.erase(cur_iter);
					m_data.pop_back();
				}
				else
				{
					auto pre_index = cur_iter->second;
					m_index.erase(cur_iter);
					m_index[m_data[pre_index].id()] = pre_index;
					m_data[pre_index].swap(m_data.back());
					m_data.pop_back();
				}
				return true;
			}
		}
		std::optional<std::reference_wrapper<const Item>> get(const K& key) const
		{
			auto cur_iter = m_index.find(key);
			if (cur_iter == m_index.end())
			{
				return std::nullopt;
			}
			else
			{
				return std::cref(m_data[cur_iter->second]);
			}
		}
		std::optional<std::reference_wrapper<Item>> get_mut(const K& key)
		{
			auto cur_iter = m_index.find(key);
			if (cur_iter == m_index.end())
			{
				return std::nullopt;
			}
			else
			{
				return std::ref(m_data[cur_iter->second]);
			}
		}

		prop_proxy<void*>* get(const std::string& member_name) override
		{
			return nullptr;
		}
	};
	template <typename T>
	class prop_proxy<T, 
		std::enable_if_t<std::is_base_of_v<property_bag_base, T>, void>>
	{

		T& m_data;
	public:
		using key_type = typename T::key_type;
		using value_type = typename T::value_type;
		prop_proxy(T& data,
			msg_queue_base& msg_queue,
			const var_idx_type& offset)
			:m_data(data)
			,m_msg_queue(msg_queue)
			,m_offset(offset)
		{

		}
		operator const std::vector<value_type>&() const
		{
			return m_data.m_data;
		}
		prop_proxy<void*>* get(const std::string& member_name)
		{
			return m_data.get(member_name);
		}
		void clear()
		{
			m_data.clear();
			m_msg_queue.add(m_offset, var_mutate_cmd::clear, json());
		}
		std::vector<key_type> keys() const
		{
			std::vector<key_type> result;
			result.reserve(m_data.m_data.size());
			for (const auto& one_item : m_data.m_data)
			{
				result.push_back(one_item.first);
			}
			return result;
		}
		void insert(const json& value)
		{
			if (m_data.create(value))
			{
				m_msg_queue.add(m_offset, var_mutate_cmd::map_insert, 
					value);
			}
			
		}

		void erase(const key_type& key)
		{
			if (m_data.erase(key))
			{
				m_msg_queue.add(m_offset, var_mutate_cmd::map_erase, encode(key));
			}
			
		}
		bool replay_insert(const json& data)
		{
			return m_data.create(data);
			
		}
		bool replay_clear(const json& data)
		{
			m_data.clear();
			return true;
		}
		bool replay_erase(const json& data)
		{
			key_type cur_k;
			if (!decode(data, cur_k))
			{
				return false;
			}
			return m_data.erase(cur_k);
		}
		bool replay(var_mutate_cmd _cmd, const json& data)
		{
			switch (_cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(data);
			case var_mutate_cmd::map_insert:
				return replay_insert(data);
			case var_mutate_cmd::map_erase:
				return replay_erase(data);
			default:
				return false;
			}
		}
	private:
		msg_queue_base& m_msg_queue;
		const var_idx_type m_offset;
	};

	template <typename T>
	prop_proxy<T, void> make_proxy(T& data,
		msg_queue_base& msg_queue,
		const var_idx_type& offset)
	{
		return prop_proxy<T, void>(data, msg_queue, offset);
	}

}