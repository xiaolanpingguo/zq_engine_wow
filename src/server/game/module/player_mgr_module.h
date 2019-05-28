#pragma once


#include "player.h"
#include "item.h"
#include "interface_header/logic/IPlayerMgrModule.h"
#include "interface_header/logic/IObjectMgrModule.h"
#include "interface_header/base/IConfigModule.h"

namespace zq {


class PlayerMgrModule : public IPlayerMgrModule
{
public:
	PlayerMgrModule(ILibManager* p);
	virtual ~PlayerMgrModule();

	bool init() override;
	bool initEnd() override;
	bool run() override;

	bool storeNewItemInBestSlots(Player* player, uint32 item_id, uint32 item_count);	 
	InventoryResult canEquipItem(Player* player, uint32 item_id, int& slot);
	uint8 FindEquipSlot(Player* player, int InventoryType, uint32 slot);

protected:


private:

	IObjectMgrModule* objectMgrModule_;
	IConfigModule* configModule_;
};

}
