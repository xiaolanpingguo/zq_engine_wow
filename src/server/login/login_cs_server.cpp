#include "login_cs_server.h"
#include "login_cs_module.h"


namespace zq{


const int LoginCSServerLib::getLibVersion()
{
    return 0;
}

const std::string LoginCSServerLib::getLibName()
{
	return GET_CLASS_NAME(LoginCSServerLib);
}

void LoginCSServerLib::install()
{
    REGISTER_MODULE(libManager_, ILoginCSModule, LoginCSModule)
}

void LoginCSServerLib::uninstall()
{
	UNREGISTER_MODULE(libManager_, ILoginCSModule, LoginCSModule)
}

}