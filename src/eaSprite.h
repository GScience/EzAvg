#pragma once

#include <functional>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include "eaLuaBridge.h"
#include "eaLuaDomain.h"
#include "eaSaveable.h"
#include "eaLua.h"

struct SDL_Renderer;
class eaSprite;
class eaScene;

/*
精灵的行为，由Lua控制
*/
class eaSpriteBehaviour : public eaLuaBridge
{
public:
	std::string name;

	eaSpriteBehaviour(eaSprite* scene, const std::string& name, const std::string& type);

	void SendMessage(const std::string& msg);

	void Update();

	bool IsEnabled();
	bool IsDestroyed();

	eaPropertyValue Get(const std::string& str);
	void Set(const std::string& str, eaPropertyValue value);
};

struct eaPropertyBinder
{
public:
	std::function<void(eaPropertyValue)> set;
	std::function<eaPropertyValue()> get;

	eaPropertyBinder(std::function<eaPropertyValue()> get, std::function<void(eaPropertyValue)> set) :
		get(get), set(set) {}

	eaPropertyBinder() = default;
};

/*
精灵大小
*/
struct eaSpriteSize
{
	int width = 0;
	int height = 0;
};

/*
渲染区域
*/
struct eaRenderRect
{
	int x;
	int y;
	int width;
	int height;
};

/*
边界
*/
struct eaSpriteMargin
{
	int up = 0;
	int right = 0;
	int bottom = 0;
	int left = 0;
};

/*
精灵盒
*/
struct eaSpriteBox
{
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
};

/*
场景中所有元素均为精灵
*/
class eaSprite : public eaSaveable, public std::enable_shared_from_this<eaSprite>
{
	std::vector<std::shared_ptr<eaSpriteBehaviour>> behaviours;
	std::shared_ptr<eaLuaDomain> domain;

public:
	std::map<std::string, eaPropertyBinder> propertyBinder;

	eaSpriteMargin margin;
	eaSpriteBox box;
	eaSpriteSize size;

	double alpha = 1;
	int zOrder = 0;

	eaSprite();
	virtual ~eaSprite() = default;

	std::vector<std::function<int(const std::string&)>> customLuaGetFunctions;
	std::vector<std::function<int(const std::string&, int)>> customLuaSetFunctions;

	bool enabled = true;
	bool destroyed = false;

	std::string name;

	eaRenderRect GetRenderRect();

	void SendMessage(const std::string& msg);

	int GetZOrder() const
	{
		return zOrder;
	}

	/*
	设置属性
	*/
	void SetProperty(const std::string& name, eaPropertyValue obj)
	{
		auto binder = propertyBinder.find(name);
		if (binder == propertyBinder.end())
			return;

		if (binder->second.set == nullptr)
			return;

		binder->second.set(obj);
	}

	/*
	获取属性
	*/
	eaPropertyValue GetProperty(const std::string& name)
	{
		auto binder = propertyBinder.find(name);
		if (binder == propertyBinder.end())
			return nullptr;

		if (binder->second.get == nullptr)
			return nullptr;

		return binder->second.get();
	}

	/*
	绘制
	*/
	virtual void Draw(SDL_Renderer* renderer, double groupAlpha) = 0;

	/*
	保存
	*/
	virtual void Save(std::shared_ptr<eaProfileNode> node)  override;
	virtual void Load(std::shared_ptr<eaProfileNode> node)  override;

	/*
	刷新
	*/
	virtual void Update();

	virtual void OnLayoutChanged() {}

	/*
	添加行为
	*/
	std::shared_ptr<eaSpriteBehaviour> AddBehaviour(const std::string& name, const std::string& type);

	/*
	获取行为
	*/
	std::shared_ptr<eaSpriteBehaviour> GetBehaviour(const std::string& name);

	/*
	获取域
	*/
	std::shared_ptr<eaLuaDomain> GetDomain()
	{
		return domain;
	}
	
	/*
	绑定域
	*/
	void BindDomain(std::shared_ptr<eaLuaDomain> ownerDomain);

	virtual std::string GetType() = 0;
};
