#pragma once


#include "interface_header/logic/IPlayerBaseModule.h"

namespace zq {


class PlayerBaseModule : public IPlayerBaseModule
{
public:
	PlayerBaseModule(ILibManager* p) { libManager_ = p; }

	bool init() override;
	bool initEnd() override;
	bool run() override;


protected:


private:

	//IObjectMgrModule* objectMgrModule_;
	//IConfigModule* configModule_;
};

}
