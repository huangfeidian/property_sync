#pragma once
#include "simple_item.h"

namespace spiritsaway::serialize
{
	class PropertyMap
	{
	protected:
		int m_a;
		std::vector<std::string> m_b;
		std::unordered_map<int, std::string> m_c;
		std::vector<int> m_d;
		std::unordered_map<int, int> m_e;
		std::unordered_map<std::string, int> m_f;
		simple_bag m_g;
	public:
		friend class prop_record_proxy< PropertyMap>;
		friend class prop_replay_proxy< PropertyMap>;
		PropertyMap()
		{

		}


		std::string type_name() const
		{
			static std::string name = "PropertyMap";
			return name;
		}
		const decltype(m_a)& a() const
		{
			return m_a;
		}
		

		const decltype(m_b)& b() const
		{
			return m_b;
		}
		

		const decltype(m_c)& c() const
		{
			return m_c;
		}
		
		const decltype(m_d)& d() const
		{
			return m_d;
		}
		
		const decltype(m_e)& e() const
		{
			return m_e;
		}
		
		const decltype(m_f)& f() const
		{
			return m_f;
		}
		
		const decltype(m_g)& g() const
		{
			return m_g;
		}
		

		bool replay_mutate_msg(property_offset offset, var_mutate_cmd cmd, const json& data)
		{
			auto split_result = offset.split();
			auto field_index = split_result.second;
			switch (field_index)
			{
			case index_for_a:
			{
				auto temp_proxy = make_replay_proxy(m_a);
				return temp_proxy.replay(split_result.first, cmd, data);
			}
			case index_for_b:
			{
				auto temp_proxy = make_replay_proxy(m_b);
				return temp_proxy.replay(split_result.first, cmd, data);
			}
			case index_for_c:
			{
				auto temp_proxy = make_replay_proxy(m_c);
				return temp_proxy.replay(split_result.first, cmd, data);
			}
			case index_for_d:
			{
				auto temp_proxy = make_replay_proxy(m_d);
				return temp_proxy.replay(split_result.first, cmd, data);
			}
			case index_for_e:
			{
				auto temp_proxy = make_replay_proxy(m_e);
				return temp_proxy.replay(split_result.first, cmd, data);
			}
			case index_for_f:
			{
				auto temp_proxy = make_replay_proxy(m_f);
				return temp_proxy.replay(split_result.first, cmd, data);
			}
			case index_for_g:
			{
				auto temp_proxy = make_replay_proxy(m_g);
				return temp_proxy.replay(split_result.first, cmd, data);
			}
			default:
				return false;
			}
		}
		bool operator==(const PropertyMap& other)
		{
			return m_a == other.m_a && m_b == other.m_b &&
				m_c == other.m_c && m_d == other.m_d
				&& m_f == other.m_f && m_e == other.m_e
				&& m_g == other.m_g;
		}
		json encode() const
		{
			json result;
			result["a"] = ::encode(m_a);
			result["b"] = ::encode(m_b);
			result["c"] = ::encode(m_c);
			result["d"] = ::encode(m_d);
			result["e"] = ::encode(m_e);
			result["f"] = ::encode(m_f);
			result["g"] = ::encode(m_g);
			return result;
		}
		bool decode(const json& data)
		{
			decltype(data.end()) iter;
			iter = data.find("a");
			if (iter == data.end())
			{
				return false;
			}
			if (!::decode(*iter, m_a))
			{
				return false;
			}
			iter = data.find("b");
			if (iter == data.end())
			{
				return false;
			}
			if (!::decode(*iter, m_b))
			{
				return false;
			}
			iter = data.find("c");
			if (iter == data.end())
			{
				return false;
			}
			if (!::decode(*iter, m_c))
			{
				return false;
			}
			iter = data.find("d");
			if (iter == data.end())
			{
				return false;
			}
			if (!::decode(*iter, m_d))
			{
				return false;
			}
			iter = data.find("e");
			if (iter == data.end())
			{
				return false;
			}
			if (!::decode(*iter, m_e))
			{
				return false;
			}
			iter = data.find("f");
			if (iter == data.end())
			{
				return false;
			}
			if (!::decode(*iter, m_f))
			{
				return false;
			}
			iter = data.find("g");
			if (iter == data.end())
			{
				return false;
			}
			if (!::decode(*iter, m_g))
			{
				return false;
			}

			return true;

		}
	private:
		static constexpr var_idx_type index_for_item = 0;
		static constexpr var_idx_type index_for_a = 1;
		static constexpr var_idx_type index_for_b = 2;
		static constexpr var_idx_type index_for_c = 3;
		static constexpr var_idx_type index_for_d = 4;
		static constexpr var_idx_type index_for_e = 5;
		static constexpr var_idx_type index_for_f = 6;
		static constexpr var_idx_type index_for_g = 7;
	};
	template <>
	class prop_record_proxy<PropertyMap>
	{
		PropertyMap& m_data;
		const property_offset m_offset;
		msg_queue_base& m_queue;
	public:
		prop_record_proxy(PropertyMap& data, msg_queue_base& msg_queue,
			const property_offset& offset)
			: m_data(data)
			, m_offset(offset)
			, m_queue(msg_queue)
		{

		}
		prop_record_proxy<decltype(m_data.m_a)> a()
		{
			return prop_record_proxy<decltype(m_data.m_a)>(m_data.m_a, m_queue, m_offset.merge(PropertyMap::index_for_a));
		}

		prop_record_proxy<decltype(m_data.m_b)> b()
		{
			return prop_record_proxy<decltype(m_data.m_b)>(m_data.m_b, m_queue, m_offset.merge(PropertyMap::index_for_b));
		}
		prop_record_proxy<decltype(m_data.m_c)> c()
		{
			return prop_record_proxy<decltype(m_data.m_c)>(m_data.m_c, m_queue, m_offset.merge(PropertyMap::index_for_c));
		}
		prop_record_proxy<decltype(m_data.m_d)> d()
		{
			return prop_record_proxy<decltype(m_data.m_d)>(m_data.m_d, m_queue, m_offset.merge(PropertyMap::index_for_d));
		}
		prop_record_proxy<decltype(m_data.m_e)> e()
		{
			return prop_record_proxy<decltype(m_data.m_e)>(m_data.m_e, m_queue, m_offset.merge(PropertyMap::index_for_e));
		}
		prop_record_proxy<decltype(m_data.m_f)> f()
		{
			return prop_record_proxy<decltype(m_data.m_f)>(m_data.m_f, m_queue, m_offset.merge(PropertyMap::index_for_f));
		}

		prop_record_proxy<decltype(m_data.m_g)> g()
		{
			return prop_record_proxy<decltype(m_data.m_g)>(m_data.m_g, m_queue, m_offset.merge(PropertyMap::index_for_g));
		}
	};
	template <>
	class prop_replay_proxy<PropertyMap>
	{
		PropertyMap& m_data;
	public:
		prop_replay_proxy(PropertyMap& data)
			: m_data(data)
		{

		}
		bool replay(property_offset offset, var_mutate_cmd cmd, const json& data)
		{
			return m_data.replay_mutate_msg(offset, cmd, data);
		}
	};
}