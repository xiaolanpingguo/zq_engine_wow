#pragma once

#include "interface_header/base/IClassModule.h"
#include "interface_header/base/ILoginCSModule.h"
#include "interface_header/base/IRedislModule.h"
#include "interface_header/base/IDataAgentModule.h"
#include "interface_header/base/IConfigModule.h"
#include "interface_header/base/ICommandServerModule.h"

#include "baselib/network/socket_mgr.hpp"
#include "login_cs_session.h"

namespace zq {

class LoginCSModule : public ILoginCSModule, public SocketMgr<LoginCSSession>
{
	using BaseSockMgr = SocketMgr<LoginCSSession>;
public:
	LoginCSModule(ILibManager* p);

	bool init() override;
	bool initEnd() override;
	bool run() override;

	bool createAccount(const std::string& account_name, const std::string& pwd) override;

	const std::string& getIp() override { return BaseSockMgr::getIp(); };
	uint16 getPort() override { return BaseSockMgr::getPort(); };

private:

	bool serverCommand(const std::string& req_json, std::string& rep_json);

private:

	IClassModule* classModule_;
	IRedisModule* redisModule_;
	IDataAgentModule* dataAgentModule_;
	IConfigModule* configModule_;
	ICommandServerModule* commadServerModule_;
};

}

