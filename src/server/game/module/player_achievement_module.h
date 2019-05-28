#pragma once


#include "interface_header/logic/IPlayerAchievementModule.h"

namespace zq {


class PlayerAchivementModule : public IPlayerAchivementModule
{
public:
	PlayerAchivementModule(ILibManager* p);
	virtual ~PlayerAchivementModule();

	bool init() override;
	bool initEnd() override;
	bool run() override;


protected:


private:

	//IObjectMgrModule* objectMgrModule_;
	//IConfigModule* configModule_;
};

}
