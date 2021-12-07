#pragma once
#include <any_container/encode.h>

namespace spiritsaway::property
{
	// 每个变量在当前定义内只能是uint8的整数索引 所以一个类里面只能定义255个变量 
	// 0号变量保留不使用 以后可以扩充为std::uint16_t 或者与var_mutate_cmd合并为一个std::uint16_t 
	using var_idx_type = std::uint8_t;
	using var_cmd_type = std::uint8_t;// 对于变量的改变操作类型 全量赋值 清空 等等
	constexpr std::uint8_t depth_max = 8;
	enum class property_flags_enum: std::uint8_t
	{
		save_db = 0,
		sync_self,
		sync_ghost,
		sync_other,
	};
	struct property_flags
	{
		std::uint64_t value;
		const static std::uint64_t save_db = 1 << std::uint8_t(property_flags_enum::save_db);
		const static std::uint64_t sync_self = 1 << std::uint8_t(property_flags_enum::sync_self);
		const static std::uint64_t sync_ghost = 1 << std::uint8_t(property_flags_enum::sync_ghost);
		const static std::uint64_t sync_other = 1 << std::uint8_t(property_flags_enum::sync_other);
		const static std::uint64_t sync_clients = sync_self | sync_other;
		const static std::uint64_t sync_all = sync_clients | sync_ghost;
	};
	

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
	class prop_replay_proxy
	{
		T& m_data;
	public:
		prop_replay_proxy(T& data)
			: m_data(data)
		{

		}
		bool replay(property_offset offset, var_mutate_cmd cmd, const json& data)
		{
			return m_data.replay_mutate_msg(offset, cmd, data);
		}
	};

	template <typename T, typename B = void>
	struct has_default_value
	{
		bool operator()(const T& data) const
		{
			return data.empty();

		}
	};
	template <typename T>
	struct has_default_value< T, std::enable_if_t <
		std::is_arithmetic_v<T>, void>>
	{
		bool operator()(const T& data) const
		{
			return data == 0;
		}
	};
	template <typename T>
	struct has_default_value< T, std::enable_if_t <
		std::is_same_v<decltype(std::declval<T>().has_default_value()), bool>, void>>
	{
		bool operator()(const T& data) const
		{
			return data.has_default_value();
		}
	};

}