#pragma once
#include <string>

struct lua_State;

class eaLuaEnv
{
	int envTableRef;

public:
	eaLuaEnv(lua_State* L, std::string domain);
	eaLuaEnv(const eaLuaEnv&) = delete;
	~eaLuaEnv();

	void DoString(lua_State* L, std::string str);
	void DoFile(lua_State* L, std::string str);
};