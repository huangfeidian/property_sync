#pragma once

#include "property.h"
#include "macro.h"

using namespace spiritsaway::serialize;
using namespace spiritsaway::property;
namespace spiritsaway::test
{

	class Meta(property) simple_item : public property_item<int>
	{
		Meta(property) int m_a = 0;

#include "simple_item.generated.inch"
	};

	
	using simple_bag = property_bag<simple_item>;

}
namespace spiritsaway::property
{
	#include <simple_item.proxy.inch>

}
