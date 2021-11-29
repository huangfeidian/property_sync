#pragma once
#include "property.h"
#include "chat_record.h"

namespace spiritsaway::serialize
{
	Meta(property) class chat_prop
	{
	protected:
		Meta(property) std::size_t m_total_chats;
		Meta(property) std::unordered_map<std::string, std::size_t> m_last_chat_ts;
		Meta(property) property_bag<chat_record> m_chat_records;
#include "chat_prop.generated.h"

	};
#include "chat_prop.proxy.h"
}
