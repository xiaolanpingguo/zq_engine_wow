#include "baselib/libloader/libmanager.h"
#include "login_cs_server.h"
#include "../login_client_server/login_client_server.h"
#include "midware/data_agent/data_agent_lib.h"
#include "baselib/db_redis/redis_lib.h"
using namespace std;
using namespace zq;

int main(int argc, char* argv[])
{
	LibManager::get_instance().launch<ST_LOGIN_SERVER, LoginCSServerLib, LoginClientServerLib, RedisLib, DataAgentLib>(argc, argv);
	LibManager::get_instance().loop();

	return 0;
}
