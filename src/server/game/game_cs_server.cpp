#include "game_cs_server.h"
#include "module/game_cs_module.h"
#include "module/game_logic_module.h"
#include "module/addons_module.h"
#include "module/dbc_module.h"
#include "module/object_mgr_module.h"
#include "module/player_mgr_module.h"
#include "module/player_base_module.h"
#include "module/player_achievement_module.h"
#include "module/player_skill_module.h"


namespace zq{


const int GameCSServerLib::getLibVersion()
{
    return 0;
}

const std::string GameCSServerLib::getLibName()
{
	return GET_CLASS_NAME(GameCSServerLib);
}

void GameCSServerLib::install()
{
	REGISTER_MODULE(libManager_, IGameCSModule, GameCSModule)
	REGISTER_MODULE(libManager_, IGameLogicModule, GameLogicModule)
	REGISTER_MODULE(libManager_, IAddonsModule, AddonsModule)	   
	REGISTER_MODULE(libManager_, IDBCModule, DBCModule)
	REGISTER_MODULE(libManager_, IObjectMgrModule, ObjectMgrModule)
	REGISTER_MODULE(libManager_, IPlayerMgrModule, PlayerMgrModule)
	REGISTER_MODULE(libManager_, IPlayerSkillModule, PlayerSkillModule)
	REGISTER_MODULE(libManager_, IPlayerAchivementModule, PlayerAchivementModule)
	REGISTER_MODULE(libManager_, IPlayerBaseModule, PlayerBaseModule)
}

void GameCSServerLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, IGameCSModule, GameCSModule)
	UNREGISTER_MODULE(libManager_, IGameLogicModule, GameLogicModule)
	UNREGISTER_MODULE(libManager_, IAddonsModule, AddonsModule)
	UNREGISTER_MODULE(libManager_, IDBCModule, DBCModule)
	UNREGISTER_MODULE(libManager_, IObjectMgrModule, ObjectMgrModule)
	UNREGISTER_MODULE(libManager_, IPlayerMgrModule, PlayerMgrModule)
	UNREGISTER_MODULE(libManager_, IPlayerSkillModule, PlayerSkillModule)
	UNREGISTER_MODULE(libManager_, IPlayerAchivementModule, PlayerAchivementModule)
	UNREGISTER_MODULE(libManager_, IPlayerBaseModule, PlayerBaseModule)
}

}