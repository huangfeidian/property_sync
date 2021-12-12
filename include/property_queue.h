#pragma once
#include <any_container/decode.h>
#include "property_basic.h"
namespace spiritsaway::property
{
	
	class aggregation_msg_queue : public msg_queue_base
	{
		msg_queue_base& m_parent_queue;
		const property_offset m_parent_offset;
	public:
		aggregation_msg_queue(msg_queue_base& parent_queue,
			const property_offset& parent_offset, const property_flags& current_flag)
			: msg_queue_base(parent_queue.m_need_flags)
			, m_parent_queue(parent_queue)
			, m_parent_offset(parent_offset)
		{

		}
		aggregation_msg_queue(const aggregation_msg_queue& other) = default;
		void add(const property_offset& offset, property_cmd cmd, property_flags flag, const json& data)
		{
			m_parent_queue.add(m_parent_offset.merge(offset), cmd, flag, data );
			return;
		}
		

	};

	class top_msg_queue : public msg_queue_base
	{
		std::deque<mutate_msg> m_queue;
	public:
		top_msg_queue(const std::vector<property_flags>& need_flags)
			: msg_queue_base(need_flags)
		{

		}
		top_msg_queue(const top_msg_queue& other) = default;
		
		void add(const property_offset& offset, property_cmd cmd, property_flags flag, const json& data) override
		{
			m_queue.push_back(mutate_msg{ offset, cmd, flag, data });
		}
		std::vector<mutate_msg> dump()
		{
			std::vector<mutate_msg> result;
			result.reserve(m_queue.size());
			while (!m_queue.empty())
			{
				result.push_back(m_queue.front());
				m_queue.pop_front();
			}
			return result;
		}
		std::deque<mutate_msg>& queue()
		{
			return m_queue;
		}
		bool empty() const
		{
			return m_queue.empty();
		}

		mutate_msg& front()
		{
			return m_queue.front();
		}
		void pop_front()
		{
			m_queue.pop_front();
		}

	};

	class item_msg_queue : public msg_queue_base
	{
		msg_queue_base& m_parent_queue;
		const std::uint32_t m_item_idx;
		const property_offset m_parent_offset;

	public:
		item_msg_queue(msg_queue_base& parent_queue,
			property_offset parent_offset,
			const std::uint32_t& item_idx)
			: msg_queue_base(parent_queue.m_need_flags)
			, m_parent_queue(parent_queue)
			, m_item_idx(item_idx)
			, m_parent_offset(parent_offset)
		{

		}
		item_msg_queue(const item_msg_queue& other) = default;
		void add(const property_offset& offset, property_cmd cmd, property_flags flag, const json& data) override
		{
			m_parent_queue.add(m_parent_offset, property_cmd::item_change, flag, serialize::encode_multi(m_item_idx, offset, cmd, data));
			return;
		}
		
		
	};
}