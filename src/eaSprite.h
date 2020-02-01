#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>
#include "eaLuaBridge.h"
#include "eaLuaDomain.h"
#include "eaSaveable.h"

struct SDL_Renderer;

/*
属性
*/
struct eaPropertyValue
{
	size_t typeHash;
	std::shared_ptr<void> ptr;

	eaPropertyValue(std::shared_ptr<bool> ptr) :ptr(ptr)
	{
		typeHash = typeid(decltype(ptr)::element_type).hash_code();
	}

	eaPropertyValue(std::shared_ptr<std::string> ptr) :ptr(ptr)
	{
		typeHash = typeid(decltype(ptr)::element_type).hash_code();
	}

	eaPropertyValue(std::shared_ptr<double> ptr) :ptr(ptr)
	{
		typeHash = typeid(decltype(ptr)::element_type).hash_code();
	}

	eaPropertyValue(nullptr_t ptr) :ptr(ptr)
	{
		typeHash = typeid(nullptr_t).hash_code();
	}

	operator std::shared_ptr<void>()
	{
		return ptr;
	}

	bool IsString()
	{
		return typeHash == typeid(std::string).hash_code();
	}

	bool IsNumber()
	{
		return typeHash == typeid(double).hash_code();
	}

	bool IsBoolean()
	{
		return typeHash == typeid(bool).hash_code();
	}

	std::string ToString()
	{
		return *std::reinterpret_pointer_cast<std::string>(ptr);
	}

	double ToNumber()
	{
		return *std::reinterpret_pointer_cast<double>(ptr);
	}

	bool ToBoolean()
	{
		return *std::reinterpret_pointer_cast<bool>(ptr);
	}
};

class eaSprite;

/*
精灵的行为，由Lua控制
*/
class eaSpriteBehaviour : public eaLuaBridge
{
public:
	eaSpriteBehaviour(eaSprite* scene, const std::string& name);

	void Update();
	void Start();
};

class eaScene;

/*
场景中所有元素均为精灵
*/
class eaSprite : public eaSaveable
{
	friend class eaScene;

	int behaviourIndex = 1;

	std::map<int, std::shared_ptr<eaSpriteBehaviour>> behaviours;
	std::shared_ptr<eaLuaDomain> sceneDomain;

public:
	std::string name;

	/*
	设置属性
	*/
	virtual void SetProperty(std::string name, eaPropertyValue obj) = 0;

	/*
	获取属性
	*/
	virtual eaPropertyValue GetProperty(std::string name) = 0;

	/*
	绘制
	*/
	virtual void Draw(SDL_Renderer* renderer) = 0;

	/*
	创建
	*/
	virtual void OnCreate() = 0;

	/*
	刷新
	*/
	virtual void Update();

	/*
	添加脚本，返回的数值为此行为的索引，若无手动添加则可保证唯一性
	*/
	int AddBehaviour(std::string name);

	/*
	获取域
	*/
	std::shared_ptr<eaLuaDomain> GetDomain()
	{
		return sceneDomain;
	}
};
