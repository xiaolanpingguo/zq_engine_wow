#include "player_achievement_module.h"


namespace zq {


PlayerAchivementModule::PlayerAchivementModule(ILibManager* p)
{
	libManager_ = p;
}

PlayerAchivementModule::~PlayerAchivementModule()
{

}

bool PlayerAchivementModule::init()
{
	//objectMgrModule_ = libManager_->findModule<IObjectMgrModule>();
	//configModule_ = libManager_->findModule<IConfigModule>();
	return true;
}

bool PlayerAchivementModule::initEnd()
{

	return true;
}

bool PlayerAchivementModule::run()
{

	return true;
}

}

