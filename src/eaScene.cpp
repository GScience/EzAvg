#include <SDL.h>
#include <lua.hpp>
#include <algorithm>
#include "eaApplication.h"
#include "eaResources.h"
#include "eaScene.h"

using namespace std;

static int ScenePopScene(lua_State* L)
{
	auto scene = (eaScene*)lua_tointeger(L, lua_upvalueindex(1));
	auto sceneName = lua_tostring(L, 1);
	auto value = ToPropertyValue(L, 2);
	scene->PopScene(sceneName, value);
	return 0;
}

static int SceneClose(lua_State* L)
{
	auto scene = (eaScene*)lua_tointeger(L, lua_upvalueindex(1));
	scene->Close();
	return 0;
}

static int SceneGet(lua_State* L)
{
	auto scene = (eaScene*)lua_tointeger(L, lua_upvalueindex(1));
	auto name = lua_tostring(L, 2);
	if (name == "popScene"s)
	{
		lua_pushinteger(L, (long long)scene);
		lua_pushcclosure(L, ScenePopScene, 1);
	}
	else if (name == "close"s)
	{
		lua_pushinteger(L, (long long)scene);
		lua_pushcclosure(L, SceneClose, 1);
	}
	else
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, scene->GetSpriteGroup()->GetDomain()->GetEnvTableRef());
		lua_pushstring(L, "sprite");
		lua_gettable(L, -2);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
	}
	return 1;
}

eaScene::eaScene(string name)
{
	auto& L = eaApplication::GetLua();

	domain = eaLuaDomain::Create(name, eaApplication::GetDomain());

	spriteGroup = eaSpriteGroup::Create(domain, "scene");

	auto windowSize = eaApplication::instance->applicationSize;
	spriteGroup->box = eaSpriteBox
	{
		0,0,windowSize.width,windowSize.height
	};
	spriteGroup->OnLayoutChanged();

	// 为属性绑定器添加属性
	spriteGroup->propertyBinder["backgroundColor"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			return { backgroundColor.r,backgroundColor.g,backgroundColor.b };
		},
		[&](eaPropertyValue value)
		{
			backgroundColor.r = value[1];
			backgroundColor.g = value[2];
			backgroundColor.b = value[3];
			backgroundColor.clearBackground = true;
		}
	);

	lua_rawgeti(L, LUA_REGISTRYINDEX, domain->GetEnvTableRef());

	// 创建scene
	lua_pushstring(L, "scene");
	lua_createtable(L, 0, 0);

	// scene.name = name
	lua_pushstring(L, "name");
	lua_pushstring(L, name.c_str());
	lua_settable(L, -3);

	// 使用__index来支持通过名称查找精灵
	lua_createtable(L, 0, 0);

	lua_pushstring(L, "__index");
	lua_pushinteger(L, (long long)this);
	lua_pushcclosure(L, SceneGet, 1);

	lua_settable(L, -3);
	
	lua_rawgeti(L, LUA_REGISTRYINDEX, spriteGroup->GetDomain()->GetEnvTableRef());
	lua_pushstring(L, "__newindex");
	lua_pushstring(L, "sprite");
	lua_gettable(L, -3);

	lua_settable(L, -4);

	lua_pop(L, 1);

	// 设置元表
	lua_setmetatable(L, -2);

	// 把scene对象放入域
	lua_settable(L, -3);
}

void eaScene::InitScript(std::string name)
{
	auto script = eaResources::Load<eaScript>(name);
	runner = make_unique<eaScriptRunner>(shared_from_this());
	runner->Run(script);
}	

void eaScene::Draw(SDL_Renderer* renderer)
{
	if (destroyed)
		eaApplication::GetLogger().Error("Scene", "无法渲染已销毁场景");

	if (backgroundColor.clearBackground)
	{
		SDL_SetRenderDrawColor(
			renderer,
			(Uint8)(255 * backgroundColor.r),
			(Uint8)(255 * backgroundColor.g),
			(Uint8)(255 * backgroundColor.b),
			0);
		SDL_RenderClear(renderer);
	}

	spriteGroup->Draw(renderer, 1);

	if (popScene != nullptr && !popScene->destroyed)
		popScene->Draw(renderer);
}

void eaScene::PopScene(string name, eaPropertyValue arg)
{
	popScene = Load(name, arg);
}
void eaScene::Update()
{
	if (destroyed)
		eaApplication::GetLogger().Error("Scene", "无法刷新已销毁场景");

	if (popScene != nullptr)
	{
		if (popScene->destroyed)
			popScene = nullptr;
		else
			popScene->Update();
	}
	else
	{
		if (runner->enable)
			runner->Update();

		spriteGroup->Update();
	}
}

void eaScene::Save()
{
	spriteGroup->Save();
}

void eaScene::Load()
{
	spriteGroup->Load();
}

shared_ptr<eaScene> eaScene::Load(std::string name, eaPropertyValue value)
{
	auto& L = eaApplication::GetLua();

	auto scene = shared_ptr<eaScene>(new eaScene(name));
	lua_rawgeti(L, LUA_REGISTRYINDEX, scene->GetDomain()->GetEnvTableRef());
	lua_pushstring(L, "arg");
	PushPropertyValue(L, value);
	lua_settable(L, -3);

	scene->InitScript(name);
	return scene;
}

shared_ptr<eaSprite> eaScene::GetSprite(std::string name)
{
	return spriteGroup->GetSprite(name);
}
