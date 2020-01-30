#pragma once
#include <string>
#include <memory>

class eaResources
{
	static std::shared_ptr<void> Load(std::string name, size_t type);
public:
	eaResources() = delete;

	template<class T> static std::shared_ptr<T> Load(std::string name)
	{
		return std::reinterpret_pointer_cast<T>(Load(name, typeid(T).hash_code()));
	}
};