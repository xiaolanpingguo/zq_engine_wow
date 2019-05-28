#include "player_skill_module.h"


namespace zq {


PlayerSkillModule::PlayerSkillModule(ILibManager* p)
{
	libManager_ = p;
}

PlayerSkillModule::~PlayerSkillModule()
{

}

bool PlayerSkillModule::init()
{
	//objectMgrModule_ = libManager_->findModule<IObjectMgrModule>();
	//configModule_ = libManager_->findModule<IConfigModule>();
	return true;
}

bool PlayerSkillModule::initEnd()
{

	return true;
}

bool PlayerSkillModule::run()
{

	return true;
}


}

