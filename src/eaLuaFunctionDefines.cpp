#include <functional>
#include <lua.hpp>
#include <SDL.h>
#include "eaResources.h"
#include "eaSpriteImage.h"
#include "eaApplication.h"
#include "eaLuaFunctionDefines.h"
#include "eaTime.h"
#include "eaInput.h"
#include "eaSpriteText.h"

using namespace std;

vector<eaLuaFunction> eaLuaFuncTable;

class eaGlobalFunction
{
	/*
	void require(string name, string space)
	*/
	LuaFunc(require, )
	{
		string name = GetString(1);
		for (auto& c : name)
			if (c == '.' || c == '\\')
				c = '/';
		string space;
		if (lua_isstring(L, 2))
			space = GetString(2);

		if (space == "scene")
			eaApplication::instance->CurrentScene()->GetDomain()->DoFile(name + ".lua");
		else
			eaApplication::GetDomain()->DoFile(name + ".lua");

		return 0;
	}

	/*
	void require(string message)
	*/
	LuaFunc(print, )
	{
		string str = GetString(1);

		eaApplication::GetLogger().Log("Info", str);

		return 0;
	}

	/*
	void logError(string message)
	*/
	LuaFunc(logError, )
	{
		string err = GetString(1);

		lua_Debug debug;
		uint32_t level = 0;

		while (lua_getstack(L, level, &debug)) 
		{
			lua_getinfo(L, "Sln", &debug);
			if (debug.name != nullptr)
				err += debug.short_src + ":"s + to_string(debug.currentline) + " " + debug.name;
			else
				err += debug.short_src + ":"s + to_string(debug.currentline) + " ";
			level++;
		}

		eaApplication::GetLogger().Log("Error", "发现错误： "s + err);

		return 0;
	}
};

class eaApplicationFunction
{
	/*
	void setWindowSize(int width, int height)
	*/
	LuaFunc(setApplicationSize, Application)
	{
		int width = (int)GetInteger(1);
		int height = (int)GetInteger(2);

		eaApplication::instance->SetApplicationSize(width, height);

		return 0;
	}

	/*
	void setTitle(string title)
	*/
	LuaFunc(setTitle, Application)
	{
		string title = GetString(1);

		SDL_SetWindowTitle(eaApplication::GetWindow(), title.c_str());
		return 0;
	}

	/*
	void loadScene(string scriptName)
	*/
	LuaFunc(loadScene, Application)
	{
		string name = GetString(1);

		eaApplication::instance->LoadScene(name);

		return 0;
	}
};

class eaScriptFunction
{
	/*
	bool scriptJump(string name)
	bool scriptJump(int pos)
	*/
	LuaFunc(jump, Script)
	{
		auto scene = eaApplication::instance->CurrentScene();

		// 标签
		if (lua_type(L, 1) == LUA_TSTRING)
		{
			string labelName = lua_tostring(L, 1);
			lua_pushboolean(L, scene->runner->Jump(labelName));
		}
		else if (lua_type(L, 1) == LUA_TNUMBER)
		{
			long long pos = lua_tointeger(L, 1);
			lua_pushboolean(L, scene->runner->Jump(pos));
		}
		else
			lua_pushboolean(L, false);

		return 1;
	}

	/*
	int getNextPos()
	*/
	LuaFunc(getNextPos, Script)
	{
		auto scene = eaApplication::instance->CurrentScene();

		lua_pushinteger(L, scene->runner->GetNextPos());

		return 1;
	}
};

class eaResourcesFunction
{
	/*
	void loadImage(string name)
	*/
	LuaFunc(loadImage, Resources)
	{
		string name = GetString(1);

		//eaResources::Load<SDL_Surface>(name);
		return 0;
	}
};

class eaTimeFunction
{
	/*
	number deltaTime()
	*/
	LuaFunc(deltaTime, Time)
	{
		lua_pushnumber(L, eaTime::DeltaTime());
		return 1;
	}
};

class eaInputFunc
{
	/*
	bool getButton(int button)
	*/
	LuaFunc(getButton, Input)
	{
		auto button = (MouseButton)(int)GetNumber(1);
		lua_pushboolean(L, eaInput::GetButton(button));
		return 1;
	}

	/*
	bool getButtonDown(int button)
	*/
	LuaFunc(getButtonDown, Input)
	{
		auto button = (MouseButton)(int)GetNumber(1);
		lua_pushboolean(L, eaInput::GetButtonDown(button));
		return 1;
	}

	/*
	bool getButtonUp(int button)
	*/
	LuaFunc(getButtonUp, Input)
	{
		auto button = (MouseButton)(int)GetNumber(1);
		lua_pushboolean(L, eaInput::GetButtonUp(button));
		return 1;
	}
};

eaGlobalFunction globalFunc;
eaApplicationFunction applicationFunc;

eaTimeFunction timeFunc;
eaInputFunc inputFunc;

eaScriptFunction scriptFunc;
eaResourcesFunction resourceFunc;