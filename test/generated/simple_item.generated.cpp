
bool simple_item::replay_mutate_msg(spiritsaway::property::property_offset offset, spiritsaway::property::var_mutate_cmd cmd, const json& data)
{
	auto split_result = offset.split();
	auto field_index = split_result.second;
	auto remain_offset = split_result.first;
	switch(field_index)
	{
		case index_for_a:
		{
			auto temp_proxy = make_replay_proxy(m_a);
			return temp_proxy.replay(remain_offset, cmd, data);
		}
		default:
		
			return false;
	}
}

bool simple_item::operator==(const simple_item& other)
{
	if(!spiritsaway::property::property_item<int>::operator==(other))
	{
		return false;
	}
	if(m_a != other.m_a)
	{
		return false;
	}
	return true;
}

json simple_item::encode() const
{
	json result = spiritsaway::property::property_item<int>::encode();
	
	result["a"] = spiritsaway::serialize::encode(m_a);
	return result;
}

bool simple_item::decode(const json& data)
{
	if(!spiritsaway::property::property_item<int>::decode(data))
	{
		return false;
	}
	decltype(data.end()) iter;
	iter = data.find("a");
	if(iter == data.end())
	{
		return false;
	}
	if(!spiritsaway::serialize::decode(*iter, m_a))
	{
		return false;
	}
	return true;
}


