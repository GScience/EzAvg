#include <SDL.h>
#include <lua.hpp>
#include "eaApplication.h"
#include "eaResources.h"
#include "eaScene.h"

using namespace std;

eaScene::eaScene(string name)
{
	auto& L = eaApplication::GetLua();

	domain = eaLuaDomain::Create(name, eaApplication::GetDomain());

	lua_rawgeti(L, LUA_REGISTRYINDEX, domain->GetEnvTableRef());
	lua_pushstring(L, "scene");
	lua_pushinteger(L, (long long)this);
	lua_settable(L, -3);
	lua_pop(L, 1);
}

void eaScene::InitScript(std::string name)
{
	auto script = eaResources::Load<eaScript>(name);
	runner = make_unique<eaScriptRunner>(shared_from_this());
	runner->Run(script);
}	

void eaScene::Draw(SDL_Renderer* renderer)
{
	for (auto& sprite : sprites)
		sprite->Draw(renderer);
}

void eaScene::Update()
{
	if (runner->enable)
		runner->Update();

	for (auto& sprite : sprites)
		sprite->Update();
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

void eaScene::RemoveSprite(std::string name)
{
	for (auto sprite = sprites.begin(); sprite != sprites.end(); ++sprite)
	{
		if ((*sprite)->name != name)
			continue;

		sprites.erase(sprite);
		return;
	}
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
