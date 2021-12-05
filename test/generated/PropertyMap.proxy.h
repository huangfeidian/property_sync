
#ifndef __meta_parse__


template <>
class prop_record_proxy<PropertyMap>
{
	PropertyMap& m_PropertyMap_data;
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

	prop_record_proxy<decltype(m_PropertyMap_data.m_a)> a()
	{
		return prop_record_proxy<decltype(m_PropertyMap_data.m_a)>(m_PropertyMap_data.m_a, m_queue, m_offset.merge(PropertyMap::index_for_a));
	}
	prop_record_proxy<decltype(m_PropertyMap_data.m_b)> b()
	{
		return prop_record_proxy<decltype(m_PropertyMap_data.m_b)>(m_PropertyMap_data.m_b, m_queue, m_offset.merge(PropertyMap::index_for_b));
	}
	prop_record_proxy<decltype(m_PropertyMap_data.m_c)> c()
	{
		return prop_record_proxy<decltype(m_PropertyMap_data.m_c)>(m_PropertyMap_data.m_c, m_queue, m_offset.merge(PropertyMap::index_for_c));
	}
	prop_record_proxy<decltype(m_PropertyMap_data.m_d)> d()
	{
		return prop_record_proxy<decltype(m_PropertyMap_data.m_d)>(m_PropertyMap_data.m_d, m_queue, m_offset.merge(PropertyMap::index_for_d));
	}
	prop_record_proxy<decltype(m_PropertyMap_data.m_e)> e()
	{
		return prop_record_proxy<decltype(m_PropertyMap_data.m_e)>(m_PropertyMap_data.m_e, m_queue, m_offset.merge(PropertyMap::index_for_e));
	}
	prop_record_proxy<decltype(m_PropertyMap_data.m_f)> f()
	{
		return prop_record_proxy<decltype(m_PropertyMap_data.m_f)>(m_PropertyMap_data.m_f, m_queue, m_offset.merge(PropertyMap::index_for_f));
	}
	prop_record_proxy<decltype(m_PropertyMap_data.m_g)> g()
	{
		return prop_record_proxy<decltype(m_PropertyMap_data.m_g)>(m_PropertyMap_data.m_g, m_queue, m_offset.merge(PropertyMap::index_for_g));
	}
}
#endif