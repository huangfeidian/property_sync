#pragma once
#include <any_container/encode.h>
#include <any_container/decode.h>
namespace spiritsaway::property
{
	// 每个变量在当前定义内只能是uint8的整数索引 所以一个类里面只能定义255个变量 
	// 0号变量保留不使用 以后可以扩充为std::uint16_t 或者与property_cmd合并为一个std::uint16_t 
	using property_idx_type = std::uint8_t;
	using property_cmd_type = std::uint8_t;// 对于变量的改变操作类型 全量赋值 清空 等等
	constexpr std::uint8_t depth_max = 8;

	struct property_flags
	{
		std::uint64_t value;
		const static std::uint64_t mask_all = std::numeric_limits<std::uint64_t>::max();
		const static std::uint64_t mask_none = 0;
		constexpr property_flags merge(property_flags other) const
		{
			return property_flags{ value & other.value };
		}
		constexpr bool include_by(property_flags other) const
		{
			return (value & other.value) == value;
		}
	};
	
	// record offset 和replay offset 都可以当作八字节的数组 但是两者的字节序是不同的 因此这里用类型区分了一下
	// 同时为了记录数组里有几个值，record的时候所有的值都会加1 避免出现0 
	class property_replay_offset
	{
		std::uint64_t m_value;

	public:
		property_replay_offset()
			: m_value(0)
		{

		}
		property_replay_offset(std::uint64_t value)
			: m_value(value)
		{

		}
		bool operator==(const property_replay_offset& other_offset) const
		{
			return m_value == other_offset.m_value;
		}
		std::uint64_t value() const
		{
			return m_value;
		}
		std::pair<property_replay_offset, std::uint8_t> split() const
		{
			auto temp_value = m_value;

			return std::make_pair(property_replay_offset(temp_value >> 8), std::uint8_t(temp_value & 0xff));
		}

		bool decode(const json& data)
		{

			if (!serialize::decode(data, m_value))
			{
				return false;
			}
			auto temp_value = m_value;
			std::array<std::uint8_t, 8> parts = { 0 };
			std::uint8_t n_parts = 0;
			while (temp_value != 0)
			{
				std::uint8_t cur_part_value = static_cast<std::uint8_t>(temp_value & 0xff);
				temp_value >>= 8;
				assert(cur_part_value >= 1);
				parts[n_parts] = cur_part_value - 1;
				n_parts++;
			}
			m_value = 0;
			for (int i = 0; i < n_parts; i++)
			{
				m_value <<= 8;
				m_value += parts[i];
			}
			
			return true;
		}
	};

	class property_record_offset
	{
		std::uint64_t m_value;
		
	public:
		property_record_offset()
			: m_value(0)
		{

		}
		property_record_offset(std::uint64_t value)
			: m_value(value)
		{

		}
		std::uint64_t value() const
		{
			return m_value;
		}
		property_record_offset merge(property_record_offset child_var) const
		{
			assert(m_value < (std::uint64_t(1) << 56));
			assert((child_var.m_value + 1) < (std::uint64_t(1) << 8));
			auto new_value = (m_value << 8) | ((child_var.m_value + 1) & 0xff); // 这里加一是为了避免出现0 因为查找的时候会以0作为终止符号
			return property_record_offset(new_value);
		}
		property_record_offset merge_all(property_record_offset child_var) const
		{
			std::uint64_t new_value = m_value;
			std::uint64_t child_value = child_var.m_value;
			while(child_value)
			{
				std::uint8_t cur_part_value = static_cast<std::uint8_t>(child_value & 0xff);
				child_value >>= 8;
				assert(cur_part_value >= 1);
				new_value = (new_value <<8) | cur_part_value;
			}
			return new_value;
		}
		
		json encode() const
		{
			return m_value;
		}
		
		property_replay_offset to_replay_offset() const
		{
			auto temp_value = m_value;
			std::array<std::uint8_t, 8> parts = { 0 };
			std::uint8_t n_parts = 0;
			while (temp_value != 0)
			{
				// 这里判定temp_value是否是0 所以有效值绝对不能取0 所有merge过来的值都会加1
				std::uint8_t cur_part_value = static_cast<std::uint8_t>(temp_value & 0xff);
				temp_value >>= 8;
				assert(cur_part_value >= 1);
				parts[n_parts] = cur_part_value - 1;
				n_parts++;
			}
			temp_value = 0;
			for (int i = 0; i < n_parts; i++)
			{
				temp_value <<= 8;
				temp_value += parts[i];
			}
			return property_replay_offset{ temp_value };
		}
	};

	
	enum class property_cmd : property_cmd_type
	{
		clear = 0,
		set,
		add,
		erase,
		push,
		pop,
		pop_erase,
		item_change,
		slot_swap,
		slot_resize,
		slot_move,
		update_fields,
	};
	enum class notify_kind
	{
		no_notify = 0,
		self_notify = 1,
		all_notify = 2
	};

	struct mutate_msg
	{
		property_record_offset offset;
		property_cmd cmd;
		property_flags flag;
		json data;
	};

	struct replay_msg
	{
		property_replay_offset offset;
		property_cmd cmd;
		const json& data;
	};
	struct property_item
	{

	};
	class msg_queue_base
	{
	public:
		const std::vector<property_flags>& m_need_flags;
		const bool m_encode_ignore_default;
		const bool m_encode_with_array;
		msg_queue_base(const std::vector<property_flags>& need_flags, bool encode_ignore_default, bool encode_with_array)
			: m_need_flags(need_flags)
			, m_encode_ignore_default(encode_ignore_default)
			, m_encode_with_array(encode_with_array)
		{

		}
		virtual void add_for_flag(const property_record_offset& offset, property_cmd cmd, property_flags need_flag, property_flags data_flag, const json& data) = 0;
		inline void add(const property_record_offset& offset, property_cmd cmd, property_flags data_flag, const json& data)
		{
			for (auto one_need_flag : m_need_flags)
			{
				if (one_need_flag.include_by(data_flag))
				{
					add_for_flag(offset, cmd, one_need_flag, data_flag, data);
				}
			}
		}
		inline bool is_flag_need(property_flags flag) const
		{
			for (auto one_need_flag : m_need_flags)
			{
				if (one_need_flag.include_by(flag))
				{
					return true;
				}
			}
			return false;
		}
		virtual ~msg_queue_base()
		{

		}
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
		bool replay(property_replay_offset offset, property_cmd cmd, const json& data)
		{
			return m_data.replay_mutate_msg(offset, cmd, data);
		}
		const T& data() const
		{
			return m_data;
		}

	};

	template <typename T, typename B = void>
	struct has_default_value
	{
		bool operator()(const T& data) const
		{
			return data == T{};

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

namespace std
{
	template <>
	struct hash<spiritsaway::property::property_replay_offset>
	{
		std::size_t operator()(const spiritsaway::property::property_replay_offset& cur_offset) const
		{
			return std::hash<std::uint64_t>()(cur_offset.value());
		}
	};
}