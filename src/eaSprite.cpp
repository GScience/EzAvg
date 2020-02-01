#include <lua.hpp>
#include <iostream>
#include "eaSprite.h"
#include "eaApplication.h"

using namespace std;

void eaSprite::Update()
{
	for (auto pair : behaviours)
		pair.second->Update();
}

int eaSprite::AddBehaviour(std::string name)
{
	auto behaviour = make_shared<eaSpriteBehaviour>(this, name);
	behaviours[behaviourIndex] = behaviour;
	behaviour->Start();
	return behaviourIndex++;
}

eaSpriteBehaviour::eaSpriteBehaviour(eaSprite* sprite, const std::string& name)
	:eaLuaBridge(sprite->GetDomain(), "behaviour/" + name)
{
}

void eaSpriteBehaviour::Update()
{
	if (objRef == LUA_REFNIL)
		return;

	auto & L = eaApplication::GetLua();
	lua_settop(L, 0);

	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
	lua_pushstring(L, "update");
	lua_gettable(L, -2);

	if (lua_pcall(L, 0, 0, 0) != LUA_OK)
	{
		cout << "���нű�" << name << "ʱ�����쳣" << endl;
		throw eaLuaError();
	}
}

void eaSpriteBehaviour::Start()
{
	auto& L = eaApplication::GetLua();
	lua_settop(L, 0);

	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
	lua_pushstring(L, "start");
	lua_gettable(L, -2);

	if (lua_pcall(L, 0, 0, 0) != LUA_OK)
	{
		cout << "�����ű�" << name << "ʱ�����쳣" << endl;
		throw eaLuaError();
	}
}
