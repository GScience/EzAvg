#pragma once
#include "eaSprite.h"

class eaTexture;

struct eaSpriteColor
{
	double r = 1;
	double g = 1;
	double b = 1;
	double a = 1;
};

class eaSpriteImage : public eaSprite
{
	std::shared_ptr<eaTexture> image;

	eaSpriteColor color;

public:
	eaSpriteImage();

	/*
	绘制
	*/
	void Draw(SDL_Renderer* renderer, double groupAlpha) override;

	void Save() override;
	void Load() override;

	std::string GetType() override
	{
		return "image";
	}
};