#pragma once
#include "chat_prop.h"
#include "friend_prop.h"
#include "equip_prop.h"

namespace spiritsaway::serialize
{
	Meta(property) class avatar_prop
	{
	protected:
		Meta(property) char_prop m_chat;
		Meta(property) friend_prop m_friend;
		Meta(property) equip_prop m_equips;
		#include "avatar_prop.generated.h"
	};
}