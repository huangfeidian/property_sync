#pragma onece
#include "property.h"
namespace spiritsaway::property
{
	Meta(property) class chat_record: public property_item<std::string>
	{
	protected:
		Meta(property) std::size_t m_last_chat_ts = 0;
		Meta(property) std::uint32_t m_total_chat_count = 0;
	#include "chat_record.generated.h"
	};
#include "chat_record.proxy.h"
}