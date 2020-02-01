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
����
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
�������Ϊ����Lua����
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
����������Ԫ�ؾ�Ϊ����
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
	��������
	*/
	virtual void SetProperty(std::string name, eaPropertyValue obj) = 0;

	/*
	��ȡ����
	*/
	virtual eaPropertyValue GetProperty(std::string name) = 0;

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
		return sceneDomain;
	}
};
