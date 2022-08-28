#pragma once
#include <cstdint>
#include <limits>

namespace spiritsaway::property
{
	enum class test_property_flags_enum: std::uint8_t
	{
		save_db = 0,
		sync_self,
		sync_ghost,
		sync_other,
		sync_redis,
	};
	struct test_property_flags
	{
		std::uint64_t value;
		const static std::uint64_t save_db = 1 << std::uint8_t(test_property_flags_enum::save_db);
		const static std::uint64_t sync_self = 1 << std::uint8_t(test_property_flags_enum::sync_self);
		const static std::uint64_t sync_ghost = 1 << std::uint8_t(test_property_flags_enum::sync_ghost);
		const static std::uint64_t sync_other = 1 << std::uint8_t(test_property_flags_enum::sync_other);
		const static std::uint64_t sync_redis = 1 << std::uint8_t(test_property_flags_enum::sync_redis);
		const static std::uint64_t sync_clients = sync_self | sync_other;
		const static std::uint64_t sync_all = sync_clients | sync_ghost;
		const static std::uint64_t mask_all = std::numeric_limits<std::uint64_t>::max();
	};
}