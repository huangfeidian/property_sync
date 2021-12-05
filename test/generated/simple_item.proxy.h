
#ifndef __meta_parse__

template <> 
class prop_record_proxy<simple_item>: public prop_record_proxy<spiritsaway::property::property_item<int>>
{
	simple_item& m_simple_item_data;
public:
	prop_record_proxy(simple_item& data, msg_queue_base& msg_queue,
		const property_offset& offset, std::uint32_t data_idx)
		: prop_record_proxy<spiritsaway::property::property_item<int>>(data, msg_queue, offset, data_idx)
		, m_simple_item_data(data)
	{

	}
	


	prop_record_proxy<decltype(m_simple_item_data.m_a)> a()
	{
		return prop_record_proxy<decltype(m_simple_item_data.m_a)>(m_simple_item_data.m_a, m_queue, m_offset.merge(simple_item::index_for_a));
	}
}
#endif