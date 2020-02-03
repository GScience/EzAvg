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
	propertyBinder["color"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			return { fontColor.r,fontColor.g,fontColor.b,fontColor.a };
		},
		[&](eaPropertyValue value)
		{
			auto table = value.ToTable();

			fontColor.r = table[1];
			fontColor.g = table[2];
			fontColor.b = table[3];
			auto alpha = table.find(4);
			if (alpha != table.end())
				fontColor.a = alpha->second;
			else
				fontColor.a = 1;
		}
	);
	propertyBinder["fontName"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			return font->GetFontName();
		},
		[&](eaPropertyValue value)
		{
			SetFont(value, font->GetFontSize());
		}
	);
	propertyBinder["fontSize"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			return font->GetFontSize();
		},
		[&](eaPropertyValue value)
		{
			SetFont(font->GetFontName(), value);
		}
	);
	propertyBinder["shadowColor"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			return { shadowColor.r,shadowColor.g,shadowColor.b,shadowColor.a };
		},
		[&](eaPropertyValue value)
		{
			auto table = value.ToTable();

			shadowColor.r = table[1];
			shadowColor.g = table[2];
			shadowColor.b = table[3];
			auto alpha = table.find(4);
			if (alpha != table.end())
				shadowColor.a = alpha->second;
			else
				fontColor.a = 1;
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

	cursorX = 0;
	cursorY = 0;

	text = "";

	if (renderRect.width <= 0 || renderRect.height <= 0)
	{
		if (textSurface != nullptr)
		{
			SDL_FreeSurface(textSurface);
			textSurface = nullptr;
		}
		return;
	}

	if (textSurface != nullptr && 
		textSurface->w == renderRect.width &&
		textSurface->h == renderRect.height)
	{
		SDL_FillRect(textSurface, nullptr, SDL_MapRGBA(textSurface->format, 0, 0, 0, 0));
	}
	else
	{
		if (textSurface != nullptr)
		{
			SDL_FreeSurface(textSurface);
			textSurface = nullptr;
		}

		textSurface = SDL_CreateRGBSurface(0, renderRect.width, renderRect.height, 32, 0xff, 0xff00, 0xff0000, 0xff000000);
		SDL_FillRect(textSurface, nullptr, SDL_MapRGBA(textSurface->format, 0, 0, 0, 0));
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

		wordSize.width += shadowOffset;
		wordSize.height += shadowOffset;

		// 下一行
		if (cursorX + wordSize.width + shadowOffset > renderRect.width)
		{
			cursorX = 0;
			cursorY += font->GetLineHeight();
		}

		SDL_Color color =
		{
			(uint8_t)(fontColor.r * 255),
			(uint8_t)(fontColor.g * 255),
			(uint8_t)(fontColor.b * 255),
			(uint8_t)(fontColor.a * 255)
		};
		SDL_Color shadow =
		{
			(uint8_t)(shadowColor.r * 255),
			(uint8_t)(shadowColor.g * 255),
			(uint8_t)(shadowColor.b * 255),
			(uint8_t)(shadowColor.a * 255)
		};
		auto charSurface = TTF_RenderUTF8_Blended(*font, c.c_str(), color);
		auto shadowSurface = TTF_RenderUTF8_Blended(*font, c.c_str(), shadow);
		SDL_SetSurfaceBlendMode(charSurface, SDL_BLENDMODE_BLEND);
		SDL_SetSurfaceBlendMode(shadowSurface, SDL_BLENDMODE_NONE);

		switch (textLayout)
		{
			case TextLayoutLeft:
			{
				SDL_Rect rect = SDL_Rect{ cursorX , cursorY, wordSize.width , wordSize.height };
				SDL_Rect shadowRect = SDL_Rect{ rect.x + shadowOffset , rect.y + shadowOffset,rect.w , rect.h };
				SDL_BlitSurface(shadowSurface, nullptr, textSurface, &shadowRect);
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

				SDL_Rect shadowRect = SDL_Rect{ rect.x + shadowOffset , rect.y + shadowOffset,rect.w , rect.h };
				SDL_Rect clearRect = SDL_Rect{ rect.x,rect.y,rect.w + shadowOffset,rect.h + shadowOffset };
				SDL_FillRect(textSurface, &clearRect, SDL_MapRGBA(textSurface->format, 0, 0, 0, 0));
				SDL_BlitSurface(shadowSurface, nullptr, textSurface, &shadowRect);
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
				SDL_Rect shadowRect = SDL_Rect{ rect.x + shadowOffset , rect.y + shadowOffset,rect.w , rect.h };
				SDL_Rect clearRect = SDL_Rect{ rect.x,rect.y,rect.w + shadowOffset,rect.h + shadowOffset };
				SDL_FillRect(textSurface, &clearRect, SDL_MapRGBA(textSurface->format, 0, 0, 0, 0));
				SDL_BlitSurface(shadowSurface, nullptr, textSurface, &shadowRect);
				SDL_BlitSurface(charSurface, nullptr, textSurface, &rect);
				break;
			}
		}
		SDL_FreeSurface(charSurface);
		SDL_FreeSurface(shadowSurface);
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