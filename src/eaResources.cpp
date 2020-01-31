#include <SDL.h>
#include <iostream>
#include "eaTexture.h"
#include <SDL_image.h>
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

	for (auto c : rawName)
	{
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
		default:
			name += c;
		}
	}

	auto findResult = resDict.find(name);
	if (findResult != resDict.end())
		return findResult->second;
	
	if (type == typeid(eaTexture).hash_code())
	{
		auto surface = IMG_Load(("image/" + name).c_str());
		auto texture = make_shared<eaTexture>(surface);
		SDL_FreeSurface(surface);
		resDict[name] = texture;
		return texture;
	}

	if (type == typeid(eaScript).hash_code())
	{
		auto result = eaScript::FromFile("scene/" + name);
		auto error = eaScript::GetError();
		if (error != "")
			cout << error << endl;
		resDict[name] = result;
		return result;
	}

	return nullptr;
}