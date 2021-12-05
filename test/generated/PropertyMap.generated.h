
#ifndef __meta_parse__
public:
friend class spiritsaway::property::prop_record_proxy<PropertyMap>;
friend class spiritsaway::property::prop_replay_proxy<PropertyMap>;
PropertyMap() = default;
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


protected:

bool replay_mutate_msg(spiritsaway::property::property_offset offset, spiritsaway::property::var_mutate_cmd cmd, const json& data);


bool operator==(const PropertyMap& other)


json encode() const;

bool decode(const json& data);

protected:
const static std::uint8_t index_for_a = 0;
const static std::uint8_t index_for_b = 1;
const static std::uint8_t index_for_c = 2;
const static std::uint8_t index_for_d = 3;
const static std::uint8_t index_for_e = 4;
const static std::uint8_t index_for_f = 5;
const static std::uint8_t index_for_g = 6;
private:

#endif


