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
				m_msg_queue.add_multi(m_offset,
					property_cmd::set, m_flag, serialize::encode(m_data));
			}
			


		}

		void clear()
		{
			m_data = {};
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add_multi(m_offset,
					property_cmd::clear, m_flag, json());
			}
			
		}



	private:
		T& m_data;
		msg_queue_base& m_msg_queue;
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
				m_msg_queue.add_multi(m_offset,
					property_cmd::set, m_flag, serialize::encode(m_data));
			}
		}

		void clear()
		{
			m_data.clear();
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add_multi(m_offset, property_cmd::clear, m_flag, json());
			}
		}

		void push_back(const T& new_data)
		{
			m_data.push_back(new_data);
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add_multi(m_offset, property_cmd::vector_push_back, m_flag, serialize::encode(new_data));
			}
		}

		void pop_back()
		{
			if (m_data.size())
			{
				m_data.pop_back();
			}
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add_multi(m_offset, property_cmd::vector_pop_back, m_flag, json());
			}
		}

		void idx_mutate(std::size_t idx, const T& new_data)
		{
			if (idx < m_data.size())
			{
				m_data[idx] = new_data;
			}
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add_multi(m_offset, property_cmd::vector_idx_mutate, m_flag, serialize::encode_multi(idx, new_data));
			}
		}

		void idx_delete(std::size_t idx)
		{
			if (idx < m_data.size())
			{
				m_data.erase(m_data.begin() + idx);
			}
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add_multi(m_offset, property_cmd::vector_idx_mutate, m_flag, serialize::encode(idx));
			}
		}

		

	private:
		std::vector<T>& m_data;
		msg_queue_base& m_msg_queue;
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
			case property_cmd::vector_push_back:
				return replay_push_back(data);
			case property_cmd::vector_pop_back:
				return replay_pop_back(data);
			case property_cmd::vector_idx_mutate:
				return replaym_idx_mutate(data);
			case property_cmd::vector_idx_delete:
				return replaym_idx_delete(data);
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
		bool replay_push_back(const json& data)
		{
			T temp;
			if (serialize::decode(data, temp))
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
		bool replaym_idx_delete(const json& data)
		{
			std::size_t idx;
			if (!serialize::decode(data, idx))
			{
				return false;
			}
			if (idx < m_data.size())
			{
				m_data.erase(m_data.begin() + idx);
			}
			return true;
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
				m_msg_queue.add_multi(m_offset, property_cmd::set, m_flag, serialize::encode(m_data));
			}
		}

		void clear()
		{
			m_data.clear();
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add_multi(m_offset, property_cmd::clear, m_flag, json());
			}
		}

		void insert(const T1& key, const T2& value)
		{
			m_data[key] = value;
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add_multi(m_offset, property_cmd::map_insert, m_flag, serialize::encode_multi(key, value));
			}
		}

		void erase(const T1& key)
		{
			m_data.erase(key);
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add_multi(m_offset, property_cmd::map_erase, m_flag, serialize::encode(key));
			}
		}


	private:
		std::unordered_map<T1, T2>& m_data;
		msg_queue_base& m_msg_queue;
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
			case property_cmd::map_insert:
				return replay_insert(data);
			case property_cmd::map_erase:
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
				m_msg_queue.add_multi(m_offset, property_cmd::set, m_flag, serialize::encode(m_data));
			}
		}

		void clear()
		{
			m_data.clear();
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add_multi(m_offset, property_cmd::clear, m_flag, json());
			}
		}

		void insert(const T1& key, const T2& value)
		{
			m_data[key] = value;
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add_multi(m_offset, property_cmd::map_insert, m_flag, serialize::encode_multi(key, value));
			}
		}

		void erase(const T1& key)
		{
			m_data.erase(key);
			if (m_msg_queue.is_flag_need(m_flag))
			{
				m_msg_queue.add_multi(m_offset, property_cmd::map_erase, m_flag, serialize::encode(key));
			}
		}


	private:
		std::map<T1, T2>& m_data;
		msg_queue_base& m_msg_queue;
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
			case property_cmd::map_insert:
				return replay_insert(data);
			case property_cmd::map_erase:
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