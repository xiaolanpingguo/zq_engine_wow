#include "item_mgr_module.h"																		


namespace zq {


ItemMgrModule::ItemMgrModule(ILibManager* p)
{
	libManager_ = p;
}

ItemMgrModule::~ItemMgrModule()
{

}

bool ItemMgrModule::init()
{

	return true;
}

bool ItemMgrModule::initEnd()
{

	return true;
}

bool ItemMgrModule::run()
{

	return true;
}

}

