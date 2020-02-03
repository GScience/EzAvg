#pragma once
#include <string>
#include "eaFont.h"
#include "eaSprite.h"

struct SDL_Surface;

enum eaTextLayout
{
	TextLayoutLeft, TextLayoutCenter, TextLayoutRight
};

class eaSpriteText : public eaSprite
{
	std::string text;
	std::shared_ptr<eaFont> font;

	int cursorX = 0;
	int cursorY = 0;

	eaTextLayout textLayout = TextLayoutLeft;

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

	void OnResize() override;

	/*
	»æÖÆ
	*/
	void Draw(SDL_Renderer* renderer) override;

	void Save() override;
	void Load() override;
};