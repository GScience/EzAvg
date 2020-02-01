#pragma once
#include <string>
#include <functional>
#include <memory>

using eaLuaNumber = double;
using eaLuaBoolean = bool;
using eaLuaString = std::string;

struct lua_State;

class eaLua
{
	lua_State* state;
	
public:
	eaLua();
	eaLua(const eaLua&) = delete;

private:
	void LoadFunction();

public:
	operator lua_State* () const
	{
		return state;
	}
	
	void Update();
};