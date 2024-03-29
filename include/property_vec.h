#pragma once

#include "property_basic.h"
#include <algorithm>

namespace spiritsaway::property
{
	struct property_vec_item: public property_item
	{
		json encode(bool ignore_default = true) const
		{
			return json::object_t{};
		}

		void encode_with_flag(const property_flags flag, bool ignore_default, json::object_t& result) const
		{
			return;
		}
		void encode_with_flag(const property_flags flag, bool ignore_default, json::array_t& result) const
		{
			return;
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
		std::vector<std::uint8_t> get_fields_with_flag(const property_flags& flag) const
		{
			return {};
		}

		std::vector<std::uint8_t> get_fields_without_flag(const property_flags& flag) const
		{
			return {};
		}

		json encode_fields(const std::vector<std::uint8_t>& offsets, bool ignore_default) const
		{
			return {};
		}

		json encode_except_fields(const std::vector<std::uint8_t>& offsets, bool ignore_default) const
		{
			return encode(ignore_default);
		}

		json encode_fields_with_flag(const std::vector<std::uint8_t>& offsets, const spiritsaway::property::property_flags flag, bool ignore_default) const
		{
			return {};
		}

		void update_fields(const property_vec_item& other, const json& other_json, std::vector<std::uint8_t>& field_indexes)
		{
			return ;
		}
		bool decode(const json::object_t& data)
		{
			return true;
		}
		bool decode(const std::vector<std::pair<std::uint8_t, json>>& data, std::uint32_t& next_idx)
		{
			if (next_idx != 0)
			{
				return false;
			}
			return true;
		}
		bool decode(const json& data)
		{
			return true;
		}

		bool has_default_value() const
		{
			return false;
		}
		bool replay_mutate_msg(property_replay_offset offset, property_cmd cmd, const json& data)
		{
			return false;
		}
		void clear_fields(const std::vector<std::uint8_t>& related_indexes)
		{

		}
		bool set_fields(const std::vector<std::pair<std::uint8_t, json>>& field_values)
		{
			return true;
		}

		friend void swap(property_vec_item& a, property_vec_item& b)
		{
			return;
		}
		bool operator==(const property_vec_item& other) const
		{
			return true;
		}
		bool operator!=(const property_vec_item& other) const
		{
			return false;
		}
	};
	template <typename Item>
	class property_vec final
	{
	public:
		static_assert(std::is_base_of<property_vec_item, Item>::value,
			"item should be derived from property_vec_item");
		using value_type = Item;
		const static std::uint8_t index_for_item = 0;
	protected:
		std::vector<std::unique_ptr<Item>> m_data;
	public:
		property_vec()
		{

		}
		property_vec(const property_vec& other)
		{
			reset_by_other(other);
		}

		property_vec& operator=(const property_vec& other)
		{
			reset_by_other(other);
			return *this;
		}
		std::uint32_t size() const
		{
			return std::uint32_t(m_data.size());
		}
		const Item* back() const
		{
			if (m_data.empty())
			{
				return nullptr;
			}
			return m_data.back().get();
		}
		bool empty() const
		{
			return m_data.empty();
		}
		json encode(bool ignore_default = true) const
		{
			json::array_t data_arr;
			data_arr.reserve(m_data.size());
			for (const auto& one_slot : m_data)
			{
				data_arr.push_back(one_slot->encode(ignore_default));
			}
			return data_arr;
		}
		json encode_with_flag(const property_flags flag, bool ignore_default, bool replace_key_by_index) const
		{
			json::array_t data_arr;
			data_arr.reserve(m_data.size());
			for (const auto& one_slot : m_data)
			{
				data_arr.push_back(one_slot->encode_with_flag(flag, ignore_default, replace_key_by_index));
			}
			return data_arr;
		}

		std::vector<std::uint8_t> get_fields_with_flag(const property_flags& flag) const
		{
			return {};
		}

		std::vector<std::uint8_t> get_fields_without_flag(const property_flags& flag) const
		{
			return {};
		}

		json encode_fields(const std::vector<std::uint8_t>& offsets, bool ignore_default) const
		{
			return {};
		}

		json encode_except_fields(const std::vector<std::uint8_t>& offsets, bool ignore_default) const
		{
			return encode(ignore_default);
		}

		json encode_fields_with_flag(const std::vector<std::uint8_t>& offsets, const spiritsaway::property::property_flags flag, bool ignore_default) const
		{
			return encode_with_flag(flag, ignore_default, false);
		}

		const Item* get(std::uint32_t idx) const
		{
			if (idx >= m_data.size())
			{
				return nullptr;
			}
			return m_data[idx].get();
		}

		bool decode(const json& data)
		{
			if (!data.is_array())
			{
				return false;
			}
			m_data.reserve(data.size());
			for (const auto& one_json : data)
			{
				auto cur_item = std::make_unique<Item>();
				if (!cur_item->decode(one_json))
				{
					return false;
				}
				m_data.push_back(std::move(cur_item));
			}
			return true;

		}
		friend void swap(property_vec& a, property_vec& b)
		{
			std::swap(a.m_data, b.m_data);
		}
		void clear()
		{
			m_data.clear();
		}

		bool has_default_value() const
		{
			return m_data.empty();
		}
		bool operator==(const property_vec& other) const
		{
			if (m_data.size() != other.size())
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
		bool operator!=(const property_vec& other) const
		{
			return !(operator==(other));
		}
	protected:

		void reset_by_other(const property_vec& other)
		{
			m_data.clear();
			m_data.reserve(other.size());
			for(const auto& one_item: other.m_data)
			{
				m_data.push_back(std::make_unique<Item>(*one_item));
			}
		}
		bool insert(std::uint32_t idx, std::unique_ptr<Item> new_data)
		{
			if (idx >= m_data.size())
			{
				return false;
			}
			m_data.insert(m_data.begin() + idx, std::move(new_data));
			return true;
		}

		std::unique_ptr<value_type> erase(std::uint32_t idx)
		{
			if (idx >= m_data.size())
			{
				return {};
			}
			std::unique_ptr<value_type> result;
			std::swap(result, m_data[idx]);
			m_data.erase(m_data.begin() + idx);
			return result;
		}
		bool erase_multi(std::uint32_t idx, std::uint32_t num)
		{
			if (idx >= m_data.size() || num > m_data.size() - idx)
			{
				return false;
			}
			m_data.erase(m_data.begin() + idx, m_data.begin() + idx + num);
			return true;
		}
		std::optional<prop_record_proxy<Item>> get(msg_queue_base& parent_queue,
			property_record_offset parent_offset, property_flags parent_flag, std::uint32_t slot)
		{
			if (slot >= m_data.size() || !m_data[slot])
			{
				return {};

			}
			return prop_record_proxy<Item>(*m_data[slot], parent_queue, parent_offset, parent_flag, slot);
		}
	protected:
		
		bool replay_clear(const json& data)
		{
			clear();
			return true;
		}
		bool replay_erase(const json& data)
		{
			std::uint32_t idx, num;
			if (data.is_number_unsigned())
			{
				if (!serialize::decode(data, idx))
				{
					return false;
				}
				if (idx < m_data.size())
				{
					m_data.erase(m_data.begin() + idx);
					return true;
				}
				else
				{
					return true;
				}

			}
			else if (data.is_array())
			{
				if (!serialize::decode_multi(data, idx, num))
				{
					return false;
				}
				if (idx >= m_data.size() || num > m_data.size() - idx)
				{
					return false;
				}
				m_data.erase(m_data.begin() + idx, m_data.begin() + idx + num);
				return true;
			}
			return false;
		}
		bool replay_push(const json& data)
		{
			std::unique_ptr<Item> temp = std::make_unique<Item>();
			if (temp->decode(data))
			{
				m_data.push_back(std::move(temp));
				return true;
			}
			else
			{
				return false;
			}
		}
		bool replay_pop(const json& data)
		{
			if (m_data.empty())
			{
				return false;
			}
			m_data.pop_back();
			return true;
		}
		bool replay_add(const json& data)
		{
			std::unique_ptr<Item> temp = std::make_unique<Item>();
			std::uint32_t idx;
			if (serialize::decode_multi(data, idx, *temp))
			{
				m_data.insert(m_data.begin() + idx, std::move(temp));
				return true;
			}
			else
			{
				return false;
			}
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

		bool replay_item_mutate(std::uint32_t mutate_idx, const json& data)
		{
			property_replay_offset field_offset;
			std::uint8_t field_cmd;
			json mutate_content;
			if (!serialize::decode_multi(data, field_offset, field_cmd, mutate_content))
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
			using std::swap;
			property_vec temp;
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
			case property_cmd::set:
				return replay_set(data);
			case property_cmd::add:
				return replay_add(data);
			case property_cmd::item_change:
				return replay_item_mutate(data);
			case property_cmd::erase:
				return replay_erase(data);
			case property_cmd::pop:
				return replay_pop(data);
			case property_cmd::push:
				return replay_push(data);
			default:
				return false;
			}
		}
		friend class prop_record_proxy<property_vec<Item>>;
	};

	template <typename T>
	class prop_record_proxy<property_vec<T>>
	{

		property_vec<T>& m_data;
		msg_queue_base& m_queue;
	public:
		const property_record_offset m_offset;
		const property_flags m_flag;
	public:
		using value_type = T;
		prop_record_proxy(property_vec<T>& data,
			msg_queue_base& msg_queue,
			const property_record_offset& offset,
			const property_flags& flag)
			:m_data(data)
			, m_queue(msg_queue)
			, m_offset(offset)
			, m_flag(flag)
		{

		}
		const property_vec<T>& data() const
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
		void set(const json& other)
		{
			using std::swap;
			property_vec<T> new_vec;
			if (!new_vec.decode(other))
			{
				return;
			}
			swap(m_data, new_vec);
			for (auto one_need_flag : m_queue.m_need_flags)
			{
				if (one_need_flag.include_by(m_flag))
				{
					auto one_encode_result = m_data.encode_with_flag(one_need_flag, m_queue.m_encode_ignore_default, m_queue.m_encode_with_array);

					m_queue.add_for_flag(m_offset, property_cmd::set, one_need_flag, m_flag, one_encode_result);
				}
			}
		}
		void erase_multi(std::uint32_t idx, std::uint32_t num)
		{
			if (!m_data.erase_multi(idx, num))
			{
				return;
			}
			if (m_queue.is_flag_need(m_flag))
			{
				m_queue.add(m_offset, property_cmd::erase, m_flag, serialize::encode_multi(idx, num));
			}
		}

		std::unique_ptr<value_type> erase(std::uint32_t idx)
		{
			auto result = m_data.erase(idx);

			if (result && m_queue.is_flag_need(m_flag))
			{
				m_queue.add(m_offset, property_cmd::erase, m_flag, serialize::encode(idx));
			}
			return result;
		}
		std::optional<prop_record_proxy<value_type>> get(std::uint32_t idx)
		{
			return m_data.get(m_queue, m_offset, m_flag, idx);
		}

		void pop_back()
		{
			if (m_data.empty())
			{
				return;
			}
			m_data.m_data.pop_back();
			if (m_queue.is_flag_need(m_flag))
			{
				m_queue.add(m_offset, property_cmd::pop, m_flag, json());
			}
		}

		void push_back(const json& value)
		{
			auto temp_item = std::make_unique<T>();
			if (!temp_item->decode(value))
			{
				return;
			}
			push(std::move(temp_item));
		}
		void push_back(const T& value)
		{
			auto temp_item = std::make_unique<T>(value);
			push(std::move(temp_item));
		}

		void insert(std::uint32_t idx, const json& value)
		{
			if (idx >= m_data.size())
			{
				return;
			}
			auto temp_item = std::make_unique<T>();
			if (!temp_item->decode(value))
			{
				return;
			}
			return insert(idx, std::move(temp_item));
		}

		void insert(std::uint32_t idx, const T& value)
		{
			if (idx >= m_data.size())
			{
				return;
			}
			auto temp_item = std::make_unique<T>(value);

			return insert(idx, std::move(temp_item));
		}

		void push(std::unique_ptr<T> temp_item)
		{
			m_data.m_data.push_back(std::move(temp_item));
			for (auto one_need_flag : m_queue.m_need_flags)
			{
				if (one_need_flag.include_by(m_flag))
				{
					auto one_encode_result = m_data.m_data.back()->encode_with_flag(one_need_flag, m_queue.m_encode_ignore_default, m_queue.m_encode_with_array);

					m_queue.add_for_flag(m_offset, property_cmd::push, one_need_flag, m_flag, one_encode_result);
				}
			}
		}

		void insert(std::uint32_t idx, std::unique_ptr<T> temp_item)
		{
			if (!m_data.insert(idx, std::move(temp_item)))
			{
				return;
			}
			for (auto one_need_flag : m_queue.m_need_flags)
			{
				if (one_need_flag.include_by(m_flag))
				{
					auto one_encode_result = m_data.m_data[idx]->encode_with_flag(one_need_flag, m_queue.m_encode_ignore_default, m_queue.m_encode_with_array);

					m_queue.add_for_flag(m_offset, property_cmd::add, one_need_flag, m_flag, serialize::encode_multi(idx, one_encode_result));
				}
			}
		}

		bool replay(property_replay_offset offset, property_cmd cmd, const json& data)
		{
			if (offset.value() != 0)
			{
				return false;
			}
			switch (cmd)
			{
			case property_cmd::clear:
			{
				clear();
				return true;
			}
			case property_cmd::set:
			{
				set(data);
				return true;
			}
			case property_cmd::add:
			{
				value_type one_item;
				std::uint32_t item_idx;
				if (!serialize::decode_multi(data, item_idx, one_item))
				{
					return false;
				}
				insert(item_idx, one_item);
				return true;
			}
			case property_cmd::erase:
			{
				return replay_erase(data);
			}
			case property_cmd::push:
			{
				push_back(data);
				return true;
			}
			case property_cmd::pop:
			{
				pop_back();
				return true;
			}
			case property_cmd::item_change:
			{
				if(!data.is_array())
				{
					return false;
				}
				std::uint32_t item_idx;
				std::uint64_t item_offset;
				std::uint8_t item_cmd;
				json item_data;
				if(!serialize::decode_multi(data, item_idx, item_offset, item_cmd, item_data))
				{
					return false;
				}
				auto cur_item_proxy = m_data.get(m_queue, m_offset, m_flag, item_idx);
				if(!cur_item_proxy.has_value())
				{
					return false;
				}
				return cur_item_proxy->replay(property_record_offset(item_offset).to_replay_offset(), property_cmd(item_cmd), item_data);
				
			}
			default:
				return false;
			}
		}
	protected:
		bool replay_erase(const json& data)
		{
			std::uint32_t idx, num;
			if (data.is_number_unsigned())
			{
				if (!serialize::decode(data, idx))
				{
					return false;
				}
				erase(idx);
				return true;
			}
			else if (data.is_array())
			{
				if (!serialize::decode_multi(data, idx, num))
				{
					return false;
				}
				erase_multi(idx, num);
				return true;
			}
			return false;
		}


	};

}