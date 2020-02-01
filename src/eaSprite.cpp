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

/*
��ȡ����
*/
static int SpritePropertySet(lua_State* L)
{
	auto sprite = (eaSprite*)lua_tointeger(L, lua_upvalueindex(1));
	string name = lua_tostring(L, 2);

	switch (lua_type(L, 3))
	{
	case LUA_TNUMBER:
		sprite->SetProperty(name,
			make_shared<double>(lua_tonumber(L, 3))
		);
		break;
	case LUA_TBOOLEAN:
		sprite->SetProperty(name,
			make_shared<bool>(lua_toboolean(L, 3))
		);
		break;
	case LUA_TSTRING:
		sprite->SetProperty(name,
			make_shared<string>(lua_tostring(L, 3))
		);
		break;
	default:
		sprite->SetProperty(name, nullptr);
		break;
	}

	return 0;
}

/*
��������
*/
static int SpritePropertyGet(lua_State* L)
{
	auto sprite = (eaSprite*)lua_tointeger(L, lua_upvalueindex(1));
	string name = lua_tostring(L, 2);

	auto value = sprite->GetProperty(name);

	if (value.IsBoolean())
		lua_pushboolean(L, value.ToBoolean());
	else if (value.IsNumber())
		lua_pushnumber(L, value.ToNumber());
	else if (value.IsString())
		lua_pushstring(L, value.ToString().c_str());
	else
		lua_pushnil(L);

	return 1;
}

void eaSprite::CreateDomain()
{
	auto& L = eaApplication::GetLua();

	domain = eaLuaDomain::Create("Sprite", scene->GetDomain());

	// ����sprite���󣬿��ƾ������Ե�
	lua_rawgeti(L, LUA_REGISTRYINDEX, domain->GetEnvTableRef());
	lua_pushstring(L, "sprite");
	lua_createtable(L, 0, 0);

	// ����Ԫ��
	lua_createtable(L, 0, 0);
	lua_pushstring(L, "__index");
	lua_pushinteger(L, (long long)this);
	lua_pushcclosure(L, SpritePropertyGet, 1);
	lua_settable(L, -3);
	lua_pushstring(L, "__newindex");
	lua_pushinteger(L, (long long)this);
	lua_pushcclosure(L, SpritePropertySet, 1);
	lua_settable(L, -3);
	
	// ����Ԫ��
	lua_setmetatable(L, -2);

	// ��sprite���������
	lua_settable(L, -3);
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

	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
	lua_pushstring(L, "start");
	lua_gettable(L, -2);

	if (lua_pcall(L, 0, 0, 0) != LUA_OK)
	{
		cout << "�����ű�" << name << "ʱ�����쳣" << endl;
		throw eaLuaError();
	}
}