#pragma once
#include <map>
#include <memory>
#include <string>

/*
保存节点
*/
class eaProfileNode
{
	std::map<std::string, std::shared_ptr<void>> data;

public:
	template<class T> std::shared_ptr<T> Get(std::string s)
	{
		auto result = data.find(s);
		if (result == data.end())
			return nullptr;
		return std::reinterpret_pointer_cast<T>(result->second);
	}

	template<class T, class ...Args> std::shared_ptr<T> Set(std::string s, Args ...args)
	{
		auto ptr = std::make_shared<T>(args...);
		data[s] = ptr;
		return ptr;
	}
	
	const std::map<std::string, std::shared_ptr<void>>& GetData() const
	{
		return data;
	}
};
