
bool chat_prop::replay_mutate_msg(property_offset offset, var_mutate_cmd cmd, const json& data)
{
	auto split_result = offset.split();
	auto field_index = split_result.second;
	auto remain_offset = split_result.first;
	switch(field_index)
	{
		case index_for_chat_records:
		{
			auto temp_proxy = make_replay_proxy(m_chat_records);
			return temp_proxy.replay(remain_offset, cmd, data);
		}
		case index_for_last_chat_ts:
		{
			auto temp_proxy = make_replay_proxy(m_last_chat_ts);
			return temp_proxy.replay(remain_offset, cmd, data);
		}
		case index_for_total_chats:
		{
			auto temp_proxy = make_replay_proxy(m_total_chats);
			return temp_proxy.replay(remain_offset, cmd, data);
		}
		default:
		
			return false;
	}
}

bool chat_prop::operator==(const chat_prop& other)
{
	
	if(m_chat_records != other.m_chat_records)
	{
		return false;
	}
	if(m_last_chat_ts != other.m_last_chat_ts)
	{
		return false;
	}
	if(m_total_chats != other.m_total_chats)
	{
		return false;
	}
	return true;
}

json chat_prop::encode() const
{
	
	json result;
	result["chat_records"] = spiritsaway::serialize::encode(m_chat_records);
	result["last_chat_ts"] = spiritsaway::serialize::encode(m_last_chat_ts);
	result["total_chats"] = spiritsaway::serialize::encode(m_total_chats);
	return result;
}

bool chat_prop::decode(const json& data)
{
	
	decltype(data.end()) iter;
	iter = data.find("chat_records");
	if(iter == data.end())
	{
		return false;
	}
	if(!spiritsaway::serialize::decode(*iter, m_chat_records))
	{
		return false;
	}
	iter = data.find("last_chat_ts");
	if(iter == data.end())
	{
		return false;
	}
	if(!spiritsaway::serialize::decode(*iter, m_last_chat_ts))
	{
		return false;
	}
	iter = data.find("total_chats");
	if(iter == data.end())
	{
		return false;
	}
	if(!spiritsaway::serialize::decode(*iter, m_total_chats))
	{
		return false;
	}
	return true;
}


