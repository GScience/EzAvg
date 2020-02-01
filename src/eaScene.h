#pragma once

#include <vector>
#include <memory>
#include "eaSprite.h"
#include "eaSaveable.h"
#include "eaScriptRunner.h"

/*
场景，负责管理精灵
*/
class eaScene : public eaSaveable
{
	std::vector<std::shared_ptr<eaSprite>> sprites;
	std::shared_ptr<eaLuaDomain> domain;

public:
	eaScriptRunner runner;

	eaScene(std::string name);
	eaScene(const eaScene&) = delete;

	virtual ~eaScene() = default;

	void Draw(SDL_Renderer* renderer);
	void Update();

	void Save() override;
	void Load() override;

	std::shared_ptr<eaLuaDomain> GetDomain() const
	{
		return domain;
	}

	template<class T> std::shared_ptr<T> AddSprite(std::string name)
	{
		if (GetSprite(name) != nullptr)
			return nullptr;

		auto sprite = std::make_shared<T>();
		sprite->sceneDomain = GetDomain();
		sprite->name = name;
		sprites.emplace_back(sprite);
		return sprite;
	}

	void RemoveSprite(std::string name);
	std::shared_ptr<eaSprite> GetSprite(std::string name);
};