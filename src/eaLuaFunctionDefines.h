#pragma once

#include <vector>
#include <string>

typedef int (*lua_CFunction) (lua_State* L);

struct eaLuaFunction
{
	std::string space;
	std::string name;
	lua_CFunction ptr;

	eaLuaFunction(const std::string& space, const std::string& name, lua_CFunction ptr)
		:space(space), name(name), ptr(ptr) {}
};

#define LuaFunc(name, space) \
class name##AutoRegister\
{\
public:\
	name##AutoRegister()\
	{\
		eaLuaFuncTable.push_back(eaLuaFunction(#space, #name, reinterpret_cast<lua_CFunction>(&name)));\
	}\
};\
name##AutoRegister __##name;\
static int name(lua_State* L)\

#define GetString(index) luaL_checkstring(L, index)
#define GetNumber(index) luaL_checknumber(L, index)
#define GetInteger(index) luaL_checkinteger(L, index)

/*
函数表
*/
extern std::vector<eaLuaFunction> eaLuaFuncTable;
