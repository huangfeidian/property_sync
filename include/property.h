#pragma once
#include <any_container/decode.h>
#include "property_queue.h"
#include "property_stl.h"
#include "property_bag.h"
#include "property_slots.h"
#include "property_vec.h"

namespace spiritsaway::property
{

	template <typename T>
	prop_replay_proxy<T, void> make_replay_proxy(T& data)
	{
		return prop_replay_proxy<T, void>(data);
	}

}