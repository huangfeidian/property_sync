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
			const property_offset& offset,
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
		operator const T& () const
		{
			return m_data;
		}
		void set(const T& data)
		{
			m_data = data;
			if (m_notify_kind != notify_kind::no_notify)
			{
				m_msg_queue.add(m_offset,
					var_mutate_cmd::set, serialize::encode(m_data));
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



	private:
		T& m_data;
		msg_queue_base& m_msg_queue;
		const property_offset m_offset;
		const notify_kind m_notify_kind;
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
		bool replay(property_offset offset, var_mutate_cmd cmd, const json& data)
		{
			if (offset.value() != 0)
			{
				return false;
			}
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
			const property_offset& offset) :
			m_data(data),
			m_msg_queue(msg_queue),
			m_offset(offset)
		{

		}
		std::vector<T>& get()
		{
			return m_data;
		}
		operator const std::vector<T>& () const
		{
			return m_data;
		}
		void set(const std::vector<T>& data)
		{
			m_data = data;
			m_msg_queue.add(m_offset, var_mutate_cmd::set, serialize::encode(m_data));
		}

		void clear()
		{
			m_data.clear();
			m_msg_queue.add(m_offset, var_mutate_cmd::clear, json());
		}

		void push_back(const T& new_data)
		{
			m_data.push_back(new_data);
			m_msg_queue.add(m_offset, var_mutate_cmd::vector_push_back, serialize::encode(new_data));
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
			m_msg_queue.add(m_offset, var_mutate_cmd::vector_idx_mutate, serialize::encode_multi(idx, new_data));
		}

		void idx_delete(std::size_t idx)
		{
			if (idx < m_data.size())
			{
				m_data.erase(m_data.begin() + idx);
			}
			m_msg_queue.add(m_offset, var_mutate_cmd::vector_idx_mutate, serialize::encode(idx));
		}

		

	private:
		std::vector<T>& m_data;
		msg_queue_base& m_msg_queue;
		const property_offset m_offset;
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
		bool replay(property_offset offset, var_mutate_cmd cmd, const json& data)
		{
			if (offset.value() != 0)
			{
				return false;
			}
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
			const property_offset& offset) :
			m_data(data),
			m_msg_queue(msg_queue),
			m_offset(offset)
		{

		}
		std::unordered_map<T1, T2>& get()
		{
			return m_data;
		}
		operator const std::unordered_map<T1, T2>& () const
		{
			return m_data;
		}
		void set(const std::unordered_map<T1, T2>& data)
		{
			m_data = data;
			m_msg_queue.add(m_offset, var_mutate_cmd::set, serialize::encode(m_data));
		}

		void clear()
		{
			m_data.clear();
			m_msg_queue.add(m_offset, var_mutate_cmd::clear, json());
		}

		void insert(const T1& key, const T2& value)
		{
			m_data[key] = value;
			m_msg_queue.add(m_offset, var_mutate_cmd::map_insert, serialize::encode_multi(key, value));
		}

		void erase(const T1& key)
		{
			m_data.erase(key);
			m_msg_queue.add(m_offset, var_mutate_cmd::map_erase, serialize::encode(key));
		}


	private:
		std::unordered_map<T1, T2>& m_data;
		msg_queue_base& m_msg_queue;
		const property_offset m_offset;
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
		bool replay(property_offset offset, var_mutate_cmd cmd, const json& data)
		{
			if (offset.value() != 0)
			{
				return false;
			}
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