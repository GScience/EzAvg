#include <SDL.h>
#include <lua.hpp>
#include <algorithm>
#include "eaApplication.h"
#include "eaResources.h"
#include "eaScene.h"

using namespace std;

// 获取精灵
static int SceneSpriteGet(lua_State* L)
{
	auto scene = (eaScene*)lua_tointeger(L, lua_upvalueindex(1));
	string name = lua_tostring(L, 2);

	auto sprite = scene->GetSprite(name);

	if (sprite != nullptr && sprite->enabled)
	{
		// 获取对应精灵域的sprite对象
		lua_rawgeti(L, LUA_REGISTRYINDEX, sprite->GetDomain()->GetEnvTableRef());
		lua_pushstring(L, "sprite");
		lua_gettable(L, -2);
	}
	else
		lua_pushnil(L);

	return 1;
}

eaScene::eaScene(string name)
{
	auto& L = eaApplication::GetLua();

	domain = eaLuaDomain::Create(name, eaApplication::GetDomain());

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
	lua_pushcclosure(L, SceneSpriteGet, 1);
	lua_settable(L, -3);

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
	sort(sprites.begin(), sprites.end(), [](std::shared_ptr<eaSprite> a, std::shared_ptr<eaSprite> b)
	{
		return (a->GetZOrder() < b->GetZOrder());
	});

	for (auto& sprite : sprites)
		sprite->Draw(renderer);
}

void eaScene::Update()
{
	if (runner->enable)
		runner->Update();

	for (auto sprite = sprites.begin(); sprite != sprites.end();)
	{
		if (!(*sprite)->enabled)
			sprite = sprites.erase(sprite);
		else
		{
			(*sprite)->Update();
			++sprite;
		}
	}
}

void eaScene::Save()
{
	for (auto& sprite : sprites)
		sprite->Save();
}

void eaScene::Load()
{
	for (auto& sprite : sprites)
		sprite->Load();
}

shared_ptr<eaScene> eaScene::Load(std::string name)
{
	auto scene = shared_ptr<eaScene>(new eaScene(name));
	scene->InitScript(name);
	return scene;
}

shared_ptr<eaSprite> eaScene::GetSprite(std::string name)
{
	for (auto sprite : sprites)
		if (sprite->name == name)
			return sprite;
	return nullptr;
}
