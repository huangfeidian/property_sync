
#ifndef __meta_parse__
public:
friend class spiritsaway::property::prop_record_proxy<simple_item>;
friend class spiritsaway::property::prop_replay_proxy<simple_item>;
simple_item() = default;
const decltype(m_a)& a() const
{
    return m_a;
}


protected:

bool replay_mutate_msg(spiritsaway::property::property_offset offset, spiritsaway::property::var_mutate_cmd cmd, const json& data);


bool operator==(const simple_item& other)


json encode() const;

bool decode(const json& data);

protected:
const static std::uint8_t index_for_a = 0;
private:

#endif


