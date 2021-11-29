#pragma once

#include "property.h"

using namespace spiritsaway::serialize;

class simple_item : public property_item<int>
{
	int m_a = 0;

public:
	friend class prop_replay_proxy<simple_item>;
	friend class prop_record_proxy<simple_item>;
	using base = property_item<int>;
	using base::base;
	static constexpr var_idx_type index_for_item = 0;
	static constexpr var_idx_type index_for_a = 1;
	const decltype(m_a)& a() const
	{
		return m_a;
	}

	bool replay_mutate_msg(property_offset offset, var_mutate_cmd cmd, const json& data)
	{
		auto split_result = offset.split();
		auto field_index = split_result.second;
		auto remain_offset = split_result.first;
		switch (field_index)
		{
		case index_for_a:
		{
			auto temp_proxy = make_replay_proxy(m_a);
			return temp_proxy.replay(remain_offset, cmd, data);
		}
		default:
			return false;
		}
	}
	bool operator==(const simple_item& other) const
	{
		return m_id == other.m_id && m_a == other.m_a;
	}
	json encode() const
	{
		auto result = base::encode();
		result["a"] = m_a;
		return result;
	}
	bool decode(const json& data)
	{
		if (!base::decode(data))
		{
			return false;
		}
		auto iter = data.find("a");
		if (iter == data.end())
		{
			return false;
		}
		if (!::decode(*iter, m_a))
		{
			return false;
		}
		return true;
	}
	void swap(simple_item& other)
	{
		std::swap(m_a, other.m_a);
	}
	std::string type_name()
	{
		return "simple_item";
	}
#include "simple_item.generated.h"
};

template <>
class prop_record_proxy<simple_item>
{
protected:
	simple_item& m_data;
	item_msg_queue m_queue;
public:
	prop_record_proxy(msg_queue_base& parent_queue,
		property_offset parent_offset, simple_item& data, std::uint32_t data_idx)
		: m_queue(parent_queue, parent_offset, data_idx)
		, m_data(data)
	{

	}
	const simple_item& data() const
	{
		return m_data;
	}


	prop_record_proxy<decltype(m_data.m_a)> a()
	{
		property_offset empty_offset;
		return prop_record_proxy<decltype(m_data.m_a)>(m_data.m_a, m_queue, empty_offset.merge(simple_item::index_for_a));
	}
};
using simple_bag = property_bag<simple_item>;

