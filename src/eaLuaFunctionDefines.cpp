#include <functional>
#include <lua.hpp>
#include <iostream>
#include <SDL.h>
#include "eaResources.h"
#include "eaSpriteImage.h"
#include "eaApplication.h"
#include "eaLuaFunctionDefines.h"
#include "eaTime.h"

using namespace std;

vector<eaLuaFunction> eaLuaFuncTable;

class eaGlobalFunction
{
	/*
	void print(string message)
	*/
	LuaFunc(print, )
	{
		string str = GetString(1);

 		cout << str << endl;

		return 0;
	}

	/*
	void logError(string message)
	*/
	LuaFunc(logError, )
	{
		string err = GetString(1);

		cout << "���ִ���" << err << endl;

		lua_Debug debug;
		uint32_t level = 0;

		while (lua_getstack(L, level, &debug)) 
		{
			lua_getinfo(L, "Sln", &debug);
			if (debug.name != nullptr)
				cout << debug.short_src << ":" << debug.currentline << " " << debug.name << endl;
			else
				cout << debug.short_src << ":" << debug.currentline << endl;
			level++;
		}
		
		return 0;
	}
};

class eaApplicationFunction
{
	/*
	void setWindowSize(int width, int height)
	*/
	LuaFunc(setWindowSize, Application)
	{
		lua_Integer width = GetInteger(1);
		lua_Integer height = GetInteger(2);

		SDL_SetWindowSize(eaApplication::GetWindow(), (int) width, (int) height);
		SDL_SetWindowPosition(eaApplication::GetWindow(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

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
};

class eaActiveSceneFunction
{
	/*
	void createSubScene()
	*/
	LuaFunc(createSubScene, ActiveScene)
	{
		return 0;
	}

	/*
	void scriptRun(string name)
	*/
	LuaFunc(scriptRun, ActiveScene)
	{
		string name = GetString(1);

		eaApplication::instance->GetActiveScene()->RunScript(name);

		return 0;
	}

	/*
	bool scriptJump(string name)
	bool scriptJump(int pos)
	*/
	LuaFunc(scriptJump, ActiveScene)
	{
		auto activeScene = eaApplication::instance->GetActiveScene();

		// ��ǩ
		if (lua_type(L, 1) == LUA_TSTRING)
		{
			string labelName = lua_tostring(L, 1);
			lua_pushboolean(L, activeScene->runner.Jump(labelName));
		}
		else if (lua_type(L, 1) == LUA_TNUMBER)
		{
			long long pos = lua_tointeger(L, 1);
			lua_pushboolean(L, activeScene->runner.Jump(pos));
		}
		else
			lua_pushboolean(L, false);

		return 1;
	}
};


class eaSpriteFunction
{
	/*
	bool create(string spriteName, string spriteType)
	*/
	LuaFunc(create, Sprite)
	{
		string spriteName = GetString(1);
		string spriteType = GetString(2);

		auto activeScene = eaApplication::instance->GetActiveScene();

		std::shared_ptr<eaSprite> sprite;

		if (spriteType == "image")
			sprite = activeScene->AddSprite<eaSpriteImage>(spriteName);

		lua_pushboolean(L, sprite != nullptr);

		return 1;
	}

	/*
	void remove(string spriteName)
	*/
	LuaFunc(remove, Sprite)
	{
		string spriteName = GetString(1);

		auto activeScene = eaApplication::instance->GetActiveScene();

		activeScene->RemoveSprite(spriteName);

		return 0;
	}

	/*
	void setProperties(string spriteName, table properties)
	*/
	LuaFunc(setProperties, Sprite)
	{
		string spriteName = GetString(1);

		auto activeScene = eaApplication::instance->GetActiveScene();
		auto sprite = activeScene->GetSprite(spriteName);
		if (sprite == nullptr)
			return 0;

		lua_pushnil(L);
		while (lua_next(L, 2) != 0)
		{
			string name = lua_tostring(L, -2);

			switch (lua_type(L, -1))
			{
			case LUA_TNUMBER:
				sprite->SetProperty(name,
					make_shared<double>(lua_tonumber(L, -1))
				);
				break;
			case LUA_TBOOLEAN:
				sprite->SetProperty(name,
					make_shared<bool>(lua_toboolean(L, -1))
				);
				break;
			case LUA_TSTRING:
				sprite->SetProperty(name,
					make_shared<string>(lua_tostring(L, -1))
				);
				break;
			default:
				sprite->SetProperty(name, nullptr);
				break;
			}

			lua_pop(L, 1);
		}

		return 0;
	}

	/*
	anytype getProperty(string spriteName, string property)
	*/
	LuaFunc(getProperty, Sprite)
	{
		string spriteName = GetString(1);
		string property = GetString(2);

		auto activeScene = eaApplication::instance->GetActiveScene();
		auto sprite = activeScene->GetSprite(spriteName);
		if (sprite == nullptr)
		{
			lua_pushnil(L);
			return 1;
		}

		auto value = sprite->GetProperty(property);
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

eaGlobalFunction globalFunc;
eaApplicationFunction applicationFunc;

eaTimeFunction timeFunc;

eaActiveSceneFunction activesceneFunc;
eaSpriteFunction spriteFunc;
eaResourcesFunction resourceFunc;