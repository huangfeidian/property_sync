
bool PropertyMap::replay_mutate_msg(spiritsaway::property::property_offset offset, spiritsaway::property::var_mutate_cmd cmd, const json& data)
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
		case index_for_b:
		{
			auto temp_proxy = make_replay_proxy(m_b);
			return temp_proxy.replay(remain_offset, cmd, data);
		}
		case index_for_c:
		{
			auto temp_proxy = make_replay_proxy(m_c);
			return temp_proxy.replay(remain_offset, cmd, data);
		}
		case index_for_d:
		{
			auto temp_proxy = make_replay_proxy(m_d);
			return temp_proxy.replay(remain_offset, cmd, data);
		}
		case index_for_e:
		{
			auto temp_proxy = make_replay_proxy(m_e);
			return temp_proxy.replay(remain_offset, cmd, data);
		}
		case index_for_f:
		{
			auto temp_proxy = make_replay_proxy(m_f);
			return temp_proxy.replay(remain_offset, cmd, data);
		}
		case index_for_g:
		{
			auto temp_proxy = make_replay_proxy(m_g);
			return temp_proxy.replay(remain_offset, cmd, data);
		}
		default:
		
			return false;
	}
}

bool PropertyMap::operator==(const PropertyMap& other)
{
	
	if(m_a != other.m_a)
	{
		return false;
	}
	if(m_b != other.m_b)
	{
		return false;
	}
	if(m_c != other.m_c)
	{
		return false;
	}
	if(m_d != other.m_d)
	{
		return false;
	}
	if(m_e != other.m_e)
	{
		return false;
	}
	if(m_f != other.m_f)
	{
		return false;
	}
	if(m_g != other.m_g)
	{
		return false;
	}
	return true;
}

json PropertyMap::encode() const
{
	
	json result;
	result["a"] = spiritsaway::serialize::encode(m_a);
	result["b"] = spiritsaway::serialize::encode(m_b);
	result["c"] = spiritsaway::serialize::encode(m_c);
	result["d"] = spiritsaway::serialize::encode(m_d);
	result["e"] = spiritsaway::serialize::encode(m_e);
	result["f"] = spiritsaway::serialize::encode(m_f);
	result["g"] = spiritsaway::serialize::encode(m_g);
	return result;
}

bool PropertyMap::decode(const json& data)
{
	
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
	iter = data.find("b");
	if(iter == data.end())
	{
		return false;
	}
	if(!spiritsaway::serialize::decode(*iter, m_b))
	{
		return false;
	}
	iter = data.find("c");
	if(iter == data.end())
	{
		return false;
	}
	if(!spiritsaway::serialize::decode(*iter, m_c))
	{
		return false;
	}
	iter = data.find("d");
	if(iter == data.end())
	{
		return false;
	}
	if(!spiritsaway::serialize::decode(*iter, m_d))
	{
		return false;
	}
	iter = data.find("e");
	if(iter == data.end())
	{
		return false;
	}
	if(!spiritsaway::serialize::decode(*iter, m_e))
	{
		return false;
	}
	iter = data.find("f");
	if(iter == data.end())
	{
		return false;
	}
	if(!spiritsaway::serialize::decode(*iter, m_f))
	{
		return false;
	}
	iter = data.find("g");
	if(iter == data.end())
	{
		return false;
	}
	if(!spiritsaway::serialize::decode(*iter, m_g))
	{
		return false;
	}
	return true;
}


