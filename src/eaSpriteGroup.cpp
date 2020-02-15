#include <lua.hpp>
#include <algorithm>
#include "eaApplication.h"
#include "eaSpriteGroup.h"
#include "eaSpriteImage.h"
#include "eaSpriteText.h"

using namespace std;

static int SpriteGroupAddSprite(lua_State* L)
{
	auto spriteGroup = (eaSpriteGroup*)lua_tointeger(L, lua_upvalueindex(1));

	string spriteName = lua_tostring(L, 1);
	string spriteType = lua_tostring(L, 2);

	std::shared_ptr<eaSprite> sprite;

	if (spriteType == "image")
		sprite = spriteGroup->AddSprite<eaSpriteImage>(spriteName);
	else if (spriteType == "text")
		sprite = spriteGroup->AddSprite<eaSpriteText>(spriteName);
	else if (spriteType == "group")
		sprite = spriteGroup->AddSprite<eaSpriteGroup>(spriteName);

	if (sprite == nullptr)
		lua_pushnil(L);
	else
	{
		lua_rawgeti(L, LUA_REGISTRYINDEX, sprite->GetDomain()->GetEnvTableRef());
		lua_pushstring(L, "sprite");
		lua_gettable(L, -2);
	}
	return 1;
}

static int SpriteGroupSpriteIterator(lua_State* L)
{
	auto spriteGroup = (eaSpriteGroup*)lua_tointeger(L, lua_upvalueindex(1));

	size_t currentNumber = 0;

	if (lua_isinteger(L, 2))
		currentNumber = (size_t)lua_tointeger(L, 2);

	auto sprites = spriteGroup->GetSprites();

	++currentNumber;

	if (currentNumber > sprites.size())
		return 0;

	auto sprite = sprites[currentNumber - 1];

	lua_pushinteger(L, currentNumber);
	lua_rawgeti(L, LUA_REGISTRYINDEX, sprite->GetDomain()->GetEnvTableRef());
	lua_pushstring(L, "sprite");
	lua_gettable(L, -2);
	lua_replace(L, -2);

	return 2;
}

eaSpriteGroup::eaSpriteGroup()
{
	auto& L = eaApplication::GetLua();

	// 获取精灵组内精灵和创建精灵
	customLuaGetFunctions.push_back([&](string name)
	{
		// SpriteGroup方法
		if (name == "addSprite")
		{
			lua_pushinteger(L, (long long)this);
			lua_pushcclosure(L, SpriteGroupAddSprite, 1);
			return 1;
		}
		else if (name == "iter")
		{
			lua_pushinteger(L, (long long)this);
			lua_pushcclosure(L, SpriteGroupSpriteIterator, 1);
			return 1;
		}

		// SpriteGroup子精灵索引
		auto sprite = GetSprite(name);
		if (sprite == nullptr)
			return 0;

		lua_rawgeti(L, LUA_REGISTRYINDEX, sprite->GetDomain()->GetEnvTableRef());
		lua_pushstring(L, "sprite");
		lua_gettable(L, -2);

		return 1;
	});

	propertyBinder["autoLayout"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			return autoLayout;
		},
		[&](eaPropertyValue value)
		{
			autoLayout = value.ToBoolean();
		}
		);
	propertyBinder["propertyTable"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			auto table = std::make_shared<eaPropertyValue::eaTable>();

			for (auto& pair : propertyTable)
				(*table)[pair.first] = pair.second;

			return eaPropertyValue(table);
		},
		[&](eaPropertyValue value)
		{
			// 移除旧的绑定
			for (auto& pair : propertyTable)
				propertyBinder.erase(pair.first);
			propertyTable.clear();

			auto table = value.ToTable();

			// 创建新的绑定
			for (auto& pair : table)
			{
				if (propertyBinder.find(pair.first) != propertyBinder.end())
					continue;

				auto propertyLoc = pair.second.ToString();
				auto spriteLoc = propertyLoc.substr(0, propertyLoc.find_last_of('.'));
				auto propertyName = propertyLoc.substr(propertyLoc.find_last_of('.') + 1);

				propertyBinder[pair.first] = eaPropertyBinder(
					[this, spriteLoc, propertyName]()->eaPropertyValue
					{
						auto sprite = this;
						auto subSpriteName = spriteLoc.substr(0, spriteLoc.find_last_of('.'));

						while (subSpriteName != "")
						{
							sprite = (eaSpriteGroup*)(sprite->GetSprite(subSpriteName).operator->());
							auto dotPos = spriteLoc.find_first_of('.', subSpriteName.size());
							if (dotPos == string::npos)
								subSpriteName = "";
							else
								subSpriteName = spriteLoc.substr(dotPos);
						}

						return sprite->propertyBinder[propertyName].get();
					},
					[this, spriteLoc, propertyName](eaPropertyValue value)
					{
						auto sprite = this;
						auto subSpriteName = spriteLoc.substr(0, spriteLoc.find_first_of('.'));

						while (subSpriteName != "")
						{
							sprite = (eaSpriteGroup*)(sprite->GetSprite(subSpriteName).operator->());
							auto dotPos = spriteLoc.find_first_of('.', subSpriteName.size());
							if (dotPos == string::npos)
								subSpriteName = "";
							else
								subSpriteName = spriteLoc.substr(dotPos);
						}
						sprite->propertyBinder[propertyName].set(value);
					}
					);
				propertyTable[pair.first] = pair.second;
			}
		}
		);
}

void eaSpriteGroup::Draw(SDL_Renderer* renderer)
{
	sort(sprites.begin(), sprites.end(), [](std::shared_ptr<eaSprite> a, std::shared_ptr<eaSprite> b)
		{
			return (a->GetZOrder() < b->GetZOrder());
		});

	for (auto& sprite : sprites)
	{
		if (sprite->enabled && !sprite->destroyed)
			sprite->Draw(renderer);
	}
}

void eaSpriteGroup::Update()
{
	eaSprite::Update();

	for (auto sprite = sprites.begin(); sprite != sprites.end();)
	{
		if ((*sprite)->destroyed)
		{
			sprite = sprites.erase(sprite);
			continue;
		}
		else if ((*sprite)->enabled)
		{
			(*sprite)->Update();
		}
		++sprite;
	}
}

void eaSpriteGroup::Save()
{
}

void eaSpriteGroup::Load()
{
}

void eaSpriteGroup::OnLayoutChanged()
{
	if (!autoLayout)
		return;

	for (auto sprite : sprites)
	{
		auto renderRect = GetRenderRect();

		sprite->box.width = renderRect.width;
		sprite->box.height = renderRect.height;
		sprite->box.x = renderRect.x;
		sprite->box.y = renderRect.y;

		sprite->OnLayoutChanged();
	}
}

shared_ptr<eaSprite> eaSpriteGroup::GetSprite(string name)
{
	for (auto sprite : sprites)
		if (sprite->name == name)
			return sprite;
	return nullptr;
}