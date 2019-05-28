#include "player_base_module.h"


namespace zq {


bool PlayerBaseModule::init()
{
	//objectMgrModule_ = libManager_->findModule<IObjectMgrModule>();
	//configModule_ = libManager_->findModule<IConfigModule>();
	return true;
}

bool PlayerBaseModule::initEnd()
{

	return true;
}

bool PlayerBaseModule::run()
{

	return true;
}



}

