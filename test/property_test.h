#pragma once
#include "simple_item.h"

namespace spiritsaway::test
{
	class Meta(property) PropertyMap
	{
	protected:
		Meta(property) int m_a;
		Meta(property) std::vector<std::string> m_b;
		Meta(property) std::unordered_map<int, std::string> m_c;
		Meta(property(save_db)) std::vector<int> m_d;
		Meta(property(sync_clients)) std::unordered_map<int, int> m_e;
		Meta(property) std::unordered_map<std::string, int> m_f;
		Meta(property(save_db, sync_clients)) simple_bag m_g;
		Meta(property(save_db, sync_clients)) simple_slots m_h;
		Meta(property(save_db, sync_clients)) simple_vec m_i;
		Meta(property(save_db, sync_clients)) std::array<float, 3> m_j;
#include <PropertyMap.generated.inch>		
	};
	

}
namespace spiritsaway::property
{
	#include <PropertyMap.proxy.inch>
}