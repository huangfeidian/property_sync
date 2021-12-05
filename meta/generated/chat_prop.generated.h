
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

	prop_record_proxy<decltype(m_chat_prop_data.m_m_chat_records)> m_chat_records()
	{
		return prop_record_proxy<decltype(m_chat_prop_data.m_m_chat_records)>(m_chat_prop_data.m_m_chat_records, m_queue, m_offset.merge(chat_prop::index_for_m_chat_records));
	}
	prop_record_proxy<decltype(m_chat_prop_data.m_m_last_chat_ts)> m_last_chat_ts()
	{
		return prop_record_proxy<decltype(m_chat_prop_data.m_m_last_chat_ts)>(m_chat_prop_data.m_m_last_chat_ts, m_queue, m_offset.merge(chat_prop::index_for_m_last_chat_ts));
	}
	prop_record_proxy<decltype(m_chat_prop_data.m_m_total_chats)> m_total_chats()
	{
		return prop_record_proxy<decltype(m_chat_prop_data.m_m_total_chats)>(m_chat_prop_data.m_m_total_chats, m_queue, m_offset.merge(chat_prop::index_for_m_total_chats));
	}
}public:
friend class prop_record_proxy<chat_prop>;
friend class prop_replay_proxy<chat_prop>;
chat_prop() = default;
const decltype(m_m_chat_records)& m_chat_records() const
{
    return m_chat_records;
}
const decltype(m_m_last_chat_ts)& m_last_chat_ts() const
{
    return m_last_chat_ts;
}
const decltype(m_m_total_chats)& m_total_chats() const
{
    return m_total_chats;
}
protected:

bool replay_mutate_msg(property_offset offset, var_mutate_cmd cmd, const json& data);


bool operator==(const chat_prop& other)


json chat_prop::encode() const;

bool decode(const json& data);

protected:
const static std::uint8_t index_for_m_chat_records = ;
const static std::uint8_t index_for_m_last_chat_ts = ;
const static std::uint8_t index_for_m_total_chats = ;
private:


protected:

bool chat_prop::replay_mutate_msg(property_offset offset, var_mutate_cmd cmd, const json& data)
{
	auto split_result = offset.split();
	auto field_index = split_result.second;
	auto remain_offset = split_result.first;
	switch(field_index)
	{
		case index_for_m_chat_records:
		{
			auto temp_proxy = make_replay_proxy(m_m_chat_records);
			return temp_proxy.replay(remain_offset, cmd, data);
		}
		case index_for_m_last_chat_ts:
		{
			auto temp_proxy = make_replay_proxy(m_m_last_chat_ts);
			return temp_proxy.replay(remain_offset, cmd, data);
		}
		case index_for_m_total_chats:
		{
			auto temp_proxy = make_replay_proxy(m_m_total_chats);
			return temp_proxy.replay(remain_offset, cmd, data);
		}
		default:
		
			return false;
	}
}

bool chat_prop::operator==(const chat_prop& other)
{
	
	if(m_m_chat_records != other.m_m_chat_records)
	{
		return false;
	}
	if(m_m_last_chat_ts != other.m_m_last_chat_ts)
	{
		return false;
	}
	if(m_m_total_chats != other.m_m_total_chats)
	{
		return false;
	}
	return true;
}

json chat_prop::encode() const
{
	
	json result;
	result["m_chat_records"] = spiritsaway::serialize::encode(m_m_chat_records);
	result["m_last_chat_ts"] = spiritsaway::serialize::encode(m_m_last_chat_ts);
	result["m_total_chats"] = spiritsaway::serialize::encode(m_m_total_chats);
	return result;
}

bool chat_prop::decode(const json& data)
{
	
	decltype(data.end()) iter;
	iter = data.find("m_chat_records");
	if(iter == data.end())
	{
		return false;
	}
	if(!spiritsaway::serialize::decode(*iter, m_m_chat_records))
	{
		return false;
	}
	iter = data.find("m_last_chat_ts");
	if(iter == data.end())
	{
		return false;
	}
	if(!spiritsaway::serialize::decode(*iter, m_m_last_chat_ts))
	{
		return false;
	}
	iter = data.find("m_total_chats");
	if(iter == data.end())
	{
		return false;
	}
	if(!spiritsaway::serialize::decode(*iter, m_m_total_chats))
	{
		return false;
	}
	return true;
}


