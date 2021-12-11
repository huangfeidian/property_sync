#pragma once

#include "property_basic.h"

namespace spiritsaway::property
{

	template <typename T>
	class property_item
	{
	protected:
		T m_id;

	public:
		using key_type = T;
		const T& id() const
		{
			return m_id;
		}
		property_item()
		{

		}
		property_item(
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
		json encode_with_flag(const property_flags flag) const
		{
			json result;
			result["id"] = m_id;
			return result;
		}
		bool filter_with_flag(property_flags flag, property_offset offset, property_cmd cmd, const json& data, json& result_data) const
		{
			return false;
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
		bool operator==(const property_item& other) const
		{
			return m_id == other.m_id;
		}
		bool operator!=(const property_item& other) const
		{
			return m_id != other.m_id;
		}
		friend void swap(property_item& a, property_item& b)
		{
			std::swap(a.m_id, b.m_id);
		}
		bool has_default_value() const
		{
			return false;
		}
		bool replay_mutate_msg(property_offset offset, property_cmd cmd, const json& data)
		{
			return false;
		}
	};


	template <typename Item>
	class property_bag final
	{

	public:
		static_assert(std::is_base_of<property_item<typename Item::key_type>, Item>::value,
			"item should be derived from property_item<K>");
		using key_type = typename Item::key_type;
		using value_type = Item;
		const static std::uint8_t index_for_item = 0;
	protected:
		std::vector<Item> m_data;
		std::unordered_map<key_type, std::uint64_t> m_index;
		
	public:

		property_bag() 
		{

		}
		property_bag& operator=(const property_bag& other)
		{

			m_data = other.m_data;
			m_index = other.m_index;
			return *this;
		}

		json encode() const
		{
			return spiritsaway::serialize::encode(m_data);
		}
		json encode_with_flag(const property_flags flag) const
		{
			json::array_t json_data;
			json_data.resize(m_data.size());
			for (std::uint32_t i = 0; i < m_data.size(); i++)
			{
				json_data[i] = m_data[i].encode_with_flag(flag);
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
				insert(temp_item);
				
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
		bool insert(const value_type& temp_item)
		{
			auto cur_iter = m_index.find(temp_item.id());
			if (cur_iter == m_index.end())
			{
				m_index[temp_item.id()] = m_data.size();
				m_data.push_back(temp_item);
				return true;
			}
			else
			{
				m_data[cur_iter->second] = temp_item;
				return false;
			}
		}

		bool operator==(const property_bag& other) const
		{
			return m_data == other.m_data;
		}
		bool operator!=(const property_bag& other) const
		{
			return m_data != other.m_data;
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
		bool erase(const key_type& key)
		{
			auto cur_iter = m_index.find(key);
			if (cur_iter == m_index.end())
			{
				return false;
			}
			else
			{
				if (cur_iter->second == m_data.size() - 1)
				{
					m_index.erase(cur_iter);
					m_data.pop_back();
				}
				else
				{
					auto pre_index = cur_iter->second;
					m_index.erase(cur_iter);
					m_index[m_data[pre_index].id()] = pre_index;
					std::swap(m_data[pre_index], m_data.back());
					m_data.pop_back();
				}
				return true;
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
				return &m_data[cur_iter->second];
			}
		}

		bool has_default_value() const
		{
			return m_data.empty();
		}
		
		bool filter_insert_with_flags(property_flags flag, property_offset offset, property_cmd cmd, const json& data, json& result_data) const
		{
			if (!data.is_object())
			{
				assert(false);
				return false;
			}
			auto json_iter = data.find("id");
			if (json_iter == data.end())
			{
				assert(false);
				return false;
			}
			key_type key;
			if (!spiritsaway::serialize::decode(*json_iter, key))
			{
				assert(false);
				return false;
			}
			auto cur_iter = m_index.find(key);
			if (cur_iter == m_index.end())
			{
				assert(false);
				return false;
			}
			result_data = m_data[cur_iter->second].encode_with_flag(flag);
			return true;
			
		}
		bool filter_change_with_flags(property_flags flag, property_offset offset, property_cmd cmd, const json& data, json& result_data) const
		{
			std::uint64_t mutate_idx = 0;
			std::uint64_t field_idx;
			std::uint8_t field_cmd;
			json mutate_content;
			if (!serialize::decode_multi(data, mutate_idx, field_idx, field_cmd, mutate_content))
			{
				assert(false);
				return false;
			}
			if (mutate_idx >= m_data.size())
			{
				assert(false);
				return false;
			}
			json temp_result;
			if (m_data[mutate_idx].filter_with_flag(flag, field_idx, property_cmd(field_cmd), mutate_content, temp_result))
			{
				result_data = spiritsaway::serialize::encode_multi(mutate_idx, field_idx, field_cmd, temp_result);
				
				return true;
			}
			else
			{
				return false;
			}
		}
	protected:
		std::unique_ptr<prop_record_proxy<Item>> get(msg_queue_base& parent_queue,
			property_offset parent_offset, property_flags parent_flag, const key_type& key)
		{
			auto cur_iter = m_index.find(key);

			if (cur_iter == m_index.end())
			{
				return nullptr;
			}
			else
			{
				return std::make_unique<prop_record_proxy<Item>>(m_data[cur_iter->second], parent_queue, parent_offset , parent_flag, cur_iter->second);
			}
		}
		bool replay_insert(const json& data)
		{
			value_type temp_item;
			if (!temp_item.decode(data))
			{
				return false;
			}
			insert(temp_item);
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
			return erase(cur_k);
		}
		bool replay_item_mutate(const json& data)
		{
			std::uint64_t mutate_idx = 0;
			std::uint64_t field_idx;
			std::uint8_t field_cmd;
			json mutate_content;
			if (!serialize::decode_multi(data, mutate_idx, field_idx, field_cmd, mutate_content))
			{
				return false;
			}
			if (mutate_idx >= m_data.size())
			{
				return false;
			}
			return m_data[mutate_idx].replay_mutate_msg(field_idx, property_cmd(field_cmd), mutate_content);
		}
	public:
		bool replay_mutate_msg(property_offset offset, property_cmd cmd, const json& data)
		{
			if (offset.value() != 0)
			{
				return false;
			}
			switch (cmd)
			{
			case property_cmd::clear:
				return replay_clear(data);
			case property_cmd::bag_insert:
				return replay_insert(data);
			case property_cmd::bag_erase:
				return replay_erase(data);
			case property_cmd::item_change:

				return replay_item_mutate(data);
			default:
				return false;
			}
		}
		bool filter_with_flag(property_flags flag, property_offset offset, property_cmd cmd, const json& data, json& result_data) const
		{
			if (offset.value() != 0)
			{
				return false;
			}
			switch (cmd)
			{
			case spiritsaway::property::property_cmd::bag_insert:
				return filter_insert_with_flags(flag, offset, cmd, data, result_data);
			case spiritsaway::property::property_cmd::item_change:
				return filter_change_with_flags(flag, offset, cmd, data, result_data);
			default:
				result_data = data;
				return true;
			}
		}
		friend class prop_record_proxy<property_bag<Item>>;
	};
	template <typename T>
	class prop_record_proxy<property_bag<T>>
	{

		property_bag<T>& m_data;
		msg_queue_base& m_queue;
		const property_offset m_offset;
		const property_flags m_flag;
	public:
		using key_type = typename T::key_type;
		using value_type = T;
		prop_record_proxy(property_bag<T>& data,
			msg_queue_base& msg_queue,
			const property_offset& offset,
			const property_flags& flag)
			:m_data(data)
			, m_queue(msg_queue)
			, m_offset(offset)
			, m_flag(flag)
		{

		}
		operator const std::vector<value_type>& () const
		{
			return m_data.m_data;
		}

		void clear()
		{
			m_data.clear();
			m_queue.add(m_offset, property_cmd::clear, json());
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
			m_data.insert(value);
			m_queue.add(m_offset, property_cmd::bag_insert, m_flag,
				value.encode());
		}

		void insert(const json& value)
		{
			value_type one_item;
			if (!one_item.decode(value))
			{
				return;
			}
			insert(one_item);
		}
		


		void erase(const key_type& key)
		{
			if (m_data.erase(key))
			{
				m_queue.add(m_offset, property_cmd::bag_erase, m_flag, serialize::encode(key));
			}

		}
		std::unique_ptr<prop_record_proxy<value_type>> get(const key_type& key)
		{
			return m_data.get(m_queue, m_offset, m_flag, key);
		}

	};

}