#pragma once
#include <string>
#include <functional>
#include <memory>
#include <map>

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

	bool operator <(const eaPropertyValue & value) const
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

using eaLuaNumber = double;
using eaLuaBoolean = bool;
using eaLuaString = std::string;

struct lua_State;

class eaLua
{
	lua_State* L;
	
public:
	eaLua();
	eaLua(const eaLua&) = delete;

	void StartDebuger();
	std::string& GetCurrentInfo() const;

private:
	void LoadFunction();

public:
	operator lua_State* () const
	{
		return L;
	}
	
	void Update();
};


void PushPropertyValue(lua_State* L, eaPropertyValue value);
eaPropertyValue ToPropertyValue(lua_State* L, int index);