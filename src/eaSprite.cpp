#include <lua.hpp>
#include <SDL.h>
#include "eaSprite.h"
#include "eaApplication.h"

using namespace std;

void eaSprite::Update()
{
	for (auto behaviour = behaviours.begin(); behaviour != behaviours.end();)
	{
		if ((*behaviour)->IsDestroyed())
		{
			behaviour = behaviours.erase(behaviour);
			continue;
		}
		else if ((*behaviour)->IsEnabled())
			(*behaviour)->Update();
		++behaviour;
	}
}

std::shared_ptr<eaSpriteBehaviour> eaSprite::AddBehaviour(const string& name, const string& type)
{
	auto newBehaviour = make_shared<eaSpriteBehaviour>(this, name, type);
	behaviours.push_back(newBehaviour);

	return newBehaviour;
}

std::shared_ptr<eaSpriteBehaviour> eaSprite::GetBehaviour(const string& name)
{
	for (auto behaviour : behaviours)
	{
		if (behaviour->name == name && !behaviour->IsDestroyed())
			return behaviour;
	}
	return nullptr;
}

eaSprite::eaSprite()
{
	propertyBinder["name"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return name;
		},
		[&](eaPropertyValue value)
		{
			name = value.ToString();
		}
	);
	propertyBinder["type"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return shared_from_this()->GetType();
		},
		nullptr
	);
	propertyBinder["enabled"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return eaPropertyValue(std::make_shared<bool>(enabled));
		},
		[&](eaPropertyValue value)
		{
			enabled = value.ToBoolean();
		}
	);
	propertyBinder["destroyed"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return eaPropertyValue(std::make_shared<bool>(destroyed));
		},
		[&](eaPropertyValue value)
		{
			destroyed = value.ToBoolean();
		}
	);
	propertyBinder["zOrder"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return zOrder;
		},
		[&](eaPropertyValue value)
		{
			zOrder = value;
		}
	);
	propertyBinder["margin"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return { margin.up,margin.right,margin.bottom,margin.left };
		},
		[&](eaPropertyValue value)
		{
			if (value.ToTable().size() == 2)
			{
				margin.up = value[1];
				margin.left = value[2];
			}
			else
			{
				margin.up = value[1];
				margin.right = value[2];
				margin.bottom = value[3];
				margin.left = value[4];
			}
			shared_from_this()->OnLayoutChanged();
		}
	);
	propertyBinder["size"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return { size.width, size.height };
		},
		[&](eaPropertyValue value)
		{
			size.width = value[1];
			size.height = value[2];
			shared_from_this()->OnLayoutChanged();
		}
		);
	propertyBinder["box"] = eaPropertyBinder
	(
		[&]()->eaPropertyValue
		{
			return
			{
				box.x,box.y,
				box.width,box.height
			};
		},
		[&](eaPropertyValue value)
		{
			box.x = value[1];
			box.y = value[2];
			box.width = value[3];
			box.height = value[4];
			shared_from_this()->OnLayoutChanged();
		}
	);
	propertyBinder["alpha"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			return alpha;
		},
		[&](eaPropertyValue obj)
		{
			alpha = obj.ToNumber();
		}
	);
}

eaRenderRect eaSprite::GetRenderRect()
{
	auto actualMargin = margin;

	if (size.width != 0)
	{
		if (actualMargin.right == 0)
			actualMargin.right = box.width - actualMargin.left - size.width;
		else if (actualMargin.left == 0)
			actualMargin.left = box.width - actualMargin.left - size.width;
	}
	
	if (size.height != 0)
	{
		if (actualMargin.bottom == 0)
			actualMargin.bottom = box.height - actualMargin.up - size.height;
		else if (actualMargin.up == 0)
			actualMargin.up = box.height - actualMargin.bottom - size.height;
	}

	int x = (int)(box.x + actualMargin.left);
	int y = (int)(box.y + actualMargin.up);

	int width = box.width - actualMargin.right - actualMargin.left;
	int height = box.height - actualMargin.bottom - actualMargin.up;

	return eaRenderRect
	{
		x,y,
		width,height
	};
}


/*
设置属性
*/
static int SpritePropertySet(lua_State* L)
{
	auto sprite = (eaSprite*)lua_tointeger(L, lua_upvalueindex(1));
	string name = lua_tostring(L, 2);

	// 设置自定义对象
	for (auto func : sprite->customLuaSetFunctions)
	{
		int result = func(name, 3);
		if (result != 0)
			return result;
	}

	sprite->SetProperty(name, ToPropertyValue(L, 3));

	return 0;
}

/*
创建行为
*/
static int SpriteAddBehaviour(lua_State* L)
{
	auto sprite = (eaSprite*)lua_tointeger(L, lua_upvalueindex(1));

	string behaviourName = lua_tostring(L, 1);
	string behaviourType = lua_tostring(L, 2);

	auto behaviour = sprite->AddBehaviour(behaviourName, behaviourType);

	if (behaviour == nullptr)
		lua_pushnil(L);
	else
		lua_rawgeti(L, LUA_REGISTRYINDEX, behaviour->GetObjRef());
	
	return 1;
}

/*
获取属性
*/
static int SpritePropertyGet(lua_State* L)
{
	auto sprite = (eaSprite*)lua_tointeger(L, lua_upvalueindex(1));
	string name = lua_tostring(L, 2);

	// 方法
	if (name == "addBehaviour")
	{
		lua_pushinteger(L, (long long)sprite);
		lua_pushcclosure(L, SpriteAddBehaviour, 1);
		return 1;
	}

	// 获取自定义对象
	for (auto func : sprite->customLuaGetFunctions)
	{
		int result = func(name);
		if (result != 0)
			return result;
	}

	// 尝试获取精灵属性
	auto value = sprite->GetProperty(name);

	if (value != nullptr)
	{
		PushPropertyValue(L, value);

		return 1;
	}

	//尝试获取行为脚本
	auto behaviour = sprite->GetBehaviour(name);
	
	if (behaviour != nullptr)
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, behaviour->GetObjRef());
		return 1;
	}

	lua_pushnil(L);
	return 1;
}

void eaSprite::SendMessage(const string& msg)
{
	for (auto behaviour : behaviours)
		behaviour->SendMessage(msg);
}

void eaSprite::BindDomain(std::shared_ptr<eaLuaDomain> ownerDomain)
{
	auto& L = eaApplication::GetLua();

	domain = eaLuaDomain::Create("Sprite", ownerDomain);

	// 创建sprite对象，控制精灵属性等
	lua_rawgeti(L, LUA_REGISTRYINDEX, domain->GetEnvTableRef());
	lua_pushstring(L, "sprite");
	lua_createtable(L, 0, 0);

	// 创建元表
	lua_createtable(L, 0, 0);
	lua_pushstring(L, "__index");
	lua_pushinteger(L, (long long)this);
	lua_pushcclosure(L, SpritePropertyGet, 1);
	lua_settable(L, -3);
	lua_pushstring(L, "__newindex");
	lua_pushinteger(L, (long long)this);
	lua_pushcclosure(L, SpritePropertySet, 1);
	lua_settable(L, -3);
	
	// 设置元表
	lua_setmetatable(L, -2);

	// 把sprite对象放入域
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
		eaApplication::GetLogger().Error("Lua", "刷新行为"s + type + "时出现异常。位置：" + L.GetCurrentInfo());
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

	return isEnable;
}


bool eaSpriteBehaviour::IsDestroyed()
{
	if (objRef == LUA_REFNIL)
		return false;

	auto & L = eaApplication::GetLua();

	// behaviour.enabled
	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
	lua_pushstring(L, "destroyed");
	lua_gettable(L, -2);
	bool isEnable = lua_toboolean(L, -1);
	lua_pop(L, 1);

	return isEnable;
}

void eaSpriteBehaviour::SendMessage(const string& msg)
{
	if (objRef == LUA_REFNIL)
		return;

	auto & L = eaApplication::GetLua();

	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
	lua_pushstring(L, msg.c_str());
	lua_gettable(L, -2);

	if (!lua_isfunction(L, -1))
		return;

	if (lua_pcall(L, 0, 0, 0) != LUA_OK)
	{
		eaApplication::GetLogger().Error("Lua", "发送消息"s + msg + "时出现异常。位置：" + L.GetCurrentInfo());
	}
}

eaPropertyValue eaSpriteBehaviour::Get(const std::string& str)
{
	auto& L = eaApplication::GetLua();

	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
	lua_pushstring(L, str.c_str());
	lua_gettable(L, -2);

	if (lua_isfunction(L, -1))
	{
		if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK)
		{
			eaApplication::GetLogger().Error("Lua", "获取属性" + str + "时出现异常。位置：" + L.GetCurrentInfo());
		}
	}
	return ToPropertyValue(L, -1);
}

void eaSpriteBehaviour::Set(const std::string& str, eaPropertyValue value)
{
	auto& L = eaApplication::GetLua();

	lua_rawgeti(L, LUA_REGISTRYINDEX, objRef);
	lua_pushstring(L, str.c_str());
	if (lua_isfunction(L, -1))
	{
		eaApplication::GetLogger().Error("Lua", "属性只读，因为其指向一个lua函数。位置：" + L.GetCurrentInfo());
	}
	PushPropertyValue(L, value);
	lua_settable(L, -3);
}