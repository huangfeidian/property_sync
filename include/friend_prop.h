#pragma once
#include "property.h"
namespace spiritsaway::serialize
{
	Meta(property) class friend_prop
	{
		Meta(property) std::unordered_map<std::string, std::uint32_t> m_friend_level;
#include "friend_prop.generated.h"
	};
#include "friend_prop.proxy.h"
}
