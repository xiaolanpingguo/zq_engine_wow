#include "baselib/libloader/libmanager.h"
#include "baselib/db_redis/redis_lib.h"
#include "midware/data_agent/data_agent_lib.h"
#include "game_cs_server.h"
#include "../game_client_server/game_client_server.h"
using namespace std;
using namespace zq;

int main(int argc, char* argv[])
{
	LibManager::get_instance().launch<ST_GAME_SERVER, GameCSServerLib, GameClientServerLib, RedisLib, DataAgentLib>(argc, argv);
	LibManager::get_instance().loop();

	return 0;
}
