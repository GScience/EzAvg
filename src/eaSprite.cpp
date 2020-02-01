#include <lua.hpp>
#include <iostream>
#include "eaSprite.h"
#include "eaApplication.h"

using namespace std;

void eaSprite::Update()
{
	for (auto behaviour = behaviours.begin(); behaviour != behaviours.end();)
	{
		if (!(*behaviour)->IsEnabled())
			behaviour = behaviours.erase(behaviour);
		else
		{
			(*behaviour)->Update();
			++behaviour;
		}
	}
}

std::shared_ptr<eaSpriteBehaviour> eaSprite::AddBehaviour(std::string name, std::string type)
{
	auto newBehaviour = make_shared<eaSpriteBehaviour>(this, name, type);
	behaviours.push_back(newBehaviour);
	newBehaviour->Start();

	return newBehaviour;
}

std::shared_ptr<eaSpriteBehaviour> eaSprite::GetBehaviour(std::string name)
{
	for (auto behaviour : behaviours)
		if (behaviour->name == name)
			return behaviour;
	return nullptr;
}

eaSprite::eaSprite()
{
	propertyBinder["enabled"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return enabled;
		},
		[&](eaPropertyValue value)
		{
			enabled = value.ToBoolean();
		}
	);
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

	// ���Ի�ȡ��������
	auto value = sprite->GetProperty(name);

	if (value != nullptr)
	{
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

	//���Ի�ȡ��Ϊ�ű�
	auto behaviour = sprite->GetBehaviour(name);
	
	if (behaviour != nullptr && behaviour->IsEnabled())
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, behaviour->GetObjRef());
		return 1;
	}

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

	// sprite.name = name
	lua_pushstring(L, "name");
	lua_pushstring(L, name.c_str());
	lua_settable(L, -3);

	// ��sprite���������
	lua_settable(L, -3);
}

eaSpriteBehaviour::eaSpriteBehaviour(eaSprite* sprite, const std::string& name, const std::string& type)
	:eaLuaBridge(sprite->GetDomain(), "behaviour/" + type), name(name)
{
	auto& L = eaApplication::GetLua();

	// behaviour.name = name
	lua_rawgeti(L, LUA_REGISTRYINDEX, domain->GetEnvTableRef());
	lua_pushstring(L, "name");
	lua_pushstring(L, name.c_str());
	lua_settable(L, -3);
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
		cout << "���нű�" << type << "ʱ�����쳣" << endl;
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
		cout << "�����ű�" << type << "ʱ�����쳣" << endl;
		throw eaLuaError();
	}
}

bool eaSpriteBehaviour::IsEnabled()
{
	if (objRef == LUA_REFNIL)
		return false;

	auto & L = eaApplication::GetLua();

	// behaviour.enabled
	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
	lua_pushstring(L, "enabled");
	lua_gettable(L, -2);
	bool isEnable = lua_toboolean(L, -1);
	lua_pop(L, 1);

	if (!isEnable)
		Dispose();

	return isEnable;
}