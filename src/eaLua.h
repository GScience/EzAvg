#pragma once
#include <string>
#include <functional>
#pragma once

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
	void DoString(const std::string& str);
	void DoFile(const std::string& str);

	operator lua_State* ()
	{
		return state;
	}
};