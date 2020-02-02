#pragma once

#include <string>

struct SDL_Texture;
struct SDL_Surface;

class eaTexture
{
	SDL_Texture* texture;

	int height;
	int width;

public:
	const std::string name;

	eaTexture(std::string name);
	~eaTexture();

	operator SDL_Texture* ()
	{
		return texture;
	}

	int GetWidth();
	int GetHeight();
};