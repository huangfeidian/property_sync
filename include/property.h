#include <any_container/decode.h>
#include "macro.h"
#include "property_queue.h"
#include "property_stl.h"
#include "property_bag.h"

namespace spiritsaway::property
{

	


   
	

	template <typename T>
	prop_replay_proxy<T, void> make_replay_proxy(T& data)
	{
		return prop_replay_proxy<T, void>(data);
	}

}