#pragma once
#include <string>
#include "eaFont.h"
#include "eaSprite.h"

struct SDL_Surface;
struct SDL_Texture;

enum eaTextVerticalLayout
{
	TextLayoutVerticalUp, TextLayoutVerticalCenter, TextLayoutVerticalDown
};

enum eaTextHorizontalLayout
{
	TextLayoutHorizontalLeft, TextLayoutHorizontalCenter, TextLayoutHorizontalRight
};

struct eaTextColor
{
	double r = 0.2;
	double g = 0.2;
	double b = 0.2;
	double a = 1;
};

class eaSpriteText : public eaSprite
{
	std::string text;
	std::shared_ptr<eaFont> font;
	
	int cursorX = 0;
	int cursorY = 0;

	int shadowOffset = 1;

	eaTextHorizontalLayout textHorizontalLayout = TextLayoutHorizontalLeft;
	eaTextVerticalLayout textVerticalLayout = TextLayoutVerticalUp;
	eaTextColor fontColor = eaTextColor{ 0.2,0.2,0.2,1 };
	eaTextColor shadowColor = eaTextColor{ 0.1,0.1,0.1,0.5 };

	SDL_Surface* lineSurface = nullptr;
	SDL_Surface* textSurface = nullptr;
	SDL_Texture* textTexture = nullptr;

	void Clear();

	void Redraw();

public:
	eaSpriteText();
	~eaSpriteText();

	std::string GetText()
	{
		return text;
	}

	void SetText(std::string str);
	void SetFont(std::string fontName, int fontSize);

	void OnLayoutChanged() override;

	/*
	绘制
	*/
	void Draw(SDL_Renderer* renderer, double groupAlpha) override;

	std::string GetType() override
	{
		return "text";
	}
};