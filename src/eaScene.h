#pragma once

#include <vector>
#include <memory>
#include "eaSpriteGroup.h"
#include "eaSprite.h"
#include "eaSaveable.h"
#include "eaScriptRunner.h"

struct eaSceneBackgroundColor
{
	double r = -1, g = -1, b = -1;
};

/*
场景，负责管理精灵
*/
class eaScene : public eaSaveable, public std::enable_shared_from_this<eaScene>
{
public:
	std::string name;

private:
	std::shared_ptr<eaSpriteGroup> spriteGroup;
	std::shared_ptr<eaLuaDomain> domain;
	
	bool destroyed = false;

	eaScene(std::string name);

	void InitScript(std::string name);

	std::shared_ptr<eaScene> popScene;
	eaPropertyValue result;
	eaPropertyValue popSceneResult;
public:
	/*
	是否需要保存
	*/
	bool needSave = true;

	std::shared_ptr<eaSpriteGroup> GetSpriteGroup()
	{
		return spriteGroup;
	}

	eaSceneBackgroundColor backgroundColor;
	std::unique_ptr<eaScriptRunner> runner;

	eaScene(const eaScene&) = delete;

	virtual ~eaScene() = default;

	void Draw(SDL_Renderer* renderer);
	void Update();

	void Save(std::shared_ptr<eaProfileNode> node)  override;
	void Load(std::shared_ptr<eaProfileNode> node)  override;

	void PopScene(std::string sceneName, eaPropertyValue arg = nullptr);

	std::shared_ptr<eaScene> GetPopScene()
	{
		return popScene;
	}

	const eaPropertyValue& GetPopSceneResult() const
	{
		return popSceneResult;
	}

	void Close(eaPropertyValue result);

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

	static std::shared_ptr<eaScene> Load(std::string name, eaPropertyValue arg = nullptr);
};