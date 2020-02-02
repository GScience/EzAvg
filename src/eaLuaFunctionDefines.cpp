#include <functional>
#include <lua.hpp>
#include <iostream>
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

		cout << "发现错误：" << err << endl;

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
	int getCurrentPos(string name)
	*/
	LuaFunc(getNextPos, Script)
	{
		auto scene = eaApplication::instance->CurrentScene();

		lua_pushinteger(L, scene->runner->GetNextPos());

		return 1;
	}
};

class eaSpriteFunction
{
	/*
	为精灵添加行为脚本
	table addBehaviour(string spriteName, string behaviour, string type)
	*/
	LuaFunc(addBehaviour, Sprite)
	{
		std::shared_ptr<eaSprite> sprite;

		string spriteName = GetString(1);

		auto scene = eaApplication::instance->CurrentScene();
		sprite = scene->GetSprite(spriteName);

		if (sprite == nullptr)
			return 0;

		string behaviour = GetString(2);
		string type = GetString(3);

		auto obj = sprite->AddBehaviour(behaviour, type);
		lua_rawgeti(L, LUA_REGISTRYINDEX, obj->GetObjRef());

		return 1;
	}

	/*
	创建精灵
	bool create(string spriteName, string spriteType)
	*/
	LuaFunc(create, Sprite)
	{
		string spriteName = GetString(1);
		string spriteType = GetString(2);

		auto scene = eaApplication::instance->CurrentScene();

		std::shared_ptr<eaSprite> sprite;

		if (spriteType == "image")
			sprite = scene->AddSprite<eaSpriteImage>(spriteName);
		else if (spriteType == "text")
			sprite = scene->AddSprite<eaSpriteText>(spriteName);

		lua_pushboolean(L, sprite != nullptr);

		return 1;
	}

	/*
	设置精灵属性
	void setProperties(string spriteName, table properties)
	*/
	LuaFunc(setProperties, Sprite)
	{
		std::shared_ptr<eaSprite> sprite;

		string spriteName = GetString(1);

		auto scene = eaApplication::instance->CurrentScene();
		sprite = scene->GetSprite(spriteName);

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
	获取精灵属性
	anytype getProperty(string spriteName, string property)
	*/
	LuaFunc(getProperty, Sprite)
	{
		std::shared_ptr<eaSprite> sprite;

		if (lua_isstring(L, 1))
		{
			string spriteName = GetString(1);

			auto scene = eaApplication::instance->CurrentScene();
			sprite = scene->GetSprite(spriteName);
		}
		else if (lua_isinteger(L, 1))
		{
			auto spritePtr = reinterpret_cast<eaSprite*>(GetInteger(1));
			sprite = spritePtr->shared_from_this();
		}

		string property = GetString(2);

		auto scene = eaApplication::instance->CurrentScene();
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
eaSpriteFunction spriteFunc;
eaResourcesFunction resourceFunc;