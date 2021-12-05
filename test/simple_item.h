#pragma once

#include "property.h"

using namespace spiritsaway::serialize;
using namespace spiritsaway::property;
namespace spiritsaway::test
{

	class Meta(property) simple_item : public property_item<int>
	{
		Meta(property) int m_a = 0;

#include "simple_item.generated.h"
	};

	
	using simple_bag = property_bag<simple_item>;

}
namespace spiritsaway::property
{
	#include "simple_item.proxy.h"
	// template <>
	// class prop_record_proxy<test::simple_item>
	// {
	// protected:
	// 	test::simple_item& m_data;
	// 	item_msg_queue m_queue;
	// public:
	// 	prop_record_proxy(msg_queue_base& parent_queue,
	// 		property_offset parent_offset, test::simple_item& data, std::uint32_t data_idx)
	// 		: m_queue(parent_queue, parent_offset, data_idx)
	// 		, m_data(data)
	// 	{

	// 	}
	// 	const test::simple_item& data() const
	// 	{
	// 		return m_data;
	// 	}


	// 	prop_record_proxy<decltype(m_data.m_a)> a()
	// 	{
	// 		property_offset empty_offset;
	// 		return prop_record_proxy<decltype(m_data.m_a)>(m_data.m_a, m_queue, empty_offset.merge(test::simple_item::index_for_a));
	// 	}
	// };
}
