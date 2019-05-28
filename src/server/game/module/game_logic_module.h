#pragma once


#include "interface_header/logic/IGameLogicModule.h"
#include "interface_header/base/IKernelModule.h"
#include "interface_header/base/IClassModule.h"
#include "interface_header/base/IRedislModule.h"
#include "interface_header/base/IDataAgentModule.h"
#include "interface_header/logic/IAddonsModule.h"
#include "interface_header/logic/IObjectMgrModule.h"
#include "interface_header/logic/IPlayerMgrModule.h"
#include "interface_header/base/IConfigModule.h"
#include "common/common_defines.h"
#include <unordered_map>


namespace zq{


class WorldSession;
class WorldPacket;
struct CSVPlayerCreateInfo;
class Player;
class Entity;
class GameLogicModule : public IGameLogicModule
{
	using MsgFunMap = std::unordered_map<int32, MsgCallBackFun>;
public:

	GameLogicModule(ILibManager* p);
	~GameLogicModule();

	bool init() override;
	bool initEnd() override;
	bool run() override;

    bool loadFromDb(Entity* object, const std::string& db_data);
	bool saveDb(const std::string& key, Entity* object);

public:

	// login-->登录界面
	bool handleAuthSession(WorldSession* session, WorldPacket& recvPacket);
	bool handlePing(WorldSession* session, WorldPacket& recvPacket);
	bool handleKeepLive(WorldSession* session, WorldPacket& recvPacket);
	bool handleReadyForAccountDataTimes(WorldSession* session, WorldPacket& recvPacket);
	bool handleRealmSplitOpcode(WorldSession* session, WorldPacket& recvPacket);

	// 创角
	bool handleCharEnumOpcode(WorldSession* session, WorldPacket& recvPacket);
	bool handleCharCreateOpcode(WorldSession* session, WorldPacket& recvPacket);
	bool handleCharDeleteOpcode(WorldSession* session, WorldPacket& recvPacket);

	// 进入游戏
	bool handlePlayerLogin(WorldSession* session, WorldPacket& recvPacket);
	void sendDungeonDifficulty(Player* player, bool IsInGroup);
	void sendMotd(Player* player);

	void readAddonsInfo(WorldSession* session, ByteBuffer& data);

	void sendAuthResponseError(WorldSession* session, uint8 code);

	void onWorldSessionInit(WorldSession* session);

public:

	const CSVPlayerCreateInfo* getPlayerCreateInfo(int race_id, int class_id);

protected:

	bool addMsgFun(int32 msgid, MsgCallBackFun&& fun) override;
	void call(int32 msgid, WorldSession* session, WorldPacket& recvPacket) override;

	void addCreateRoleCb(create_role_cb_t&& cb) override { createRoleCbs_.emplace_back(std::move(cb)); }
	void addSaveToDBCb(savedb_cb_t&& cb) override { saveDbCbs_.emplace_back(std::move(cb)); }
	void addLoadFromDbCb(load_from_db_cb_t&& cb) override { loadFromDbCbs_.emplace_back(std::move(cb)); }

private:

	const CSVPlayerCreateInfo* playerCreateInfo_[MAX_RACES][MAX_CLASSES];

private:

	MsgFunMap msgFuns_;
	std::vector<create_role_cb_t> createRoleCbs_;
	std::vector<savedb_cb_t> saveDbCbs_;
	std::vector<load_from_db_cb_t> loadFromDbCbs_;

private:

	IKernelModule* kernelModule_;
	IClassModule* classModule_;
	IRedisModule* redisModule_;
	IDataAgentModule* dataAgentModule_;
	IAddonsModule* addonsModule_;
	IObjectMgrModule* objectMgrModule_;
	IPlayerMgrModule* playerMgrModule_;
	IConfigModule* configModule_;
};

}
