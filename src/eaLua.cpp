#include <lua.hpp>
#include <string>
#include <tuple>
#include "eaLuaFunctionDefines.h"
#include "eaLuaDomain.h"
#include "eaLua.h"

using namespace std;

std::string currentDebugInfo = "未启动调试器";

eaLua::eaLua()
{
	L = luaL_newstate();
	luaL_openlibs(L);

	LoadFunction();
}

void eaLua::Update()
{
	lua_settop(L, 0);
}

void eaLua::LoadFunction()
{
	auto& funcTable = eaLuaFuncTable;

	for (const auto& func : funcTable)
	{
		if (func.space == "")
		{
			lua_register(L, func.name.c_str(), *func.ptr);
			continue;
		}

		lua_getglobal(L, func.space.c_str());

		if (lua_isnil(L, -1))
		{
			lua_pop(L, -1);
			lua_newtable(L);
			lua_setglobal(L, func.space.c_str());
			lua_getglobal(L, func.space.c_str());
		}
		lua_pushstring(L, func.name.c_str());
		lua_pushcfunction(L, *func.ptr);
		lua_settable(L, -3);
	}
}

static void DebugHook(lua_State* L, lua_Debug* ar)
{
	lua_getinfo(L, "Sln", ar);
	if (ar->name != nullptr)
		currentDebugInfo = ar->short_src + ":"s + to_string(ar->currentline) + " " + ar->name;
	else
		currentDebugInfo = ar->short_src + ":"s + to_string(ar->currentline);
}


void eaLua::StartDebuger()
{
	lua_sethook(L, DebugHook, LUA_MASKLINE, 0);
}

std::string& eaLua::GetCurrentInfo() const
{
	return currentDebugInfo;
}