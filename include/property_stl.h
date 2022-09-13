#pragma once
#include "property_basic.h"

namespace spiritsaway::property
{
	template <typename T>
	class prop_record_proxy<T, std::enable_if_t<
		std::is_arithmetic_v<T> || std::is_same_v<T, std::string>, void>
	>
	{
	public:
		prop_record_proxy(T& data,
			msg_queue_base& msg_queue,
			const property_record_offset& offset,
			const property_flags& flag)
			: m_data(data),
			m_msg_queue(msg_queue),
			m_flag(flag),
			m_offset(offset)

		{

		}
		const T& get() const
		{
			return m_data;
		}

		void set(const T& data)
		{
			m_data = data;
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset,
					property_cmd::set, m_flag, serialize::encode(m_data));
			}
			


		}

		void clear()
		{
			m_data = {};
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset,
					property_cmd::clear, m_flag, json());
			}
			
		}



	private:
		T& m_data;
		msg_queue_base& m_msg_queue;
	public:
		const property_record_offset m_offset;
		const property_flags m_flag;
	};



	template <typename T>
	class prop_replay_proxy<T, std::enable_if_t<
		std::is_arithmetic_v<T> || std::is_same_v<T, std::string>, void>
	>
	{
	private:
		T& m_data;
	public:
		prop_replay_proxy(T& data)
			: m_data(data)
		{
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
				return replay_clear(data);
			case property_cmd::set:
				return replay_set(data);
			default:
				return false;
			}
		}
	private:
		bool replay_set(const json& data)
		{
			return serialize::decode(data, m_data);
		}
		bool replay_clear(const json& data)
		{
			m_data = {};
			return true;
		}
		
	};

	template<typename T>
	class prop_record_proxy<std::vector<T>>
	{
	public:
		prop_record_proxy(std::vector<T>& data,
			msg_queue_base& msg_queue,
			const property_record_offset& offset, 
			const property_flags& flag) 
			: m_data(data)
			, m_msg_queue(msg_queue)
			, m_offset(offset)
			, m_flag(flag)

		{

		}
		const std::vector<T>& get() const
		{
			return m_data;
		}

		void set(const std::vector<T>& data)
		{
			m_data = data;
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset,
					property_cmd::set, m_flag, serialize::encode(m_data));
			}
		}

		void clear()
		{
			m_data.clear();
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::clear, m_flag, json());
			}
		}

		void push_back(const T& new_data)
		{
			m_data.push_back(new_data);
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::push, m_flag, serialize::encode( new_data));
			}
		}
		void insert(std::uint32_t idx, const T& new_data)
		{
			if (idx >= m_data.size())
			{
				return;
			}
			m_data.insert(m_data.begin() + idx, new_data);
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::add, m_flag, serialize::encode_multi(idx, new_data));
			}
		}

		void pop_back()
		{
			if (m_data.empty())
			{
				return;
			}
			m_data.pop_back();
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::pop, m_flag, json());
			}
			
		}

		void item_change(std::size_t idx, const T& new_data)
		{
			if (idx < m_data.size())
			{
				m_data[idx] = new_data;
			}
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::item_change, m_flag, serialize::encode_multi(idx, new_data));
			}
		}

		void erase(std::uint32_t idx)
		{
			if (idx < m_data.size())
			{
				m_data.erase(m_data.begin() + idx);
			}
			else
			{
				return;
			}
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::erase, m_flag, serialize::encode(idx));
			}
		}

		void erase_multi(std::uint32_t idx, std::uint32_t num)
		{
			if (idx >= m_data.size() || num > m_data.size() - idx)
			{
				return;
			}
			m_data.erase(m_data.begin() + idx, m_data.begin() + idx + num);
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::erase, m_flag, serialize::encode_multi(idx, num));
			}
		}

		

	private:
		std::vector<T>& m_data;
		msg_queue_base& m_msg_queue;
	public:
		const property_record_offset m_offset;
		const property_flags m_flag;
	};

	template<typename T>
	class prop_replay_proxy<std::vector<T>>
	{
	private:
		std::vector<T>& m_data;
	public:
		prop_replay_proxy(std::vector<T>& data)
			: m_data(data)
		{

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
				return replay_clear(data);
			case property_cmd::set:
				return replay_set(data);
			case property_cmd::add:
				return replay_add(data);
			case property_cmd::item_change:
				return replay_item_change(data);
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
	private:
		bool replay_set(const json& data)
		{
			return serialize::decode(data, m_data);
		}
		bool replay_clear(const json& data)
		{
			m_data.clear();
			return true;
		}
		bool replay_push(const json& data)
		{
			T temp;
			if (serialize::decode(data, temp))
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
			T temp;
			std::uint32_t idx;
			if (serialize::decode_multi(data, idx, temp))
			{
				m_data.insert(m_data.begin() + idx, std::move(temp));
				return true;
			}
			else
			{
				return false;
			}
		}

		bool replay_item_change(const json& data)
		{
			std::uint32_t idx;
			T temp;
			if (!serialize::decode_multi(data, idx, temp))
			{
				return false;
			}
			if (idx < m_data.size())
			{
				m_data[idx] = temp;
			}
			return true;
		}
		bool replay_erase(const json& data)
		{
			std::uint32_t idx,num;
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
	};

	template <typename T1, typename T2>
	class prop_record_proxy<std::unordered_map<T1, T2>>
	{
	public:
		prop_record_proxy(std::unordered_map<T1, T2>& data,
			msg_queue_base& msg_queue,
			const property_record_offset& offset,
			const property_flags& flag) :
			m_data(data),
			m_msg_queue(msg_queue),
			m_offset(offset),
			m_flag(flag)
		{

		}
		const std::unordered_map<T1, T2>& get() const
		{
			return m_data;
		}

		void set(const std::unordered_map<T1, T2>& data)
		{
			m_data = data;
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::set, m_flag, serialize::encode(m_data));
			}
		}

		void clear()
		{
			m_data.clear();
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::clear, m_flag, json());
			}
		}

		void insert(const T1& key, const T2& value)
		{
			m_data[key] = value;
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::add, m_flag, serialize::encode_multi(key, value));
			}
		}

		void erase(const T1& key)
		{
			m_data.erase(key);
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::erase, m_flag, serialize::encode(key));
			}
		}


	private:
		std::unordered_map<T1, T2>& m_data;
		msg_queue_base& m_msg_queue;
	public:
		const property_record_offset m_offset;
		const property_flags m_flag;
	};

	template <typename T1, typename T2>
	class prop_replay_proxy<std::unordered_map<T1, T2>>
	{
		std::unordered_map<T1, T2>& m_data;
	public:
		prop_replay_proxy(std::unordered_map<T1, T2>& data)
			: m_data(data)
		{

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
				return replay_clear(data);
			case property_cmd::set:
				return replay_set(data);
			case property_cmd::add:
				return replay_insert(data);
			case property_cmd::erase:
				return replay_erase(data);
			default:
				return false;
			}
		}
	private:
		bool replay_set(const json& data)
		{
			return serialize::decode(data, m_data);
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
			if (!serialize::decode_multi(data, key, value))
			{
				return false;
			}
			m_data[key] = value;
			return true;
		}
		bool replay_erase(const json& data)
		{
			T1 key;
			if (!serialize::decode(data, key))
			{
				return false;
			}
			m_data.erase(key);
			return true;
		}
	};


	template <typename T1, typename T2>
	class prop_record_proxy<std::map<T1, T2>>
	{
	public:
		prop_record_proxy(std::map<T1, T2>& data,
			msg_queue_base& msg_queue,
			const property_record_offset& offset,
			const property_flags& flag) :
			m_data(data),
			m_msg_queue(msg_queue),
			m_offset(offset),
			m_flag(flag)
		{

		}
		const std::map<T1, T2>& get() const
		{
			return m_data;
		}

		void set(const std::map<T1, T2>& data)
		{
			m_data = data;
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::set, m_flag, serialize::encode(m_data));
			}
		}

		void clear()
		{
			m_data.clear();
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::clear, m_flag, json());
			}
		}

		void insert(const T1& key, const T2& value)
		{
			m_data[key] = value;
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::add, m_flag, serialize::encode_multi(key, value));
			}
		}

		void erase(const T1& key)
		{
			m_data.erase(key);
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add(m_offset, property_cmd::erase, m_flag, serialize::encode(key));
			}
		}


	private:
		std::map<T1, T2>& m_data;
		msg_queue_base& m_msg_queue;
	public:
		const property_record_offset m_offset;
		const property_flags m_flag;
	};

	template <typename T1, typename T2>
	class prop_replay_proxy<std::map<T1, T2>>
	{
		std::map<T1, T2>& m_data;
	public:
		prop_replay_proxy(std::map<T1, T2>& data)
			: m_data(data)
		{

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
				return replay_clear(data);
			case property_cmd::set:
				return replay_set(data);
			case property_cmd::add:
				return replay_insert(data);
			case property_cmd::erase:
				return replay_erase(data);
			default:
				return false;
			}
		}
	private:
		bool replay_set(const json& data)
		{
			return serialize::decode(data, m_data);
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
			if (!serialize::decode_multi(data, key, value))
			{
				return false;
			}
			m_data[key] = value;
			return true;
		}
		bool replay_erase(const json& data)
		{
			T1 key;
			if (!serialize::decode(data, key))
			{
				return false;
			}
			m_data.erase(key);
			return true;
		}
	};


}