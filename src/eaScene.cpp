#include <SDL.h>
#include <lua.hpp>
#include <algorithm>
#include "eaApplication.h"
#include "eaResources.h"
#include "eaScene.h"

using namespace std;

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

	lua_rawgeti(L, LUA_REGISTRYINDEX, spriteGroup->GetDomain()->GetEnvTableRef());
	lua_pushstring(L, "__index");
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
	spriteGroup->Draw(renderer);
}

void eaScene::Update()
{
	if (runner->enable)
		runner->Update();

	spriteGroup->Update();
}

void eaScene::Save()
{
	spriteGroup->Save();
}

void eaScene::Load()
{
	spriteGroup->Load();
}

shared_ptr<eaScene> eaScene::Load(std::string name)
{
	auto scene = shared_ptr<eaScene>(new eaScene(name));
	scene->InitScript(name);
	return scene;
}

shared_ptr<eaSprite> eaScene::GetSprite(std::string name)
{
	return spriteGroup->GetSprite(name);
}
