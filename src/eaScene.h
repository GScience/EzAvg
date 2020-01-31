#pragma once

#include <vector>
#include "eaSprite.h"
#include "eaSaveable.h"
#include "eaScriptRunner.h"

/*
场景，负责管理精灵
*/
class eaScene : public eaSaveable
{
	std::vector<std::shared_ptr<eaSprite>> sprites;

public:
	eaScriptRunner runner;

	eaScene() = default;
	eaScene(const eaScene&) = delete;

	virtual ~eaScene() = default;

	void Draw(SDL_Renderer* renderer);
	void Update();

	void Save() override;
	void Load() override;

	void RunScript(std::string name);

	template<class T> std::shared_ptr<T> AddSprite(std::string name)
	{
		if (GetSprite(name) != nullptr)
			return nullptr;

		auto sprite = make_shared<T>();
		sprite->name = name;
		sprites.emplace_back(sprite);
		return sprite;
	}

	void RemoveSprite(std::string name);
	std::shared_ptr<eaSprite> GetSprite(std::string name);
};