#pragma once
#include <any_container/decode.h>
#include "property_basic.h"
namespace spiritsaway::serialize
{
	
	class child_msg_queue : public msg_queue_base
	{
		msg_queue_base& m_parent_queue;
		const property_offset m_parent_offset;
	public:
		child_msg_queue(msg_queue_base& parent_queue,
			const property_offset& parent_offset)
			: m_parent_queue(parent_queue)
			, m_parent_offset(parent_offset)
		{

		}
		child_msg_queue(const child_msg_queue& other) = default;
		void add(const std::uint8_t& offset, var_mutate_cmd cmd, const json& data) override
		{
			m_parent_queue.add(m_parent_offset.merge(offset), cmd, data );
			return;
		}
		void add(const property_offset& offset, var_mutate_cmd cmd, const json& data) override
		{
			assert(false);
		}

	};

	class top_msg_queue : public msg_queue_base
	{
		std::deque<mutate_msg> m_queue;
	public:
		top_msg_queue()
		{

		}
		top_msg_queue(const top_msg_queue& other) = default;
		void add(const std::uint8_t& offset, var_mutate_cmd cmd, const json& data) override
		{
			assert(false);
		}
		void add(const property_offset& offset, var_mutate_cmd cmd, const json& data) override
		{
			m_queue.push_back(mutate_msg{ offset, cmd, data });
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
			: m_parent_queue(parent_queue)
			, m_item_idx(item_idx)
			, m_parent_offset(parent_offset)
		{

		}
		item_msg_queue(const item_msg_queue& other) = default;
		void add(const property_offset& offset, var_mutate_cmd cmd, const json& data) override
		{
			m_parent_queue.add(m_parent_offset, var_mutate_cmd::mutate_item, encode_multi(m_item_idx, offset, cmd, data));
			return;
		}
		void add(const std::uint8_t& offset, var_mutate_cmd cmd, const json& data) override
		{
			assert(false);
			return;
		}
	};
}