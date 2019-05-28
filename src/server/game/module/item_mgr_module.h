#pragma once


#include "item.h"
#include "interface_header/logic/IItemMgrModule.h"

namespace zq {


class ItemMgrModule : public IItemMgrModule
{
public:
	ItemMgrModule(ILibManager* p);
	virtual ~ItemMgrModule();

	bool init() override;
	bool initEnd() override;
	bool run() override;

	Item* createItem(uint32 itemEntry, uint32 count) override { return 0; };
protected:


private:

};

}
