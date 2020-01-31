#include <lua.hpp>
#include "eaApplication.h"
#include "eaLuaEnv.h"

using namespace std;

eaLuaEnv::eaLuaEnv(lua_State* L, string domain)
{
	// create _ENV
	lua_newtable(L);
	lua_pushstring(L, "domain");
	lua_pushstring(L, domain.c_str());
	lua_settable(L, -3);

	// set metatable
	lua_newtable(L);
	lua_pushstring(L, "__index");
	lua_getglobal(L, "_G");
	lua_settable(L, -3);

	lua_setmetatable(L, -2);

	envTableRef = luaL_ref(L, LUA_REGISTRYINDEX);
}

eaLuaEnv::~eaLuaEnv()
{
	auto& L = eaApplication::GetLua();

	luaL_unref(L, LUA_REGISTRYINDEX, envTableRef);
}

void eaLuaEnv::DoString(lua_State* L, string str)
{
	luaL_loadstring(L, str.c_str());
	lua_rawgeti(L, LUA_REGISTRYINDEX, envTableRef);
	lua_setupvalue(L, -2, 1);
	lua_pcall(L, 0, LUA_MULTRET, 0);
}

void eaLuaEnv::DoFile(lua_State* L, string str)
{
	luaL_loadfile(L, str.c_str());
	lua_rawgeti(L, LUA_REGISTRYINDEX, envTableRef);
	lua_setupvalue(L, -2, 1);
	lua_pcall(L, 0, LUA_MULTRET, 0);
}