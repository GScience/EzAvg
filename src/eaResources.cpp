#include <SDL.h>
#include <iostream>
#include "eaFont.h"
#include "eaTexture.h"
#include <algorithm>
#include <string>
#include <map>
#include "eaResources.h"
#include "eaScript.h"

using namespace std;

std::map<string, std::shared_ptr<void>> resDict;

std::shared_ptr<void> eaResources::Load(string rawName, size_t type)
{
	string name;

	bool hasReadToArg = false;

	for (auto c : rawName)
	{
		// 参数里禁止空格
		if (hasReadToArg)
		{
			if (c != ' ')
				name += c;
		}
		else
			switch (c)
			{
			case '/':
				if (name == "" || name[name.size() - 1] == '/')
					continue;
			case '\\':
				if (name == "" || name[name.size() - 1] == '/')
					continue;
				name += '/';
				break;
			case ':':
				hasReadToArg = true;
			default:
				name += c;
			}
	}

	auto findResult = resDict.find(name);
	if (findResult != resDict.end())
		return findResult->second;
	
	if (type == typeid(eaTexture).hash_code())
	{
		auto texture = make_shared<eaTexture>(name);
		resDict[name] = texture;
		return texture;
	}

	if (type == typeid(eaScript).hash_code())
	{
		auto result = eaScript::FromFile(name);
		auto error = eaScript::GetError();
		if (error != "")
			cout << error << endl;
		resDict[name] = result;
		return result;
	}

	if (type == typeid(eaFont).hash_code())
	{
		auto result = make_shared<eaFont>(name);
		resDict[name] = result;
		return result;
	}

	return nullptr;
}