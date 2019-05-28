#pragma once

#include "interface_header/base/IClassModule.h"
#include "interface_header/base/IGameCSModule.h"
#include "interface_header/base/IConfigModule.h"
#include "interface_header/base/IMongoModule.h"
#include "baselib/message/game_ss.pb.h"
#include "interface_header/base/IGameToDBModule.h"
#include "server/game/game_client_server/game_to_db_session.h"

#include "baselib/network/socket_mgr.hpp"
#include "game_socket.h"


namespace zq {


class GameCSModule : public IGameCSModule, public SocketMgr<GameSocket>
{
	using BaseSockMgr = SocketMgr<GameSocket>;
public:
	GameCSModule(ILibManager* p);
	virtual ~GameCSModule() = default;

	bool init() override;
	bool initEnd() override;
	bool run() override;
	bool finalize() override;

	const std::string& getIp() { return BaseSockMgr::getIp(); };
	uint16 getPort() { return BaseSockMgr::getPort(); };

private:

	IClassModule* classModule_;
	IConfigModule* configModule_;
};

}

