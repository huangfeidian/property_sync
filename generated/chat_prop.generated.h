
#ifndef __meta_parse__
template <>
class prop_record_proxy<chat_prop>
{
	chat_prop& m_chat_prop_data;
	const property_offset m_offset;
	msg_queue_base& m_queue;
public:
	prop_record_proxy(chat_prop& data, msg_queue_base& msg_queue,
		const property_offset& offset)
		: m_data(data)
		, m_offset(offset)
		, m_queue(msg_queue)
	{

	}

	prop_record_proxy<decltype(m_chat_prop_data.m_chat_records)> chat_records()
	{
		return prop_record_proxy<decltype(m_chat_prop_data.m_chat_records)>(m_chat_prop_data.m_chat_records, m_queue, m_offset.merge(chat_prop::index_for_chat_records));
	}
	prop_record_proxy<decltype(m_chat_prop_data.m_last_chat_ts)> last_chat_ts()
	{
		return prop_record_proxy<decltype(m_chat_prop_data.m_last_chat_ts)>(m_chat_prop_data.m_last_chat_ts, m_queue, m_offset.merge(chat_prop::index_for_last_chat_ts));
	}
	prop_record_proxy<decltype(m_chat_prop_data.m_total_chats)> total_chats()
	{
		return prop_record_proxy<decltype(m_chat_prop_data.m_total_chats)>(m_chat_prop_data.m_total_chats, m_queue, m_offset.merge(chat_prop::index_for_total_chats));
	}
}
#endif
