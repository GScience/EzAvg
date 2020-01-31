#include <lua.hpp>
#include <string>
#include <tuple>
#include <iostream>
#include "eaLuaFunctionDefines.h"
#include "eaLuaDomain.h"
#include "eaLua.h"

#define L reinterpret_cast<lua_State*>(state)

using namespace std;

eaLua::eaLua()
{
	state = luaL_newstate();
	luaL_openlibs(L);

	LoadFunction();
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