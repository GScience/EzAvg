#pragma once
#include "eaSprite.h"

/*
精灵组，用来创建组合精灵，比如按钮
*/
class eaSpriteGroup : public eaSprite
{
private:
	std::vector<std::shared_ptr<eaSprite>> sprites;

public:
	eaSpriteGroup();

	void Draw(SDL_Renderer* renderer) override;
	void Update() override;

	void Save() override;
	void Load() override;

	std::shared_ptr<eaSprite> GetSprite(std::string name);

	static std::shared_ptr<eaSpriteGroup> Create(std::shared_ptr<eaLuaDomain> owner, std::string name)
	{
		auto obj = std::shared_ptr<eaSpriteGroup>(new eaSpriteGroup());
		obj->name = name;
		obj->BindDomain(owner);
		obj->OnMove();
		obj->OnResize();
		return obj;
	}

	template<class T> std::shared_ptr<T> AddSprite(std::string name)
	{
		auto obj = std::shared_ptr<T>(new T());
		obj->name = name;
		obj->BindDomain(GetDomain());
		obj->OnMove();
		obj->OnResize();
		sprites.push_back(obj);
		return obj;
	}
};