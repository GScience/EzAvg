#pragma once
#include <map>
#include "eaSprite.h"

/*
精灵组属性表，把精灵组内部精灵的属性暴露到外边
*/
using eaSpriteGroupPropertyTable = std::map<std::string, std::string>;


/*
精灵组，用来创建组合精灵，比如按钮
*/
class eaSpriteGroup : public eaSprite
{
private:
	std::vector<std::shared_ptr<eaSprite>> sprites;
	eaSpriteGroupPropertyTable propertyTable;

public:
	/*
	是否启用精灵组默认自动布局
	*/
	bool autoLayout = true;

	eaSpriteGroup();

	~eaSpriteGroup() override
	{

	}

	void Draw(SDL_Renderer* renderer, double groupAlpha) override;
	void Update() override;

	void Save(std::shared_ptr<eaProfileNode> node)  override;
	void Load(std::shared_ptr<eaProfileNode> node)  override;

	void OnLayoutChanged() override;

	std::shared_ptr<eaSprite> GetSprite(std::string name);
	const std::vector<std::shared_ptr<eaSprite>>& GetSprites() const
	{
		return sprites;
	}

	static std::shared_ptr<eaSpriteGroup> Create(std::shared_ptr<eaLuaDomain> owner, std::string name)
	{
		auto obj = std::shared_ptr<eaSpriteGroup>(new eaSpriteGroup());
		obj->name = name;
		obj->BindDomain(owner);
		obj->OnLayoutChanged();
		return obj;
	}

	std::shared_ptr<eaSprite> AddSprite(std::string name, std::string type);

	template<class T> std::shared_ptr<T> AddSprite(std::string name)
	{
		auto obj = std::shared_ptr<T>(new T());
		obj->name = name;
		obj->BindDomain(GetDomain());

		auto renderRect = GetRenderRect();
		obj->box.width = renderRect.width;
		obj->box.height = renderRect.height;
		obj->box.x = renderRect.x;
		obj->box.y = renderRect.y;

		obj->OnLayoutChanged();
		sprites.push_back(obj);
		return obj;
	}

	std::string GetType() override
	{
		return "group";
	}
};