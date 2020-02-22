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

	std::shared_ptr<eaSprite> sprite = spriteGroup->AddSprite(spriteName, spriteType);

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

std::shared_ptr<eaSprite> eaSpriteGroup::AddSprite(std::string name, std::string type)
{
	if (type == "image")
		return AddSprite<eaSpriteImage>(name);
	else if (type == "text")
		return AddSprite<eaSpriteText>(name);
	else if (type == "group")
		return AddSprite<eaSpriteGroup>(name);
	return nullptr;
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
						// 目标精灵
						shared_ptr<eaSprite> sprite = shared_from_this();
						// 精灵所在精灵组
						auto spriteGroup = reinterpret_pointer_cast<eaSpriteGroup>(sprite);

						auto subName = spriteLoc.substr(0, spriteLoc.find_first_of('.'));

						// 一直寻找到非精灵组对象
						auto nextPos = subName.size() + 1;
						while (true)
						{
							auto subSprite = spriteGroup->GetSprite(subName);

							// 找不到就跑
							if (subSprite == nullptr)
								break;

							auto dotPos = spriteLoc.find_first_of('.', nextPos);
							if (dotPos == string::npos)
							{
								if (nextPos < spriteLoc.length())
									subName = spriteLoc.substr(nextPos);
								else
									subName = "";
							}
							else
								subName = spriteLoc.substr(nextPos, dotPos - nextPos);

							nextPos += subName.size() + 1;

							// 精灵组，还是精灵
							if (subSprite->GetType() != "group")
							{
								sprite = subSprite;
								break;
							}
							spriteGroup = reinterpret_pointer_cast<eaSpriteGroup>(subSprite);
						}


						// 如果subName不为空，则绑定到了行为属性
						if (subName != "")
						{
							auto behaviour = sprite->GetBehaviour(subName);
							if (behaviour != nullptr)
								return sprite->GetBehaviour(subName)->Get(propertyName);
							return nullptr;
						}
						return sprite->propertyBinder[propertyName].get();
					},
					[this, spriteLoc, propertyName](eaPropertyValue value)
					{
						// 目标精灵
						shared_ptr<eaSprite> sprite = shared_from_this();
						// 精灵所在精灵组
						auto spriteGroup = reinterpret_pointer_cast<eaSpriteGroup>(sprite);
						auto subName = spriteLoc.substr(0, spriteLoc.find_first_of('.'));

						// 一直寻找到非精灵组对象
						auto nextPos = subName.size() + 1;
						while (true)
						{
							auto subSprite = spriteGroup->GetSprite(subName);

							// 找不到就跑
							if (subSprite == nullptr)
								break;

							auto dotPos = spriteLoc.find_first_of('.', nextPos);
							if (dotPos == string::npos)
							{
								if (nextPos < spriteLoc.length())
									subName = spriteLoc.substr(nextPos);
								else
									subName = "";
							}
							else
								subName = spriteLoc.substr(nextPos, dotPos - nextPos);

							nextPos += subName.size() + 1;

							// 精灵组，还是精灵
							if (subSprite->GetType() != "group")
							{
								sprite = subSprite;
								break;
							}
							spriteGroup = reinterpret_pointer_cast<eaSpriteGroup>(subSprite);
						}

						// 如果subName不为空，则绑定到了行为属性
						if (subName != "")
						{
							auto behaviour = sprite->GetBehaviour(subName);
							if (behaviour != nullptr)
								return behaviour->Set(propertyName, value);
							return;
						}
						return sprite->propertyBinder[propertyName].set(value);
					}
					);
				propertyTable[pair.first] = pair.second;
			}
		}
		);
}

void eaSpriteGroup::Draw(SDL_Renderer* renderer, double groupAlpha)
{
	sort(sprites.begin(), sprites.end(), [](std::shared_ptr<eaSprite> a, std::shared_ptr<eaSprite> b)
		{
			return (a->GetZOrder() < b->GetZOrder());
		});

	for (auto& sprite : sprites)
	{
		if (sprite->enabled && !sprite->destroyed)
			sprite->Draw(renderer, groupAlpha * alpha);
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

void eaSpriteGroup::Save(shared_ptr<eaProfileNode> node) 
{
	eaSprite::Save(node);
	auto spritesNode = node->Set("Sprites");
	for (auto& sprite : sprites)
	{
		// 为精灵创建节点
		auto spriteNode = spritesNode->Set(sprite->name);
		auto spriteType = sprite->GetType();

		auto _ = spriteNode->Set("type", spriteType);

		if (spriteType == "group")
			reinterpret_pointer_cast<eaSpriteGroup>(sprite)->Save(spriteNode);
		else
			sprite->Save(spriteNode);
	}
}

void eaSpriteGroup::Load(shared_ptr<eaProfileNode> node) 
{
	// 优先加载属性表
	auto propertyTable = node->Get<eaPropertyValue>("propertyTable");
	SetProperty("propertyTable", *propertyTable);
	eaSprite::Load(node);
	auto spritesNode = node->Get<eaProfileNode>("Sprites");
	for (auto& data : spritesNode->GetData())
	{
		auto name = data.first;
		auto spriteNode = reinterpret_pointer_cast<eaProfileNode>(data.second);

		auto spriteType = spriteNode->Get<eaPropertyValue>("type");

		auto sprite = AddSprite(name, *spriteType);
		sprite->Load(spriteNode);
	}
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