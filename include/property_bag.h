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


	template <typename Item>
	class property_bag final
	{

	public:
		static_assert(std::is_base_of<property_item<typename Item::key_type>, Item>::value,
			"item should be derived from property_item<K>");
		using key_type = typename Item::key_type;
		using value_type = Item;
		const static var_idx_type index_for_item = 0;
	protected:
		std::vector<Item> m_data;
		std::unordered_map<key_type, std::size_t> m_index;
		
	public:

		property_bag() 
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
				if (!insert(one_json_item))
				{
					return false;
				}
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
		bool insert(const json& one_item)
		{
			value_type temp_item{ key_type() };
			if (!temp_item.decode(one_item))
			{
				return false;
			}
			auto cur_iter = m_index.find(temp_item.id());
			if (cur_iter == m_index.end())
			{
				m_index[temp_item.id()] = m_data.size();
				m_data.push_back(temp_item);
			}
			else
			{
				m_data[cur_iter->second] = temp_item;
			}
			return true;
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
		bool erase(const key_type& key)
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
		


	protected:
		std::unique_ptr<prop_record_proxy<Item>> get(msg_queue_base& parent_queue,
			property_offset parent_offset, const key_type& key)
		{
			auto cur_iter = m_index.find(key);

			if (cur_iter == m_index.end())
			{
				return nullptr;
			}
			else
			{
				return std::make_unique<prop_record_proxy<Item>>(parent_queue, parent_offset,  m_data[cur_iter->second], cur_iter->second);
			}
		}
		bool replay_insert(const json& data)
		{
			Item temp_item{ key_type() };
			if (!spiritsaway::serialize::decode(data, temp_item))
			{
				// std::cout <<" fail to create item with data {}" << data.dump() << std::endl;
				return false;
			}
			m_index[temp_item.id()] = m_data.size();
			m_data.emplace_back(std::move(temp_item));
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
			std::size_t mutate_idx = 0;
			std::size_t field_idx;
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
			return m_data[mutate_idx].replay_mutate_msg(field_idx, var_mutate_cmd(field_cmd), mutate_content);
		}
	public:
		bool replay_mutate_msg(property_offset offset, var_mutate_cmd cmd, const json& data)
		{
			if (offset.value() != 0)
			{
				return false;
			}
			switch (cmd)
			{
			case var_mutate_cmd::clear:
				return replay_clear(data);
			case var_mutate_cmd::map_insert:
				return replay_insert(data);
			case var_mutate_cmd::map_erase:
				return replay_erase(data);
			case var_mutate_cmd::mutate_item:

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
		const property_offset m_offset;
	public:
		using key_type = typename T::key_type;
		using value_type = T;
		prop_record_proxy(property_bag<T>& data,
			msg_queue_base& msg_queue,
			const property_offset offset)
			:m_data(data)
			, m_queue(msg_queue)
			, m_offset(offset)
		{

		}
		operator const std::vector<value_type>& () const
		{
			return m_data.m_data;
		}

		void clear()
		{
			m_data.clear();
			m_queue.add(m_offset, var_mutate_cmd::clear, json());
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
			if (m_data.insert(value))
			{
				m_queue.add(m_offset, var_mutate_cmd::map_insert,
					value);
			}

		}

		void erase(const key_type& key)
		{
			if (m_data.erase(key))
			{
				m_queue.add(m_offset, var_mutate_cmd::map_erase, serialize::encode(key));
			}

		}
		std::unique_ptr<prop_record_proxy<value_type>> get(const key_type& key)
		{
			return m_data.get(m_queue, m_offset, key);
		}
		bool replay(var_mutate_cmd cmd, const json& data)
		{
			return m_data.replay_mutate_msg(cmd, data);
		}

	};

}