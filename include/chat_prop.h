#pragma once
#include "property.h"
namespace spiritsaway::serialize
{
	Meta(property) class chat_prop
	{
	protected:
		std::size_t m_total_chats;
		std::unordered_map<std::string, std::size_t> m_last_chat_ts;
#include "chat_prop.generated.h"

	};
}
