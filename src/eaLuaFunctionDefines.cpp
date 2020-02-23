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
	array require(string name, string space)
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

		int resultCount = 0;

		lua_settop(L, 0);

		if (space == "scene")
			resultCount = eaApplication::instance->GetActiveScene()->GetDomain()->DoFile(name + ".lua");
		else
			resultCount = eaApplication::GetDomain()->DoFile(name + ".lua");

		if (resultCount == -1)
			return 0;

		for (auto i = 1; i <= resultCount; i++)
			lua_pushvalue(L, i);

		return resultCount;
	}

	/*
	void require(string message)
	*/
	LuaFunc(print, )
	{
		string str = GetString(1);

		eaApplication::GetLogger().Info("Print", str);

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

		eaApplication::GetLogger().Error("Lua", "发现错误： "s + err);

		return 0;
	}

	LuaFunc(quit, )
	{
		eaApplication::instance->closed = true;
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
	void setInfo(string org, string app)
	*/
	LuaFunc(setInfo, Application)
	{
		string org = GetString(1);
		string app = GetString(2);

		eaApplication::instance->persistedPath = SDL_GetPrefPath(org.c_str(), app.c_str());

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

	/*
	void loadProfile(string profileName)
	*/
	LuaFunc(loadProfile, Application)
	{
		string profileName = GetString(1);
		eaApplication::instance->LoadProfile(profileName);
		return 0;
	}

	/*
	void saveProfile(string profileName)
	*/
	LuaFunc(saveProfile, Application)
	{
		string profileName = GetString(1);
		eaApplication::instance->SaveProfile(profileName);
		return 0;
	}

	/*
	string getProfileInfo(string profileName)
	*/
	LuaFunc(getProfileInfo, Application)
	{
		string profileName = GetString(1);
		auto profileInfo = eaApplication::instance->GetProfileInfo(profileName);
		if (profileInfo == "")
			lua_pushnil(L);
		else
			lua_pushstring(L, profileInfo.c_str());
		return 1;
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
		auto scene = eaApplication::instance->GetActiveScene();

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
		auto scene = eaApplication::instance->GetActiveScene();

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

	/*
	number frameCount()
	*/
	LuaFunc(frameCount, Time)
	{
		lua_pushnumber(L, eaTime::FrameCount());
		return 1;
	}
};

class eaInputFunc
{
	/*
	bool getKey(string key)
	*/
	LuaFunc(getKey, Input)
	{
		auto key = GetString(1);
		lua_pushboolean(L, eaInput::GetKey(key));
		return 1;
	}

	/*
	bool getKeyDown(string key)
	*/
	LuaFunc(getKeyDown, Input)
	{
		auto key = GetString(1);
		lua_pushboolean(L, eaInput::GetKeyDown(key));
		return 1;
	}

	/*
	bool getKeyUp(string key)
	*/
	LuaFunc(getKeyUp, Input)
	{
		auto key = GetString(1);
		lua_pushboolean(L, eaInput::GetKeyUp(key));
		return 1;
	}

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