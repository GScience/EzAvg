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
	propertyBinder["layout"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			switch (textLayout)
			{
			case TextLayoutLeft:
				return "left";
			case TextLayoutCenter:
				return "center";
			case TextLayoutRight:
				return "right";
			default:
				return nullptr;
			}
		},
		[&](eaPropertyValue value)
		{
			if (value.ToString() == "left")
				textLayout = TextLayoutLeft;
			else if (value.ToString() == "center")
				textLayout = TextLayoutCenter;
			else if (value.ToString() == "right")
				textLayout = TextLayoutRight;

			Redraw();
		}
	);
}

void eaSpriteText::Clear()
{
	auto renderRect = GetRenderRect();

	if (renderRect.width <= 0 || renderRect.height <= 0)
		return;

	cursorX = 0;
	cursorY = 0;

	text = "";

	if (textSurface != nullptr && 
		textSurface->w == renderRect.width &&
		textSurface->h == renderRect.height)
	{
		SDL_FillRect(textSurface, nullptr, SDL_MapRGBA(textSurface->format, 0, 0, 0, 0));
	}
	else
	{
		if (textSurface != nullptr)
			SDL_FreeSurface(textSurface);

		textSurface = SDL_CreateRGBSurface(0, renderRect.width, renderRect.height, 32, 0xff, 0xff00, 0xff0000, 0xff000000);
		SDL_SetSurfaceBlendMode(textSurface, SDL_BLENDMODE_NONE);
	}

	if (textTexture != nullptr)
	{
		SDL_DestroyTexture(textTexture);
		textTexture = nullptr;
	}
}

void eaSpriteText::OnResize()
{
	Redraw();
}
void eaSpriteText::Redraw()
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
	if (textSurface == nullptr)
		return;

	auto renderRect = GetRenderRect();

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
		if (cursorX + wordSize.width > renderRect.width)
		{
			cursorX = 0;
			cursorY += font->GetLineHeight();
		}

		SDL_Color color = { 255,0,0 };
		auto charSurface = TTF_RenderUTF8_Blended(*font, c.c_str(), color);
		SDL_SetSurfaceBlendMode(charSurface, SDL_BLENDMODE_NONE);

		switch (textLayout)
		{
			case TextLayoutLeft:
			{
				SDL_Rect rect = SDL_Rect{ cursorX , cursorY, wordSize.width , wordSize.height };

				SDL_BlitSurface(charSurface, nullptr, textSurface, &rect);
				break;
			}
			case TextLayoutCenter:
			{
				// 当前一行左移
				SDL_Rect currentLineRect = SDL_Rect{ 0 , cursorY, renderRect.width , font->GetLineHeight() };
				SDL_Rect updatedLineRect = SDL_Rect{ -wordSize.width / 2, cursorY, renderRect.width , font->GetLineHeight() };

				SDL_BlitSurface(textSurface, &currentLineRect, textSurface, &updatedLineRect);

				// 渲染文字
				SDL_Rect rect = SDL_Rect
				{ 
					renderRect.width / 2 + cursorX / 2 - wordSize.width / 2,
					cursorY, 
					wordSize.width , 
					wordSize.height 
				};
				SDL_BlitSurface(charSurface, nullptr, textSurface, &rect);
				break;
			}
			case TextLayoutRight:
			{
				// 当前一行左移
				SDL_Rect currentLineRect = SDL_Rect{ 0 , cursorY, renderRect.width , font->GetLineHeight() };
				SDL_Rect updatedLineRect = SDL_Rect{ -wordSize.width, cursorY, renderRect.width , font->GetLineHeight() };

				SDL_BlitSurface(textSurface, &currentLineRect, textSurface, &updatedLineRect);

				// 渲染文字
				SDL_Rect rect = SDL_Rect
				{
					renderRect.width - wordSize.width,
					cursorY,
					wordSize.width ,
					wordSize.height
				};
				SDL_BlitSurface(charSurface, nullptr, textSurface, &rect);
				break;
			}
		}
		SDL_FreeSurface(charSurface);
		cursorX += wordSize.width;
	}

	if (textTexture != nullptr)
		SDL_DestroyTexture(textTexture);

	textTexture = SDL_CreateTextureFromSurface(eaApplication::GetRenderer(), textSurface);
	SDL_SetTextureBlendMode(textTexture, SDL_BLENDMODE_BLEND);
	text = str;
}

void eaSpriteText::Draw(SDL_Renderer* renderer)
{
	if (textTexture != nullptr)
	{
		auto renderRect = GetRenderRect();
		auto rect = SDL_Rect{ renderRect.x,renderRect.y,renderRect.width,renderRect.height };
		SDL_SetTextureAlphaMod(textTexture, (uint8_t)(alpha * 255));
		SDL_RenderCopy(renderer, textTexture, nullptr, &rect);
	}
}

void eaSpriteText::Save()
{

}

void eaSpriteText::Load()
{

}