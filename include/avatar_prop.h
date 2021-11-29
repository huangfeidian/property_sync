#pragma once
#include "chat_prop.h"
#include "friend_prop.h"
#include "equip_prop.h"

namespace spiritsaway::serialize
{
	Meta(property) class avatar_prop
	{
	protected:
		Meta(property) chat_prop m_chat;
		Meta(property) friend_prop m_friend;
		Meta(property) equip_prop m_weapons;
		Meta(property) equip_prop m_armors;
		#include "avatar_prop.generated.h"
	};
#include "avatar_prop.proxy.h"
}