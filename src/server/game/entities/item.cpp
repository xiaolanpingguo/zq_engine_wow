#include "item.h"
#include "bag.h"
#include "player.h"
#include "common/game_time.h"
#include "entities_key_def.hpp"


namespace zq{


Item::Item(const std::string& guid)
	:Entity(guid)
{
    m_slot = 0;
    uState = ITEM_NEW;
    uQueuePos = -1;
    m_container = nullptr;
    mb_in_trade = false;
    m_lastPlayedTimeUpdate = GameTime::GetGameTime();

    m_paidMoney = 0;
    m_paidExtendedCost = 0;
}


}
