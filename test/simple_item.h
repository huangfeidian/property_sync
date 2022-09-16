#pragma once

#include "property.h"
#include "macro.h"
#include "prop_flags.h"

using namespace spiritsaway::serialize;
using namespace spiritsaway::property;
namespace spiritsaway::test
{

	class Meta(property) simple_bag_item : public property_bag_item<int>
	{
	public:
		Meta(property(sync_clients)) int m_a = 0;
		Meta(property(save_db)) std::string m_b;

#include "simple_bag_item.generated.inch"
	};

	class Meta(property) simple_slot_item : public property_slot_item<int>
	{
	public:
		Meta(property(sync_clients)) int m_a = 0;
		Meta(property(save_db)) std::string m_b;

#include "simple_slot_item.generated.inch"
	};

	class Meta(property) simple_vec_item : public property_vec_item
	{
	public:
		Meta(property(sync_clients)) int m_a = 0;
		Meta(property(save_db)) std::string m_b;

#include "simple_vec_item.generated.inch"
	};

	
	using simple_bag = property_bag<simple_bag_item>;

	using simple_slots = property_slots<simple_slot_item>;

	using simple_vec = property_vec<simple_vec_item>;

}
namespace spiritsaway::property
{
	#include "simple_bag_item.proxy.inch"
	#include "simple_slot_item.proxy.inch"
	#include "simple_vec_item.proxy.inch"

}
