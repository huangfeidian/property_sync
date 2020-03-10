﻿#include <any_container/decode.h>
#include <any_container/any_value.h>

namespace spiritsaway::serialize
{
	// 每个变量在当前定义内只能是uint8的整数索引 所以一个类里面只能定义255个变量 
	// 0号变量保留不使用 以后可以扩充为std::uint16_t 或者与var_mutate_cmd合并为一个std::uint16_t 
	using var_idx_type = std::uint8_t;
	using var_cmd_type = std::uint8_t;// 对于变量的改变操作类型 全量赋值 清空 等等
	const static std::uint8_t depth_max = 8;
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
	
	using mutate_msg = std::tuple<var_prefix_idx_type, var_idx_type, var_mutate_cmd, json>;
	class msg_queue_base
	{
	public:
		virtual void add(const var_idx_type& offset, var_mutate_cmd _cmd, const json& _data) = 0;
	};
	class msg_queue : public msg_queue_base
	{
		std::deque<mutate_msg>& _queue;
		const var_prefix_idx_type& parent_idxes;
	public:
		msg_queue(std::deque<mutate_msg>& _in_msg_queue,
			const var_prefix_idx_type& _in_parent_idxes)
			: _queue(_in_msg_queue)
			, parent_idxes(_in_parent_idxes)
		{

		}
		msg_queue(const msg_queue& other) = default;
		void add(const var_idx_type& offset, var_mutate_cmd _cmd, const json& _data)
		{
			_queue.emplace_back(parent_idxes, offset, _cmd, _data);
			return;
		}

	};
	template <typename T>
	class item_msg_queue : public msg_queue_base
	{
		std::deque<mutate_msg>& _queue;
		const var_prefix_idx_type& parent_idxes;
		const T& _item_key;
	public:
		item_msg_queue(std::deque<mutate_msg>& _in_msg_queue,
			const var_prefix_idx_type& _in_parent_idxes,
			const T& _in_key)
			: _queue(_in_msg_queue)
			, parent_idxes(_in_parent_idxes)
			, _item_key(_in_key)
		{

		}
		item_msg_queue(const item_msg_queue& other) = default;
		void add(const var_idx_type& offset, var_mutate_cmd _cmd, const json& _data)
		{
			std::tuple<T, var_idx_type, var_mutate_cmd, json> _new_data;
			_queue.emplace_back(parent_idxes, 0, _cmd, encode_multi(_item_key, offset, _cmd, _data));
			return;
		}
	};
    template <typename T, typename B = void>
	class prop_proxy;
	template <typename T>
    class prop_proxy<T, std::enable_if_t<
		std::is_pod_v<T> || std::is_same_v<T, std::string>, void>
	>
    {
    public:
        prop_proxy(T& _in_data, 
			msg_queue_base& _in_msg_queue, 
			const var_idx_type& in_offset,
			notify_kind in_notify_kind = notify_kind::self_notify):
        _data(_in_data),
		_msg_queue(_in_msg_queue),
		_notify_kind(in_notify_kind),
		_offset(in_offset)

        {

        }
		T& get()
		{
			return _data;
		}
		operator const T&() const
		{
			return _data;
		}
		void set(const T& _in_data)
		{
			_data = _in_data;
			if (_notify_kind != notify_kind::no_notify)
			{
				_msg_queue.add(_offset,
					var_mutate_cmd::set, encode(_data));
			}
			
		}
		
		void clear()
		{
			_data = {};
			if (_notify_kind != notify_kind::no_notify)
			{
				_msg_queue.add(_offset,
					var_mutate_cmd::clear, json());
			}
		}
		
		bool replay(var_mutate_cmd _cmd, const json& j_data)
		{
			switch (_cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(j_data);
			case var_mutate_cmd::set:
				return replay_set(j_data);
			default:
				return false;
			}
		}
	private:
		bool replay_set(const json& j_data)
		{
			return decode(j_data, _data);
		}
		bool replay_clear(const json& j_data)
		{
			_data = {};
			return true;
		}
    private:
        T& _data;
		msg_queue_base& _msg_queue;
		const var_idx_type& _offset;
		const notify_kind _notify_kind;
	};

	template<typename T>
	class prop_proxy<std::vector<T>>
	{
	public:
		prop_proxy(std::vector<T>& _in_data, 
			msg_queue_base& _in_msg_queue, 
			const var_idx_type& _in_offset) :
			_data(_in_data),
			_msg_queue(_in_msg_queue),
			_offset(_in_offset)
		{

		}
		std::vector<T>& get()
		{
			return _data;
		}
		operator const std::vector<T>&() const
		{
			return _data;
		}
		void set(const std::vector<T>& _in_data)
		{
			_data = _in_data;
			_msg_queue.add(_offset, var_mutate_cmd::set, encode(_data));
		}
		
		void clear()
		{
			_data.clear();
			_msg_queue.add(_offset, var_mutate_cmd::clear, json());
		}
		
		void push_back(const T& _new_data)
		{
			_data.push_back(_new_data);
			_msg_queue.add(_offset, var_mutate_cmd::vector_push_back, encode(_new_data));
		}
		
		void pop_back()
		{
			if (_data.size())
			{
				_data.pop_back();
			}
			_msg_queue.add(_offset, var_mutate_cmd::vector_pop_back, json());
		}
		
		void idx_mutate(std::size_t idx, const T& _new_data)
		{
			if (idx < _data.size())
			{
				_data[idx] = _new_data;
			}
			_msg_queue.add(_offset, var_mutate_cmd::vector_idx_mutate, encode_multi(idx, _new_data));
		}
		
		void idx_delete(std::size_t idx)
		{
			if (idx < _data.size())
			{
				_data.erase(_data.begin() + idx);
			}
			_msg_queue.add(_offset, var_mutate_cmd::vector_idx_mutate, encode(idx));
		}
		
		bool replay(var_mutate_cmd _cmd, const json& j_data)
		{
			switch (_cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(j_data);
			case var_mutate_cmd::set:
				return replay_set(j_data);
			case var_mutate_cmd::vector_push_back:
				return replay_push_back(j_data);
			case var_mutate_cmd::vector_pop_back:
				return replay_pop_back(j_data);
			case var_mutate_cmd::vector_idx_mutate:
				return replay_idx_mutate(j_data);
			case var_mutate_cmd::vector_idx_delete:
				return replay_idx_delete(j_data);
			default:
				return false;
			}
		}
	private:
		bool replay_set(const json& j_data)
		{
			return decode(j_data, _data);
		}
		bool replay_clear(const json& j_data)
		{
			_data.clear();
			return true;
		}
		bool replay_push_back(const json& j_data)
		{
			T temp;
			if (decode(j_data, temp))
			{
				_data.push_back(temp);
				return true;
			}
			else
			{
				return false;
			}
		}
		bool replay_pop_back(const json& j_data)
		{
			if (_data.size())
			{
				_data.pop_back();
			}
			return true;
		}
		bool replay_idx_mutate(const json& j_data)
		{
			std::size_t idx;
			T temp;
			if (!decode_multi(j_data, idx, temp))
			{
				return false;
			}
			if (idx < _data.size())
			{
				_data[idx] = temp;
			}
			return true;
		}
		bool replay_idx_delete(const json& j_data)
		{
			std::size_t idx;
			if (!decode(j_data, idx))
			{
				return false;
			}
			if (idx < _data.size())
			{
				_data.erase(_data.begin() + idx);
			}
			return true;
		}
	private:
		std::vector<T>& _data;
		msg_queue_base& _msg_queue;
		const var_idx_type _offset;
	};

	template <typename T1, typename T2>
	class prop_proxy<std::unordered_map<T1, T2>>
	{
	public:
		prop_proxy(std::unordered_map<T1, T2>& _in_data,
			msg_queue_base& _in_msg_queue,
			const var_idx_type& _in_offset) :
			_data(_in_data),
			_msg_queue(_in_msg_queue),
			_offset(_in_offset)
		{

		}
		std::unordered_map<T1, T2>& get()
		{
			return _data;
		}
		operator const std::unordered_map<T1, T2>&() const
		{
			return _data;
		}
		void set(const std::unordered_map<T1, T2>& _in_data)
		{
			_data = _in_data;
			_msg_queue.add(_offset, var_mutate_cmd::set, encode(_data));
		}

		void clear()
		{
			_data.clear();
			_msg_queue.add(_offset, var_mutate_cmd::clear, json());
		}

		void insert(const T1& key, const T2& value)
		{
			_data[key] = value;
			_msg_queue.add(_offset, var_mutate_cmd::map_insert, encode_multi(key, value));
		}

		void erase(const T1& key)
		{
			_data.erase(key);
			_msg_queue.add(_offset, var_mutate_cmd::map_erase, encode(key));
		}

		bool replay(var_mutate_cmd _cmd, const json& j_data)
		{
			switch (_cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(j_data);
			case var_mutate_cmd::set:
				return replay_set(j_data);
			case var_mutate_cmd::map_insert:
				return replay_insert(j_data);
			case var_mutate_cmd::map_erase:
				return replay_erase(j_data);
			default:
				return false;
			}
		}
	private:
		bool replay_set(const json& j_data)
		{
			return decode(j_data, _data);
		}
		bool replay_clear(const json& j_data)
		{
			_data.clear();
			return true;
		}
		bool replay_insert(const json& j_data)
		{
			T1 key;
			T2 value;
			if (!decode_multi(j_data, key, value))
			{
				return false;
			}
			_data[key] = value;
			return true;
		}
		bool replay_erase(const json& j_data)
		{
			T1 key;
			if (!decode(j_data, key))
			{
				return false;
			}
			_data.erase(key);
			return true;
		}
	private:
		std::unordered_map<T1, T2>& _data;
		msg_queue_base& _msg_queue;
		const var_idx_type _offset;
	};

	
	class property_item_base;
	class property_bag_base;
	class property_item_base
	{
	private:
		property_bag_base* _container;
	public:
		property_item_base(property_bag_base* _in_container)
			: _container(_in_container)
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
		var_prefix_idx_type _depth;
		virtual json encode() const = 0;
		virtual bool decode(const json& data) = 0;
		virtual const std::string& type_name() const = 0;
		std::deque<mutate_msg>& _dest_buffer;
		property_bag_base(var_prefix_idx_type _in_depth,
			std::deque<mutate_msg>& _in_cmd_queue) :
			_depth(_in_depth),
			_dest_buffer(_in_cmd_queue)
		{

		}
		virtual bool replay_mutate_msg(std::size_t field_index,
			var_mutate_cmd cmd, const json& data) = 0;

	};
	template <typename T>
	class property_item : public property_item_base
	{
	protected:
		T _id;
	public:
		const T& id() const
		{
			return _id;
		}
		property_item(property_bag_base* _in_container,
			std::deque<mutate_msg>& _dest_queue,
			const T& _in_id):
			property_item_base(_in_container),
			_id(_in_id),
			_cmd_buffer(_dest_queue, _in_container->_depth, _id)

		{

		}
		item_msg_queue<T> _cmd_buffer;
		json encode() const
		{
			json result;
			result["id"] = _id;
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
			if (!spiritsaway::serialize::decode(*iter, _id))
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
		std::vector<Item> _data;
		std::unordered_map<K, std::size_t> _index;
	public:
		using key_type = K;
		using value_type = Item;
		property_bag(var_prefix_idx_type _in_depth,
			std::deque<mutate_msg>& _in_cmd_queue) :
			property_bag_base(_in_depth, _in_cmd_queue)
		{

		}
		property_bag& operator=(const property_bag& other)
		{
			_data.clear();
			for (const auto& one_item : other._data)
			{
				insert(one_item.encode());
			}
			_data.shrink_to_fit();
			return *this;
		}

		json encode() const
		{
			return spiritsaway::serialize::encode(_data);
		}
		bool decode(const json& in_data)
		{
			if (_data.size())
			{
				std::cout << "the bag is not empty while decode data " << in_data.dump() << " to property_bag " << type_name() << std::endl;
				return false;
			}
			json::array_t temp_array;
			if (!in_data.is_array())
			{
				return false;
			}
			_data.reserve(in_data.size());
			temp_array = in_data.get<json::array_t>();
			for (const auto& one_json_item : temp_array)
			{
				value_type temp_item(this, _dest_buffer, K());
				if (!temp_item.decode(one_json_item))
				{
					_data.clear();
					return false;
				}
				_data.push_back(temp_item);
			}
			for (std::size_t i = 0; i < _data.size(); i++)
			{
				_index[_data[i].id()] = i;
			}
			return true;
		}
		bool has_item(const K& _key) const
		{
			auto cur_iter = _index.find(_key);
			if (cur_iter == _index.end())
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
			Item temp_item(this, _dest_buffer, K());
			if (!spiritsaway::serialize::decode(data, temp_item))
			{
				std::cout<<"property_bag "<<type_name()<<" fail to create item with data {}"<< data.dump()<<std::endl;
				return false;
			}
			_index[temp_item.id()] = _data.size();
			_data.emplace_back(std::move(temp_item));
			return true;
		}
		Item& create(const K& key)
		{
			Item temp_item(this, _dest_buffer, key);
			_index[temp_item.id()] = _data.size();
			_data.emplace_back(std::move(temp_item));
			return _data.back();
		}
		bool operator==(const property_bag& other)
		{
			return _data == other._data;
		}
		void clear()
		{
			_index.clear();
			_data.clear();
		}
		bool erase(const K& key)
		{
			auto cur_iter = _index.find(key);
			if (cur_iter == _index.end())
			{
				return false;
			}
			else
			{
				if (cur_iter->second == _data.size())
				{
					_index.erase(cur_iter);
					_data.pop_back();
				}
				else
				{
					auto pre_index = cur_iter->second;
					_index.erase(cur_iter);
					_index[_data[pre_index].id()] = pre_index;
					_data[pre_index].swap(_data.back());
					_data.pop_back();
				}
				return true;
			}
		}
		std::optional<std::reference_wrapper<const Item>> get(const K& key) const
		{
			auto cur_iter = _index.find(_key);
			if (cur_iter == _index.end())
			{
				return std::nullopt;
			}
			else
			{
				return std::cref(_data[cur_iter->second]);
			}
		}
		std::optional<std::reference_wrapper<Item>> get_mut(const K& key)
		{
			auto cur_iter = _index.find(_key);
			if (cur_iter == _index.end())
			{
				return std::nullopt;
			}
			else
			{
				return std::ref(_data[cur_iter->second]);
			}
		}
	};
	template <typename T>
	class prop_proxy<T, 
		std::enable_if_t<std::is_base_of_v<property_bag_base, T>, void>>
	{

		T& _data;
	public:
		using key_type = typename T::key_type;
		using value_type = typename T::value_type;
		prop_proxy(T& _in_data,
			msg_queue_base& _in_msg_queue,
			const var_idx_type& _in_offset)
			:_data(_in_data)
			,_msg_queue(_in_msg_queue)
			,_offset(_in_offset)
		{

		}
		operator const std::vector<value_type>&() const
		{
			return _data._data;
		}
		void clear()
		{
			_data.clear();
			_msg_queue.add(_offset, var_mutate_cmd::clear, json());
		}
		std::vector<key_type> keys() const
		{
			std::vector<key_type> result;
			result.reserve(_data._data.size());
			for (const auto& one_item : _data._data)
			{
				result.push_back(one_item.first);
			}
			return result;
		}
		void insert(const json& value)
		{
			if (_data.create(value))
			{
				_msg_queue.add(_offset, var_mutate_cmd::map_insert, 
					value);
			}
			
		}

		void erase(const key_type& key)
		{
			if (_data.erase(key))
			{
				_msg_queue.add(_offset, var_mutate_cmd::map_erase, encode(key));
			}
			
		}
		bool replay_insert(const json& j_data)
		{
			return _data.create(j_data);
			
		}
		bool replay_clear(const json& j_data)
		{
			_data.clear();
			return true;
		}
		bool replay_erase(const json& j_data)
		{
			key_type cur_k;
			if (!decode(j_data, cur_k))
			{
				return false;
			}
			return _data.erase(cur_k);
		}
		bool replay(var_mutate_cmd _cmd, const json& j_data)
		{
			switch (_cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(j_data);
			case var_mutate_cmd::map_insert:
				return replay_insert(j_data);
			case var_mutate_cmd::map_erase:
				return replay_erase(j_data);
			default:
				return false;
			}
		}
	private:
		msg_queue_base& _msg_queue;
		const var_idx_type _offset;
	};

	template <typename T>
	prop_proxy<T, void> make_proxy(T& _in_data,
		msg_queue_base& _in_msg_queue,
		const var_idx_type& _in_offset)
	{
		return prop_proxy<T, void>(_in_data, _in_msg_queue, _in_offset);
	}

}