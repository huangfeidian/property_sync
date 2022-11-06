#pragma once

#include "property_vec.h"

namespace spiritsaway::property
{

	template <typename T>
	class property_bag_item: public property_vec_item
	{
	public:
		T m_id;

	public:
		using key_type = T;
		const T& id() const
		{
			return m_id;
		}
		property_bag_item()
		{

		}
		property_bag_item(
			const T& id) :
			m_id(id)
		{

		}
		json encode() const
		{
			json result;
			result["id"] = m_id;
			return result;
		}
		void encode_with_flag(const property_flags flag, bool ignore_default, json::object_t& result) const
		{
			result["id"] = m_id;
		}

		void encode_with_flag(const property_flags flag, bool ignore_default, json::array_t& result) const
		{
			result.push_back(spiritsaway::serialize::encode(std::make_pair(0, m_id)));
		}
		json encode_with_flag(const property_flags flag, bool ignore_default, bool replace_key_by_index) const
		{
			if (replace_key_by_index)
			{
				json::array_t result;
				encode_with_flag(flag, ignore_default, result);
				return result;
			}
			else
			{
				json::object_t result;
				encode_with_flag(flag, ignore_default, result);
				return result;
			}
		}
		bool decode(const json::object_t& data)
		{
			auto iter = data.find("id");
			if (iter == data.end())
			{
				return false;
			}
			if (!spiritsaway::serialize::decode(iter->second, m_id))
			{
				return false;
			}
			return true;
		}
		bool decode(const std::vector<std::pair<std::uint8_t, json>>& data, std::uint32_t& next_idx)
		{
			if (next_idx != 0)
			{
				return false;
			}
			if (data.size() == 0)
			{
				return false;
			}
			if (data[0].first != 0)
			{
				return false;
			}
			next_idx = 1;
			return spiritsaway::serialize::decode(data[0].second, m_id);
		}
		bool decode(const json& data)
		{
			if (data.is_object())
			{
				return decode(data.get<json::object_t>());
			}
			else if (data.is_array())
			{
				std::vector<std::pair<std::uint8_t, json>> array_data;
				if (!spiritsaway::serialize::decode(data, array_data))
				{
					return false;
				}
				return decode(array_data);
			}
			else
			{
				return false;
			}

			
		}
		bool operator==(const property_bag_item& other) const
		{
			return m_id == other.m_id;
		}
		bool operator!=(const property_bag_item& other) const
		{
			return m_id != other.m_id;
		}
		friend void swap(property_bag_item& a, property_bag_item& b)
		{
			std::swap(a.m_id, b.m_id);
		}
		bool has_default_value() const
		{
			return false;
		}
		bool replay_mutate_msg(property_replay_offset offset, property_cmd cmd, const json& data)
		{
			return false;
		}
		void update_fields(const property_bag_item& other, const json& other_json, std::vector<std::uint8_t>& field_indexes)
		{
			return ;
		}
		void clear_fields(const std::vector<std::uint8_t>& related_indexes)
		{

		}
		bool set_fields(const std::vector<std::pair<std::uint8_t, json>>& field_values)
		{
			return true;
		}
	};


	template <typename Item>
	class property_bag final
	{

	public:
		static_assert(std::is_base_of<property_bag_item<typename Item::key_type>, Item>::value,
			"item should be derived from property_bag_item<K>");
		using key_type = typename Item::key_type;
		using value_type = Item;
		const static std::uint8_t index_for_item = 0;
	protected:
		std::vector<std::unique_ptr<Item>> m_data;
		std::unordered_map<key_type, std::uint32_t> m_index;
		
	public:

		property_bag() 
		{

		}
		property_bag(const property_bag& other)
		{
			m_data.clear();
			m_index.clear();
			m_index = other.m_index;
			m_data.reserve(other.m_data.size());
			for (const auto& one_item : other.m_data)
			{
				m_data.emplace_back(std::make_unique<Item>(*one_item));
			}
		}

		// property_bag& operator=(const property_bag& other)
		// {
		// 	m_data.clear();
		// 	m_index.clear();
		// 	m_index = other.m_index;
		// 	m_data.reserve(other.m_data.size());
		// 	for(const auto& one_item: other.m_data)
		// 	{
		// 		m_data.emplace_back(std::make_unique<Item>(*one_item));
		// 	}
		// 	return *this;
		// }

		const std::unordered_map<key_type, std::uint32_t>& index() const
		{
			return m_index;
		}

		json encode() const
		{
			json::array_t json_data;
			json_data.reserve(m_data.size());
			for(const auto& one_item: m_data)
			{
				json_data.push_back(spiritsaway::serialize::encode(*one_item));
			}
			return json_data;
		}
		json encode_with_flag(const property_flags flag, bool ignore_default, bool replace_key_by_index) const
		{
			json::array_t json_data;
			json_data.resize(m_data.size());
			for (std::uint32_t i = 0; i < m_data.size(); i++)
			{
				json_data[i] = m_data[i]->encode_with_flag(flag, ignore_default, replace_key_by_index);
			}
			return json_data;
		}
		bool decode(const json& data)
		{
			if (m_data.size())
			{
				// std::cout << "the bag is not empty while decode data " << data.dump() << " to property_bag " << std::endl;
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
				value_type temp_item;
				if (!temp_item.decode(one_json_item))
				{
					return false;
				}
				insert(std::move(temp_item));
				
			}
			
			return true;
		}
		bool has_item(const key_type& _key) const
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
		//std::pair<std::uint32_t, bool> insert(const value_type& temp_item)
		//{
		//	value_type new_item = temp_item;
		//	return insert(std::move(new_item));
		//}


		bool operator==(const property_bag& other) const
		{
			if (m_data.size() != other.m_data.size())
			{
				return false;
			}
			for (std::uint32_t i = 0; i < m_data.size(); i++)
			{
				if (m_data[i]->operator==(*other.m_data[i]))
				{
					continue;
				}
				else
				{
					return false;
				}
			}
			return true;
		}
		bool operator!=(const property_bag& other) const
		{
			return !(operator==(other));
		}
		friend void swap(property_bag& a, property_bag& b)
		{
			std::swap(a.m_index, b.m_index);
			std::swap(a.m_data, b.m_data);
		}
		void clear()
		{
			m_index.clear();
			m_data.clear();
		}
		std::unique_ptr<value_type> erase(const key_type& key)
		{
			auto cur_iter = m_index.find(key);
			if (cur_iter == m_index.end())
			{
				return {};
			}
			else
			{
				
				if (cur_iter->second == m_data.size() - 1)
				{
					m_index.erase(cur_iter);
				}
				else
				{
					auto pre_index = cur_iter->second;
					m_index.erase(cur_iter);
					m_index[m_data.back()->id()] = pre_index;
					std::swap(m_data[pre_index], m_data.back());
					
				}
				std::unique_ptr<value_type> result = std::move(m_data.back());
				m_data.pop_back();
				return result;
			}
		}
		const Item* get(const key_type& key) const
		{
			auto cur_iter = m_index.find(key);
			if (cur_iter == m_index.end())
			{
				return nullptr;
			}
			else
			{
				return m_data[cur_iter->second].get();
			}
		}
		

		bool has_default_value() const
		{
			return m_data.empty();
		}
		
		
	protected:
		std::pair<std::uint32_t, bool> insert(value_type&& temp_item)
		{
			auto cur_item_ptr = std::make_unique<value_type>(std::move(temp_item));
			return insert(std::move(cur_item_ptr));
		}
		std::pair<std::uint32_t, bool> insert(std::unique_ptr<value_type> temp_item_ptr)
		{
			auto cur_iter = m_index.find(temp_item_ptr->id());
			if (cur_iter == m_index.end())
			{
				m_index[temp_item_ptr->id()] = std::uint32_t(m_data.size());
				m_data.push_back(std::move(temp_item_ptr));
				return std::make_pair(m_data.size() - 1, true);
			}
			else
			{
				m_data[cur_iter->second] = std::move(temp_item_ptr);
				return std::make_pair(cur_iter->second, false);
			}
		}
		std::pair<std::uint32_t, bool> get_insert(const key_type& key)
		{
			auto cur_iter = m_index.find(key);
			if (cur_iter == m_index.end())
			{
				std::unique_ptr<value_type> temp_item_ptr = std::make_unique<value_type>(key);
				m_index[temp_item_ptr->id()] = std::uint32_t(m_data.size());
				m_data.push_back(std::move(temp_item_ptr));
				return std::make_pair(m_data.size() - 1, true);
			}
			else
			{
				return std::make_pair(cur_iter->second, false);
			}
		}
		std::optional<prop_record_proxy<Item>> get(msg_queue_base& parent_queue,
			property_record_offset parent_offset, property_flags parent_flag, const key_type& key)
		{
			auto cur_iter = m_index.find(key);

			if (cur_iter == m_index.end())
			{
				return {};
			}
			else
			{
				return prop_record_proxy<Item>(*m_data[cur_iter->second], parent_queue, parent_offset , parent_flag, cur_iter->second);
			}
		}

		bool replay_insert(const json& data)
		{
			value_type temp_item;
			if (!temp_item.decode(data))
			{
				return false;
			}
			insert(std::make_unique<value_type>(std::move(temp_item)));
			return true;
		}
		bool replay_clear(const json& data)
		{
			clear();
			return true;
		}
		bool replay_erase(const json& data)
		{
			key_type cur_k;
			if (!serialize::decode(data, cur_k))
			{
				return false;
			}
			return !!erase(cur_k);
		}
		bool replay_item_mutate(const json& data)
		{
			std::uint32_t mutate_idx = 0;
			property_replay_offset field_offset;
			std::uint8_t field_cmd;
			json mutate_content;
			if (!serialize::decode_multi(data, mutate_idx, field_offset, field_cmd, mutate_content))
			{
				return false;
			}
			if (mutate_idx >= m_data.size())
			{
				return false;
			}
			return m_data[mutate_idx]->replay_mutate_msg(field_offset, property_cmd(field_cmd), mutate_content);
		}
		bool replay_set(const json& data)
		{
			using std::swap;
			property_bag temp;
			if (!temp.decode(data))
			{
				return false;
			}
			swap(*this, temp);
			return true;
			
		}
	public:
		bool replay_mutate_msg(property_replay_offset offset, property_cmd cmd, const json& data)
		{
			if (offset.value() != 0)
			{
				return false;
			}
			switch (cmd)
			{
			case property_cmd::clear:
				return replay_clear(data);
			case property_cmd::add:
				return replay_insert(data);
			case property_cmd::erase:
				return replay_erase(data);
			case property_cmd::set:
				return replay_set(data);
			case property_cmd::item_change:
				return replay_item_mutate(data);
			default:
				return false;
			}
		}
		friend class prop_record_proxy<property_bag<Item>>;
	};
	template <typename T>
	class prop_record_proxy<property_bag<T>>
	{

		property_bag<T>& m_data;
		msg_queue_base& m_queue;
	public:
		const property_record_offset m_offset;
		const property_flags m_flag;
	public:
		using key_type = typename T::key_type;
		using value_type = T;
		prop_record_proxy(property_bag<T>& data,
			msg_queue_base& msg_queue,
			const property_record_offset& offset,
			const property_flags& flag)
			:m_data(data)
			, m_queue(msg_queue)
			, m_offset(offset)
			, m_flag(flag)
		{

		}
		const property_bag<T>& data() const
		{
			return m_data;
		}

		void clear()
		{
			m_data.clear();
			if (m_queue.is_flag_need(m_flag))
			{
				m_queue.add(m_offset, property_cmd::clear, m_flag, json());
			}
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
		void insert(const value_type& value)
		{
			value_type new_value = value;
			return insert(std::make_unique<value_type>(std::move(new_value)));
			
		}

		void insert(value_type&& value)
		{
			return insert(std::make_unique<value_type>(std::move(value)));
			
		}

		void set(const property_bag<T>& other)
		{
			m_data = other;
			for (auto one_need_flag : m_queue.m_need_flags)
			{
				if (one_need_flag.include_by(m_flag))
				{
					auto one_encode_result = m_data.encode_with_flag(one_need_flag,m_queue.m_encode_ignore_default, m_queue.m_encode_with_array);

					m_queue.add(m_offset, property_cmd::set, m_flag, one_encode_result);
				}
			}
		}

		void insert(const json& value)
		{
			value_type one_item;
			if (!one_item.decode(value))
			{
				return;
			}
			return insert(std::make_unique<value_type>(std::move(one_item)));
		}
		


		std::unique_ptr<value_type> erase(const key_type& key)
		{
			std::unique_ptr<value_type> result = m_data.erase(key);
			if (result && m_queue.is_flag_need(m_flag))
			{
				m_queue.add(m_offset, property_cmd::erase, m_flag, serialize::encode(key));
			}
			return result;

		}
		std::optional<prop_record_proxy<value_type>> get(const key_type& key)
		{
			return m_data.get(m_queue, m_offset, m_flag, key);
		}
		prop_record_proxy<value_type> get_insert(const key_type& key)
		{
			auto cur_result = m_data.get_insert(key);
			auto& value = *m_data.m_data[cur_result.first];
			if(cur_result.second)
			{
				
				for (auto one_need_flag : m_queue.m_need_flags)
				{
					if (one_need_flag.include_by(m_flag))
					{
						auto one_encode_result = value.encode_with_flag(one_need_flag, m_queue.m_encode_ignore_default, m_queue.m_encode_with_array);

						m_queue.add_for_flag(m_offset, property_cmd::add, one_need_flag, m_flag, one_encode_result);
					}
				}
			}
			return  prop_record_proxy<value_type>(value, m_queue, m_offset, m_flag, cur_result.first);
		}


		void insert(std::unique_ptr<value_type> value_ptr)
		{
			auto insert_result = m_data.insert(std::move(value_ptr));
			for (auto one_need_flag : m_queue.m_need_flags)
			{
				if (one_need_flag.include_by(m_flag))
				{
					auto one_encode_result = m_data.m_data[insert_result.first]->encode_with_flag(one_need_flag, m_queue.m_encode_ignore_default, m_queue.m_encode_with_array);

					m_queue.add_for_flag(m_offset, property_cmd::add, one_need_flag, m_flag, one_encode_result);
				}
			}

		}
	};

}