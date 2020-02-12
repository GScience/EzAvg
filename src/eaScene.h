#pragma once

#include <vector>
#include <memory>
#include "eaSpriteGroup.h"
#include "eaSprite.h"
#include "eaSaveable.h"
#include "eaScriptRunner.h"

struct eaSceneBackgroundColor
{
	double r, g, b = 0;
};

/*
场景，负责管理精灵
*/
class eaScene : public eaSaveable, public std::enable_shared_from_this<eaScene>
{
	std::shared_ptr<eaSpriteGroup> spriteGroup;
	std::shared_ptr<eaLuaDomain> domain;
	
	eaScene(std::string name);

	void InitScript(std::string name);

public:
	eaSceneBackgroundColor backgroundColor;
	std::unique_ptr<eaScriptRunner> runner;

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

		auto sprite = spriteGroup->Create<T>(name);
		return sprite;
	}

	std::shared_ptr<eaSprite> GetSprite(std::string name);

	static std::shared_ptr<eaScene> Load(std::string name);
};