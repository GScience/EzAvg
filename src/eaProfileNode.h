#pragma once
#include <map>
#include <memory>
#include <vector>
#include <string>
#include <istream>
#include <ostream>
#include "eaLua.h"

/*
保存节点
*/
class eaProfileNode
{
	std::map<std::string, std::shared_ptr<void>> data;
	std::vector<std::string> nodeList;

public:
	template<class T> std::shared_ptr<T> Get(const std::string& s)
	{
		auto result = data.find(s);
		if (result == data.end())
			return nullptr;
		return std::reinterpret_pointer_cast<T>(result->second);
	}

	std::shared_ptr<eaProfileNode> Set(const std::string& s)
	{
		auto ptr = std::make_shared<eaProfileNode>();
		data[s] = ptr;
		return ptr;
	}

	template<class ...Args> std::shared_ptr<eaPropertyValue> Set(const std::string& s, Args ...args)
	{
		auto ptr = std::make_shared<eaPropertyValue>(args...);
		data[s] = ptr;
		return ptr;
	}
	
	const std::map<std::string, std::shared_ptr<void>>& GetData() const
	{
		return data;
	}

	void WriteToStream(std::ostream stream);
	void ReadFromStream(std::istream stream);
};
