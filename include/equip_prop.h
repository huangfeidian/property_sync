#pragma once
#include "property.h"
namespace spiritsaway::serialize
{
	Meta(property) class equip_prop
	{
		Meta(property) std::unordered_map<std::uint64_t, std::uint64_t> m_get_ts;
#include "equip_prop.generated.h"
	};


#include "equip_prop.proxy.h"
}
