#pragma once


#include "interface_header/logic/IDBCModule.h"
#include "interface_header/base/IKernelModule.h"
#include "interface_header/base/IClassModule.h"
#include "interface_header/base/IRedislModule.h"
#include "interface_header/logic/IGameLogicModule.h"	
#include "interface_header/base/IConfigModule.h"
#include <list>


namespace zq{


class ByteBuffer;
class WorldPacket;
class WorldSession;
class DBCModule : public IDBCModule
{
public:
	DBCModule(ILibManager* p)
	{
		libManager_ = p;
	}

	bool init() override;
	bool initEnd() override;

private:

	IKernelModule* kernelModule_;
	IClassModule* classModule_;
	IGameLogicModule* gameLogicModule_;
	IConfigModule* configModule_;
};

}
