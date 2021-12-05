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
		Meta(property) std::vector<int> m_d;
		Meta(property) std::unordered_map<int, int> m_e;
		Meta(property) std::unordered_map<std::string, int> m_f;
		Meta(property) simple_bag m_g;
#include <PropertyMap.generated.h>		
	};
	

}
namespace spiritsaway::property
{
	#include <PropertyMap.proxy.h>
}