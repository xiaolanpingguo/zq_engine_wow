#pragma once



#include "interface_header/logic/IPlayerSkillModule.h"


namespace zq {


class Player;
class PlayerSkillModule : public IPlayerSkillModule
{
public:
	PlayerSkillModule(ILibManager* p);
	virtual ~PlayerSkillModule();

	bool init() override;
	bool initEnd() override;
	bool run() override;


protected:


private:

};

}
