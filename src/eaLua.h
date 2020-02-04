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
	lua_State* L;
	
public:
	eaLua();
	eaLua(const eaLua&) = delete;

	void StartDebuger();
	std::string& GetCurrentInfo() const;

private:
	void LoadFunction();

public:
	operator lua_State* () const
	{
		return L;
	}
	
	void Update();
};