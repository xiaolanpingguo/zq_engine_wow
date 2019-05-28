#include "dbc_module.h"
#include "dbc_stores.h"

namespace zq {


bool DBCModule::init()
{
	kernelModule_ = libManager_->findModule<IKernelModule>();
	classModule_ = libManager_->findModule<IClassModule>();
	gameLogicModule_ = libManager_->findModule<IGameLogicModule>();
	configModule_ = libManager_->findModule<IConfigModule>();

	LoadDBCStores("../datacfg/dbc/");
	return true;
}

bool DBCModule::initEnd()
{

	return true;
}




}

