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

enum DataType
{
	DataNode = 0, DataPropertyValue = 1
};

class eaProfileNode
{
	std::map<std::string, std::pair<DataType, std::shared_ptr<void>>> data;

public:
	template<class T> std::shared_ptr<T> Get(const std::string& s)
	{
		auto result = data.find(s);
		if (result == data.end())
			return nullptr;
		return std::reinterpret_pointer_cast<T>(result->second.second);
	}

	std::shared_ptr<eaProfileNode> Set(const std::string& s)
	{
		auto ptr = std::make_shared<eaProfileNode>();
		data[s] = { DataNode, ptr };
		return ptr;
	}

	template<class ...Args> void Set(const std::string& s, Args ...args)
	{
		auto ptr = std::make_shared<eaPropertyValue>(args...);
		data[s] = { DataPropertyValue, ptr };
	}
	
	std::map<std::string, std::shared_ptr<void>> GetData() const
	{
		std::map<std::string, std::shared_ptr<void>> dataMap;

		for (auto& pair : data)
			dataMap[pair.first] = pair.second.second;
		return dataMap;
	}

	void WriteToStream(std::ostream& stream);
	void ReadFromStream(std::istream& stream);
};
