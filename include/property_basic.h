#pragma once
#include <any_container/encode.h>

namespace spiritsaway::serialize
{
	// 每个变量在当前定义内只能是uint8的整数索引 所以一个类里面只能定义255个变量 
	// 0号变量保留不使用 以后可以扩充为std::uint16_t 或者与var_mutate_cmd合并为一个std::uint16_t 
	using var_idx_type = std::uint8_t;
	using var_cmd_type = std::uint8_t;// 对于变量的改变操作类型 全量赋值 清空 等等
	const static std::uint8_t depth_max = 8;
	class property_offset
	{
		std::uint64_t m_value;
		
	public:
		property_offset()
			: m_value(0)
		{

		}
		property_offset(std::uint64_t value)
			: m_value(value)
		{

		}
		std::uint64_t value() const
		{
			return m_value;
		}
		property_offset merge(std::uint8_t child_var) const
		{
			assert(m_value < (std::uint64_t(1) << 56));
			auto new_value = (m_value << 8) | child_var;
			return property_offset(new_value);
		}
		std::pair<property_offset, std::uint8_t> split() const
		{
			auto temp_value = m_value;
			std::array<std::uint8_t, 8> parts = { 0 };
			std::uint8_t n_parts = 0;
			while (temp_value != 0)
			{
				std::uint8_t cur_part_value = static_cast<std::uint8_t>(temp_value & 0xff);
				temp_value >>= 8;
				parts[n_parts] = cur_part_value;
				n_parts++;
			}
			std::uint64_t remain_value = 0;
			for (int i = 0; i + 1 < n_parts; i++)
			{
				remain_value <<= 8;
				remain_value |= parts[n_parts - i - 1];
			}
			return std::make_pair(property_offset(remain_value), parts[0]);
		}
		json encode() const
		{
			return m_value;
		}
		bool decode(const json& data)
		{
			return serialize::decode(data, m_value);
		}

	};

	enum class var_mutate_cmd : var_cmd_type
	{
		clear = 0,
		set = 1,
		mutate_item = 2,
		vector_push_back = 10,
		vector_idx_mutate = 11,
		vector_idx_delete = 12,
		vector_insert_head = 13,
		vector_pop_back = 14,
		map_insert = 20,
		map_erase = 21,
		set_add = 30,
		set_erase = 31,
		item_update = 41,

	};
	enum class notify_kind
	{
		no_notify = 0,
		self_notify = 1,
		all_notify = 2
	};

	struct mutate_msg
	{
		property_offset offset;
		var_mutate_cmd cmd;
		json data;
	};
	class msg_queue_base
	{
	public:
		virtual void add(const std::uint8_t& offset, var_mutate_cmd cmd, const json& data) = 0;
		virtual void add(const property_offset& offset, var_mutate_cmd cmd, const json& data) = 0;
	};
	template <typename T, typename B = void>
	class prop_record_proxy;

	template<typename T, typename B = void>
	class prop_replay_proxy;

}