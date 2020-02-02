#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include "eaSpriteText.h"
#include "eaResources.h"
#include "eaApplication.h"

using namespace std;

eaSpriteText::eaSpriteText()
{
	SetFont("Font.ttf", 20);

	propertyBinder["text"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			return GetText();
		},
		[&](eaPropertyValue value)
		{
			SetText(value.ToString());
		}
	);
}

void eaSpriteText::Clear()
{
	cursorX = 0;
	cursorY = 0;

	text = "";

	if (textSurface != nullptr && 
		textSurface->w != size.width && 
		textSurface->h != size.height)
	{
		SDL_FillRect(textSurface, nullptr, SDL_MapRGBA(textSurface->format, 0, 0, 0, 0));
	}
	else
	{
		if (textSurface != nullptr)
			SDL_FreeSurface(textSurface);

		textSurface = SDL_CreateRGBSurface(0, size.width, size.height, 32, 0xff, 0xff00, 0xff0000, 0xff000000);
	}

	if (textTexture != nullptr)
	{
		SDL_DestroyTexture(textTexture);
		textTexture = nullptr;
	}
}

void eaSpriteText::OnResize()
{
	auto oldText = text;
	Clear();
	if (oldText != "")
		SetText(oldText);
}

eaSpriteText::~eaSpriteText()
{
	SDL_FreeSurface(textSurface);
}

void eaSpriteText::SetFont(string fontName, int fontSize)
{
	auto fullFontName = fontName + ":" + to_string(fontSize);

	font = eaResources::Load<eaFont>(fullFontName);
	if (text != "")
		SetText(text);
}

void eaSpriteText::SetText(std::string str)
{
	// 判断是否只是增加字符
	for (size_t i = 0; i < text.size(); ++i)
	{
		if (str[i] == text[i])
			continue;
		Clear();
		break;
	}

	// 一个一个的画
	for (size_t i = text.size(); i < str.size(); ++i)
	{
		unsigned char cSignBit = ~str[i];
		string c;

		// 一位
		if (cSignBit >> 7 == 1)
			c += str[i];
		else
		{
			// 中间值
			if (cSignBit >> 6 == 1)
				continue;

			// 多位
			size_t count = 1;
			while (cSignBit >> (7 - count) == 0)
				++count;

			for (auto j = 0; j < count; ++j)
				c += str[i + j];

			// 移动到下一位
			i += count - 1;
		}

		auto wordSize = font->GetStringSize(c);

		// 下一行
		if (cursorX + wordSize.width > size.width)
		{
			cursorX = 0;
			cursorY += font->GetLineHeight();
		}

		SDL_Rect rect = SDL_Rect{ cursorX , cursorY, wordSize.width , wordSize.height };

		SDL_Color color = { 255,0,0 };
		auto charSurface = TTF_RenderUTF8_Blended(*font, c.c_str(), color);
		SDL_BlitSurface(charSurface, nullptr, textSurface, &rect);
		SDL_FreeSurface(charSurface);

		cursorX += wordSize.width;
	}

	if (textTexture != nullptr)
		SDL_DestroyTexture(textTexture);

	textTexture = SDL_CreateTextureFromSurface(eaApplication::GetRenderer(), textSurface);
	text = str;
}

void eaSpriteText::Draw(SDL_Renderer* renderer)
{
	if (textTexture != nullptr)
	{
		auto rect = SDL_Rect{ pos.x,pos.y,size.width,size.height };
		SDL_RenderCopy(renderer, textTexture, nullptr, &rect);
	}
}

void eaSpriteText::Save()
{

}

void eaSpriteText::Load()
{

}