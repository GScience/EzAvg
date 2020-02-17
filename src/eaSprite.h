#pragma once

#include <functional>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include "eaLuaBridge.h"
#include "eaLuaDomain.h"
#include "eaSaveable.h"

struct SDL_Renderer;
class eaSprite;
class eaScene;

/*
属性
*/
struct eaPropertyValue
{
	using eaTable = std::map<eaPropertyValue, eaPropertyValue>;

	size_t typeHash;
	std::shared_ptr<void> ptr;

	eaPropertyValue(std::string value)
		: eaPropertyValue(std::make_shared<std::string>(value)) {}
	eaPropertyValue(double value)
		: eaPropertyValue(std::make_shared<double>(value)) {}
	eaPropertyValue(int value)
		: eaPropertyValue(std::make_shared<double>(value)) {}
	eaPropertyValue(eaTable value)
		: eaPropertyValue(std::make_shared<eaTable>(value)) {}
	eaPropertyValue(std::vector<eaPropertyValue> value)
		: eaPropertyValue(std::make_shared<eaTable>())
	{
		for (auto i = 0; i < value.size(); ++i)
			ToTable()[i + 1] = value[i];
	}
	eaPropertyValue(std::initializer_list<eaPropertyValue> value)
		: eaPropertyValue(std::vector<eaPropertyValue>(value)) {}

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
	eaPropertyValue(std::shared_ptr<eaTable> ptr) :ptr(ptr)
	{
		typeHash = typeid(decltype(ptr)::element_type).hash_code();
	}
	eaPropertyValue(nullptr_t ptr) :ptr(ptr)
	{
		typeHash = typeid(nullptr_t).hash_code();
	}

	eaPropertyValue() :eaPropertyValue(nullptr) {}

	operator std::shared_ptr<void>()
	{
		return ptr;
	}

	bool IsString() const
	{
		return typeHash == typeid(std::string).hash_code();
	}
	bool IsNumber() const
	{
		return typeHash == typeid(double).hash_code();
	}
	bool IsBoolean() const
	{
		return typeHash == typeid(bool).hash_code();
	}
	bool IsTable() const
	{
		return typeHash == typeid(eaTable).hash_code();
	}

	std::string ToString() const
	{
		return *std::reinterpret_pointer_cast<std::string>(ptr);
	}
	double ToNumber() const
	{
		return *std::reinterpret_pointer_cast<double>(ptr);
	}
	bool ToBoolean() const
	{
		return *std::reinterpret_pointer_cast<bool>(ptr);
	}
	eaTable& ToTable() const
	{
		return *std::reinterpret_pointer_cast<eaTable>(ptr);
	}

	operator std::string() const
	{
		return ToString();
	}
	operator double() const
	{
		return ToNumber();
	}
	operator int() const
	{
		return (int)ToNumber();
	}
	
	bool operator <(const eaPropertyValue& value) const
	{
		if (typeHash != value.typeHash)
			return typeHash < value.typeHash;

		if (IsBoolean()) return ToBoolean() < value.ToBoolean();
		if (IsString()) return ToString() < value.ToString();
		if (IsNumber()) return ToNumber() < value.ToNumber();
		if (IsTable()) return ToTable() < value.ToTable();

		return true;
	}

	template<class T> eaPropertyValue operator[](T key)
	{
		if (!IsTable())
			return nullptr;
		return ToTable()[key];
	}

	bool operator ==(nullptr_t)
	{
		return ptr == nullptr;
	}

	bool operator !=(nullptr_t)
	{
		return ptr != nullptr;
	}
};

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
	
	double alpha = 1;
	int zOrder = 0;

	eaSprite();
	virtual ~eaSprite() = default;

	std::vector<std::function<int(const std::string&)>> customLuaGetFunctions;
	std::vector<std::function<int(const std::string&, int)>> customLuaSetFunctions;

	bool enabled = true;
	bool destroyed = false;

	std::string name;

protected:
	eaRenderRect GetRenderRect();

public:
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
	virtual void Draw(SDL_Renderer* renderer) = 0;

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
