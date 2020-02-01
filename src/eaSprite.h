#pragma once

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
����
*/
struct eaPropertyValue
{
	size_t typeHash;
	std::shared_ptr<void> ptr;

	eaPropertyValue(bool value)
		: eaPropertyValue(std::make_shared<bool>(value)) {}

	eaPropertyValue(std::string value)
		: eaPropertyValue(std::make_shared<std::string>(value)) {}

	eaPropertyValue(double value)
		: eaPropertyValue(std::make_shared<double>(value)) {}

	eaPropertyValue(int value)
		: eaPropertyValue(std::make_shared<double>(value)) {}

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

/*
�������Ϊ����Lua����
*/
class eaSpriteBehaviour : public eaLuaBridge
{
public:
	eaSpriteBehaviour(eaSprite* scene, const std::string& name);

	void Update();
	void Start();
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
����������Ԫ�ؾ�Ϊ����
*/
class eaSprite : public eaSaveable, public std::enable_shared_from_this<eaSprite>
{
	int behaviourIndex = 1;

	std::map<int, std::shared_ptr<eaSpriteBehaviour>> behaviours;
	std::shared_ptr<eaLuaDomain> domain;
	std::shared_ptr<eaScene> scene;

	void CreateDomain();

protected:
	std::map<std::string, eaPropertyBinder> propertyBinder;

	eaSprite() = default;

public:
	std::string name;

	/*
	��������
	*/
	virtual void SetProperty(std::string name, eaPropertyValue obj)
	{
		auto binder = propertyBinder.find(name);
		if (binder == propertyBinder.end())
			return;

		if (binder->second.set == nullptr)
			return;

		binder->second.set(obj);
	}

	/*
	��ȡ����
	*/
	virtual eaPropertyValue GetProperty(std::string name) 
	{
		auto binder = propertyBinder.find(name);
		if (binder == propertyBinder.end())
			return nullptr;

		if (binder->second.get == nullptr)
			return nullptr;

		return binder->second.get();
	}

	/*
	����
	*/
	virtual void Draw(SDL_Renderer* renderer) = 0;

	/*
	����
	*/
	virtual void OnCreate() = 0;

	/*
	ˢ��
	*/
	virtual void Update();

	/*
	��ӽű������ص���ֵΪ����Ϊ�������������ֶ������ɱ�֤Ψһ��
	*/
	int AddBehaviour(std::string name);

	/*
	��ȡ��
	*/
	std::shared_ptr<eaLuaDomain> GetDomain()
	{
		return domain;
	}

	template<class T> static std::shared_ptr<T> Create(std::shared_ptr<eaScene> scene, std::string name)
	{
		auto obj = std::shared_ptr<T>(new T());
		obj->scene = scene;
		obj->name = name;
		obj->CreateDomain();
		return obj;
	}
};
