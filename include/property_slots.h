#pragma once

#include "property_basic.h"

namespace spiritsaway::property
{

	template <typename T>
	class property_slot_item
	{
	protected:
		T m_id;
		std::uint32_t m_slot = 0;

	public:
		using key_type = T;
		const T& id() const
		{
			return m_id;
		}
		std::uint32_t slot() const
		{
			return m_slot;
		}
		void set_slot(std::uint32_t new_slot)
		{
			m_slot = new_slot;
		}
		property_slot_item()
		{

		}
		property_slot_item(const T& id, const std::uint32_t slot)
		 : m_id(id)
		 , m_slot(slot)
		{

		}
		json encode() const
		{
			json result;
			result["id"] = m_id;
			result["slot"] = m_slot;
			return result;
		}
		void encode_with_flag(const property_flags flag, bool ignore_default, json::object_t& result) const
		{
			result["id"] = m_id;
			result["slot"] = m_slot;
		}

		void encode_with_flag(const property_flags flag, bool ignore_default, json::array_t& result) const
		{
			result.push_back(spiritsaway::serialize::encode(std::make_pair(0, m_id)));
			result.push_back(spiritsaway::serialize::encode(std::make_pair(1, m_slot)));
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
			auto id_iter = data.find("id");
			if (id_iter == data.end())
			{
				return false;
			}
			if (!spiritsaway::serialize::decode(id_iter->second, m_id))
			{
				return false;
			}
			auto slot_iter = data.find("slot");
			if(slot_iter == data.end())
			{
				return false;
			}
			if (!spiritsaway::serialize::decode(slot_iter->second, m_slot))
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
			if (data.size() < 2)
			{
				return false;
			}
			if (data[0].first != next_idx)
			{
				return false;
			}
			next_idx++;
			if(!spiritsaway::serialize::decode(data[0].second, m_id))
			{
				return false;
			}
			if(data[1].first != next_idx)
			{
				return false;
			}
			next_idx++;
			if(!spiritsaway::serialize::decode(data[1].second, m_slot))
			{
				return false;
			}
			return true;

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
		bool operator==(const property_slot_item& other) const
		{
			return m_id == other.m_id && m_slot == other.m_slot;
		}
		bool operator!=(const property_slot_item& other) const
		{
			return m_id != other.m_id || m_slot != other.m_slot;
		}
		friend void swap(property_slot_item& a, property_slot_item& b)
		{
			std::swap(a.m_id, b.m_id);
			std::swap(a.m_slot, b.m_slot);
		}
		bool has_default_value() const
		{
			return false;
		}
		bool replay_mutate_msg(property_replay_offset offset, property_cmd cmd, const json& data)
		{
			return false;
		}
	};


	


	template <typename Item>
	class property_slot_bag
	{
	public:
		static_assert(std::is_base_of<property_slot_item<typename Item::key_type>, Item>::value,
			"item should be derived from property_slot_item<K>");
		using key_type = typename Item::key_type;
		using value_type = Item;
		const static std::uint8_t index_for_item = 0;
	protected:
		const static std::uint8_t bit_mask_sz = sizeof(std::uint32_t) * 8;
		std::vector<std::unique_ptr<Item>> m_data;
		std::vector<std::uint32_t> m_used_slots; // true for used false for empty
		std::unordered_map<key_type, std::uint32_t> m_index;
	public:
		property_slot_bag()
		{

		}
		const std::unordered_map<key_type, std::uint32_t>& index() const
		{
			return m_index;
		}
		json encode() const
		{
			json::array_t data_arr;
			data_arr.reserve(m_index.size());
			for (const auto& one_slot : m_data)
			{
				if (one_slot)
				{
					data_arr.push_back(one_slot->encode());
				}
			}
			json result;
			result["sz"] = m_data.size();
			result["data"] = std::move(data_arr);
			return result;
		}
		json encode_with_flag(const property_flags flag, bool ignore_default, bool replace_key_by_index) const
		{
			json::array_t data_arr;
			data_arr.reserve(m_index.size());
			for (const auto& one_slot : m_data)
			{
				if (one_slot)
				{
					data_arr.push_back(one_slot->encode_with_flag(flag, ignore_default, replace_key_by_index));
				}
			}
			json result;
			result["sz"] = m_data.size();
			result["data"] = std::move(data_arr);
			return result;
		}

		const Item* get_slot(std::uint32_t slot) const
		{
			if (slot >= m_data.size())
			{
				return nullptr;
			}
			return m_data[slot].get();
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

		bool decode(const json& data)
		{
			if (!m_data.empty())
			{
				// std::cout << "the bag is not empty while decode data " << data.dump() << " to property_slot_bag " << std::endl;
				return false;
			}
			if (!data.is_object())
			{
				return false;
			}
			auto sz_iter = data.find("sz");
			auto data_iter = data.find("data");
			if (sz_iter == data.end() || data_iter == data.end())
			{
				return false;
			}

			if (!sz_iter->is_number_unsigned())
			{
				return false;
			}
			if (!data_iter->is_array())
			{
				return false;
			}
			m_data.resize(sz_iter->get<std::uint32_t>());
			m_used_slots.swap(std::vector<std::uint32_t>((m_data.size() + bit_mask_sz - 1) / bit_mask_sz));
			for (const auto& one_slot_json : data_iter->get<json::array_t>())
			{
				std::unique_ptr< value_type> temp_slot = std::make_unique<value_type>();
				if (!temp_slot->decode(one_slot_json))
				{
					return false;
				}
				if (m_index.find(temp_slot->id()) != m_index.end())
				{
					return false;
				}
				insert(std::move(temp_slot));
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
		friend void swap(property_slot_bag& a, property_slot_bag& b)
		{
			std::swap(a.m_index, b.m_index);
			std::swap(a.m_data, b.m_data);
			std::swap(a.m_used_slots, b.m_used_slots);
		}
		void clear()
		{
			m_index.clear();
			m_data.clear();
			m_used_slots.clear();
		}
		bool has_default_value() const
		{
			return m_data.empty();
		}
		bool full() const
		{
			return m_index.size() == m_data.size();
		}
		bool erase(std::uint32_t slot)
		{
			if (slot >= m_data.size() || !m_data[slot])
			{
				return false;
			}
			const auto& cur_item = *m_data[slot];
			m_index.erase(cur_item.id());
			m_used_slots[slot / bit_mask_sz] -= (1 << (slot % bit_mask_sz));
			return true;
		}
	public:


		std::uint32_t get_first_empty_slot() const
		{
			for(std::uint32_t i = 0; i< m_used_slots.size(); i++)
			{
				if(m_used_slots[i] == std::numeric_limits<std::uint32_t>::max())
				{
					continue;
				}
				for(std::uint8_t j = 0; j <bit_mask_sz; j++)
				{
					if(!(m_used_slots[i] & (std::uint32_t(1) << j)))
					{
						return i * bit_mask_sz + j;
					}
				}
			}
			return m_used_slots.size() * bit_mask_sz;
		}

		
	protected:
		bool insert(std::unique_ptr<Item> temp_item)
		{
			auto cur_slot = temp_item->slot();
			if (cur_slot >= m_data.size())
			{
				return false;
			}
			if (m_data[cur_slot])
			{
				return false;
			}
			if (m_index.find(temp_item->id()) != m_index.end())
			{
				return false;
			}
			auto cur_slot = temp_item->slot();
			m_data[cur_slot] = std::move(temp_item);
			m_used_slots[cur_slot / bit_mask_sz] |= (1 << (cur_slot % bit_mask_sz));
			m_index[m_data[cur_slot].id()] = cur_slot;
			return true;
		}
		std::unique_ptr<prop_record_proxy<Item>> get(msg_queue_base& parent_queue,
			property_record_offset parent_offset, property_flags parent_flag, const key_type& key)
		{
			auto cur_iter = m_index.find(key);

			if (cur_iter == m_index.end())
			{
				return nullptr;
			}
			else
			{
				return std::make_unique<prop_record_proxy<Item>>(m_data[cur_iter->second], parent_queue, parent_offset, parent_flag, cur_iter->second);
			}
		}

		std::unique_ptr<prop_record_proxy<Item>> get_slot(msg_queue_base& parent_queue,
			property_record_offset parent_offset, property_flags parent_flag, std::uint32_t slot)
		{
			if(slot >= m_data.size() || !m_data[slot])
			{
				return nullptr;
				
			}
			return std::make_unique<prop_record_proxy<Item>>(*m_data[slot], parent_queue, parent_offset, parent_flag, slot);
		}
		
		bool replay_insert(const json& data)
		{
			std::unique_ptr<value_type> temp_item = std::make_unique<value_type>();
			if (!temp_item->decode(data))
			{
				return false;
			}
			
			return insert(std::move(temp_item));
			
		}
		bool replay_clear(const json& data)
		{
			clear();
			return true;
		}
		bool replay_erase(const json& data)
		{
			std::uint32_t cur_slot;
			if (!serialize::decode(data, cur_slot))
			{
				return false;
			}
			return erase(cur_slot);
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
			if (mutate_idx >= m_data.size() || !m_data[mutate_idx])
			{
				return false;
			}
			return m_data[mutate_idx]->replay_mutate_msg(field_offset, property_cmd(field_cmd), mutate_content);
		}
		bool replay_set(const json& data)
		{
			property_slot_bag temp;
			if (!temp.decode(data))
			{
				return false;
			}
			std::swap(*this, temp);
			return true;

		}
		void resize(std::uint32_t new_sz)
		{
			auto pre_sz = m_data.size();
			if (new_sz < m_data.size())
			{
				for (std::uint32_t i = new_sz; i++; i < pre_sz)
				{
					erase(i);
				}
			}
			m_data.resize(new_sz);
			m_used_slots.resize((new_sz + bit_mask_sz - 1) / bit_mask_sz);
		}
		bool replay_resize(const json& data)
		{
			std::uint32_t new_sz;
			if (!spiritsaway::serialize::decode(data, new_sz))
			{
				return false;
			}
			if (new_sz == m_data.size())
			{
				return true;
			}
			resize(new_sz);
			return true;
		}
		bool swap_slot(std::uint32_t slot_a, std::uint32_t slot_b)
		{
			if (slot_a == slot_b)
			{
				return false;
			}
			Item* pre_item_a = get_slot(slot_a);
			Item* pre_item_b = get_slot(slot_b);
			if (!pre_item_a || !pre_item_b)
			{
				return false;
			}
			pre_item_a->set_slot(slot_b);
			pre_item_b->set_slot(slot_a);
			m_index[pre_item_a->id()] = slot_b;
			m_index[pre_item_b->id()] = slot_a;
			return true;
		}
		bool replay_swap_slot(const json& data)
		{
			std::uint32_t slot_a;
			std::uint32_t slot_b;
			if (!spiritsaway::serialize::decode_multi(data, slot_a, slot_b))
			{
				return false;
			}
			return swap_slot(slot_a, slot_b);
			
		}
		bool move_slot(std::uint32_t slot_from, std::uint32_t slot_to)
		{
			if (slot_from == slot_to)
			{
				return false;
			}
			Item* pre_item_from = get_slot(slot_from);
			Item* pre_item_to = get_slot(slot_to);
			if (!pre_item_from || pre_item_b)
			{
				return false;
			}
			pre_item_from->set_slot(slot_to);
			m_index[pre_item_from->id()] = slot_to;
			m_used_slots[slot_from / bit_mask_sz] -= (1 << (slot_from % bit_mask_sz));
			m_used_slots[slot_to / bit_mask_sz] |= (1 << (slot_to % bit_mask_sz));
			return true;
		}
		bool replay_move_slot(const json& data)
		{
			std::uint32_t slot_from, slot_to;
			if (!spiritsaway::serialize::decode_multi(data, slot_from, slot_to))
			{
				return false;
			}
			return move_slot(slot_from, slot_to);
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
			case property_cmd::slot_swap:
				return replay_swap_slot(data);
			case property_cmd::slot_resize:
				return replay_resize(data);
			case property_cmd::slot_move:
				return replay_move_slot(data);
			default:
				return false;
			}
		}
		friend class prop_record_proxy<property_slot_bag<Item>>;
	};

	template <typename T>
	class prop_record_proxy<property_slot_bag<T>>
	{

		property_slot_bag<T>& m_data;
		msg_queue_base& m_queue;
		const property_record_offset m_offset;
		const property_flags m_flag;
	public:
		using key_type = typename T::key_type;
		using value_type = T;
		prop_record_proxy(property_slot_bag<T>& data,
			msg_queue_base& msg_queue,
			const property_record_offset& offset,
			const property_flags& flag)
			:m_data(data)
			, m_queue(msg_queue)
			, m_offset(offset)
			, m_flag(flag)
		{

		}
		const property_slot_bag<T>& data() const
		{
			return m_data;
		}

		void clear()
		{
			m_data.clear();
			if (m_queue.is_flag_need(m_flag))
			{
				m_queue.add_multi(m_offset, property_cmd::clear, m_flag, json());
			}
		}

		void insert(const value_type& value)
		{
			auto new_value_ptr = std::make_unique<value_type>(value);
			return insert(std::move(new_value_ptr));

		}

		void set(const json& other)
		{
			property_slot_bag new_bag;
			if (!new_bag.decode(other))
			{
				return;
			}
			std::swap(m_data, other);
			for (auto one_need_flag : m_queue.m_need_flags)
			{
				if (one_need_flag.include_by(m_flag))
				{
					auto one_encode_result = other.encode_with_flag(one_need_flag);

					m_queue.add(m_offset, property_cmd::set, one_need_flag, one_encode_result);
				}
			}
		}

		void insert(const json& value)
		{
			auto new_value_ptr = std::make_unique<value_type>();
			if (!new_value_ptr->decode(value))
			{
				return;
			}
			insert(std::move(new_value_ptr));
		}



		bool erase(const key_type& key)
		{
			auto pre_data = m_data.get(key);
			if (!pre_data)
			{
				return false;
			}
			auto cur_slot = pre_data->slot();
			m_data.erase(cur_slot);
			if (pre_data && m_queue.is_flag_need(m_flag))
			{
				m_queue.add_multi(m_offset, property_cmd::erase, m_flag, serialize::encode(cur_slot));
			}
			return true;

		}
		bool erase_by_slot(std::uint32_t cur_slot)
		{
			auto pre_data = m_data.get_slot(cur_slot);
			if (!pre_data)
			{
				return false;
			}
			m_data.erase(cur_slot);
			if (pre_data && m_queue.is_flag_need(m_flag))
			{
				m_queue.add_multi(m_offset, property_cmd::erase, m_flag, serialize::encode(cur_slot));
			}
			return true;
		}

		void resize(std::uint32_t new_sz)
		{
			m_data.resize(new_sz);
			if (m_queue.is_flag_need(m_flag))
			{
				m_queue.add_multi(m_offset, property_cmd::slot_resize, m_flag, serialize::encode(new_sz));
			}
		}

		bool swap_slot(std::uint32_t slot_a, std::uint32_t slot_b)
		{
			if (!m_data.swap_slot(slot_a, slot_b))
			{
				return false;
			}
			if (m_queue.is_flag_need(m_flag))
			{
				m_queue.add_multi(m_offset, property_cmd::slot_swap, m_flag, serialize::encode_multi(slot_a, slot_b));
			}
			return true;
		}

		bool move_slot(std::uint32_t slot_from, std::uint32_t slot_to)
		{
			if (!m_data.move_slot(slot_from, slot_to))
			{
				return false;
			}
			if (m_queue.is_flag_need(m_flag))
			{
				m_queue.add_multi(m_offset, property_cmd::slot_move, m_flag, serialize::encode_multi(slot_from, slot_to));
			}
			return true;

		}
		
		std::unique_ptr<prop_record_proxy<value_type>> get(const key_type& key)
		{
			return m_data.get(m_queue, m_offset, m_flag, key);
		}
		std::unique_ptr<prop_record_proxy<value_type>> get_slot(std::uint32_t cur_slot)
		{
			return m_data.get_slot(m_queue, m_offset, m_flag, cur_slot);
		}


	protected:
		void insert(std::unique_ptr<value_type> value)
		{
			auto cur_slot = value->slot();
			auto insert_result = m_data.insert(std::move(value));
			if (!insert_result)
			{
				return;
			}

			for (auto one_need_flag : m_queue.m_need_flags)
			{
				if (one_need_flag.include_by(m_flag))
				{
					auto one_encode_result = m_data.m_data[cur_slot]->encode_with_flag(one_need_flag, m_queue.m_encode_ignore_default, m_queue.m_encode_with_array);

					m_queue.add(m_offset, property_cmd::add, one_need_flag, one_encode_result);
				}
			}

		}
	};
}