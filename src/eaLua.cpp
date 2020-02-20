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

eaPropertyValue::eaTable ToTableValue(lua_State* L, int index)
{
	eaPropertyValue::eaTable table;

	if (index < 0)
		--index;

	lua_pushnil(L);
	while (lua_next(L, index))
	{
		eaPropertyValue name = ToPropertyValue(L, -2);
		eaPropertyValue value = ToPropertyValue(L, -1);

		table[name] = value;
		lua_pop(L, 1);
	}

	return table;
}

eaPropertyValue ToPropertyValue(lua_State * L, int index)
{
	switch (lua_type(L, index))
	{
	case LUA_TNUMBER:
		return make_shared<double>(lua_tonumber(L, index));
	case LUA_TBOOLEAN:
		return make_shared<bool>(lua_toboolean(L, index));
		break;
	case LUA_TSTRING:
		return make_shared<string>(lua_tostring(L, index));
		break;
	case LUA_TTABLE:
		return ToTableValue(L, index);
		break;
	default:
		return nullptr;
		break;
	}
}

void PushTable(lua_State * L, eaPropertyValue::eaTable table)
{
	lua_createtable(L, 0, 0);
	for (auto& pair : table)
	{
		PushPropertyValue(L, pair.first);
		PushPropertyValue(L, pair.second);
		lua_settable(L, -3);
	}
}

void PushPropertyValue(lua_State * L, eaPropertyValue value)
{
	if (value.IsBoolean())
		lua_pushboolean(L, value.ToBoolean());
	else if (value.IsNumber())
		lua_pushnumber(L, value.ToNumber());
	else if (value.IsString())
		lua_pushstring(L, value.ToString().c_str());
	else if (value.IsTable())
		PushTable(L, value.ToTable());
	else
		lua_pushnil(L);
}
