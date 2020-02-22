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
	propertyBinder["horizontalLayout"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			switch (textHorizontalLayout)
			{
			case TextLayoutHorizontalLeft:
				return "left";
			case TextLayoutHorizontalCenter:
				return "center";
			case TextLayoutHorizontalRight:
				return "right";
			default:
				return nullptr;

			Redraw();
			}
		},
		[&](eaPropertyValue value)
		{
			if (value.ToString() == "left")
				textHorizontalLayout = TextLayoutHorizontalLeft;
			else if (value.ToString() == "center")
				textHorizontalLayout = TextLayoutHorizontalCenter;
			else if (value.ToString() == "right")
				textHorizontalLayout = TextLayoutHorizontalRight;

			Redraw();
		}
		);
	propertyBinder["verticalLayout"] = eaPropertyBinder(
		[&]()->eaPropertyValue
		{
			switch (textVerticalLayout)
			{
			case TextLayoutVerticalUp:
				return "up";
			case TextLayoutVerticalCenter:
				return "center";
			case TextLayoutVerticalDown:
				return "down";
			default:
				return nullptr;
			}
		},
		[&](eaPropertyValue value)
		{
			if (value.ToString() == "up")
				textVerticalLayout = TextLayoutVerticalUp;
			else if (value.ToString() == "center")
				textVerticalLayout = TextLayoutVerticalCenter;
			else if (value.ToString() == "down")
				textVerticalLayout = TextLayoutVerticalDown;

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

	// 如果都小于0则清空
	if (renderRect.width <= 0 || renderRect.height <= 0)
	{
		if (textSurface != nullptr)
		{
			SDL_FreeSurface(textSurface);
			textSurface = nullptr;
		}
		if (lineSurface != nullptr)
		{
			SDL_FreeSurface(lineSurface);
			lineSurface = nullptr;
		}
		return;
	}

	// 清空行Surface
	if (lineSurface != nullptr)
		SDL_FreeSurface(lineSurface);
	lineSurface = SDL_CreateRGBSurface(0, renderRect.width, font->GetLineHeight() + shadowOffset + 1, 32, 0xff, 0xff00, 0xff0000, 0xff000000);
	SDL_SetSurfaceBlendMode(lineSurface, SDL_BLENDMODE_NONE);
	
	// 清空文本surface
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

	// 清空纹理
	if (textTexture != nullptr)
	{
		SDL_DestroyTexture(textTexture);
		textTexture = nullptr;
	}
}

void eaSpriteText::OnLayoutChanged()
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
	if (lineSurface != nullptr)
	{
		SDL_FreeSurface(lineSurface);
		lineSurface = nullptr;
	}
	if (textSurface != nullptr)
	{
		SDL_FreeSurface(textSurface);
		textSurface = nullptr;
	}
	if (textTexture != nullptr)
	{
		SDL_DestroyTexture(textTexture);
		textTexture = nullptr;
	}
}

void eaSpriteText::SetFont(string fontName, int fontSize)
{
	auto fullFontName = fontName + ":" + to_string(fontSize);

	font = eaResources::Load<eaFont>(fullFontName);
	Redraw();
}

void eaSpriteText::SetText(std::string str)
{
	if (textSurface == nullptr)
	{
		text = str;
		return;
	}

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

			for (auto j = 0; j < count && i + j < str.length(); ++j)
				c += str[i + j];

			// 移动到下一位
			i += count - 1;
		}

		auto wordSize = font->GetStringSize(c);

		bool isNewLine = false;

		// 换行符
		if (c == "\n")
		{
			cursorX = 0;
			cursorY += font->GetLineHeight() + shadowOffset;
			isNewLine = true;
		}

		// 下一行
		if (cursorX + wordSize.width + shadowOffset > renderRect.width)
		{
			cursorX = 0;
			cursorY += font->GetLineHeight() + shadowOffset;
			isNewLine = true;
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

		if (isNewLine)
			SDL_FillRect(lineSurface, nullptr, SDL_MapRGBA(textSurface->format, 0, 0, 0, 0));

		// 计算布局
		int lineX = 0;
		switch (textHorizontalLayout)
		{
		case TextLayoutHorizontalLeft:
			lineX = 0;
			break;
		case TextLayoutHorizontalCenter:
			lineX = renderRect.width / 2 - cursorX / 2 - wordSize.width / 2;
			break;
		case TextLayoutHorizontalRight:
			lineX = renderRect.width - cursorX - wordSize.width;
			break;
		}

		int lineY = 0;
		switch (textVerticalLayout)
		{
		case TextLayoutVerticalUp:
			lineY = cursorY;
			break;
		case TextLayoutVerticalCenter:
			lineY = renderRect.height / 2 + cursorY / 2 - wordSize.height / 2;
			break;
		case TextLayoutVerticalDown:
			lineY = renderRect.height + cursorY - font->GetLineHeight();
			break;
		}

		// 调整垂直布局
		if (isNewLine)
		{
			SDL_Rect blockRect;
			switch (textVerticalLayout)
			{
			case TextLayoutVerticalUp:
				break;
			case TextLayoutVerticalCenter:
				blockRect = SDL_Rect{ 0,-font->GetLineHeight() / 2, renderRect.width, renderRect.height };
				SDL_BlitSurface(textSurface, nullptr, textSurface, &blockRect);
				break;
			case TextLayoutVerticalDown:
				blockRect = SDL_Rect{ 0,-font->GetLineHeight(), renderRect.width, renderRect.height };
				SDL_BlitSurface(textSurface, nullptr, textSurface, &blockRect);
				break;
			}
		}

		// 对特殊字符的处理
		if (c == "\t")
		{
			cursorX += wordSize.width * 4;
			continue;
		}
		if (c == "\n")
			continue;

		// 绘制文本
		auto charSurface = TTF_RenderUTF8_Blended(*font, c.c_str(), color);
		auto shadowSurface = TTF_RenderUTF8_Blended(*font, c.c_str(), shadow);
		SDL_SetSurfaceBlendMode(charSurface, SDL_BLENDMODE_BLEND);
		SDL_SetSurfaceBlendMode(shadowSurface, SDL_BLENDMODE_NONE);

		SDL_Rect textRect = SDL_Rect{ cursorX , 0, wordSize.width , wordSize.height };
		SDL_Rect shadowRect = SDL_Rect{ cursorX + shadowOffset , shadowOffset, wordSize.width ,wordSize.height };

		SDL_BlitSurface(shadowSurface, nullptr, lineSurface, &shadowRect);
		SDL_BlitSurface(charSurface, nullptr, lineSurface, &textRect);

		SDL_FreeSurface(charSurface);
		SDL_FreeSurface(shadowSurface);

		SDL_Rect lineRect = SDL_Rect{ lineX,lineY, renderRect.width, font->GetLineHeight() + shadowOffset };
		SDL_BlitSurface(lineSurface, nullptr, textSurface, &lineRect);

		cursorX += wordSize.width;
	}

	if (textTexture != nullptr)
		SDL_DestroyTexture(textTexture);

	textTexture = SDL_CreateTextureFromSurface(eaApplication::GetRenderer(), textSurface);
	SDL_SetTextureBlendMode(textTexture, SDL_BLENDMODE_BLEND);
	text = str;
}

void eaSpriteText::Draw(SDL_Renderer * renderer, double groupAlpha)
{
	if (textTexture != nullptr)
	{
		auto renderRect = GetRenderRect();
		auto margin = SDL_Rect{ renderRect.x,renderRect.y,renderRect.width,renderRect.height };
		SDL_SetTextureAlphaMod(textTexture, (uint8_t)(groupAlpha * alpha * 255));
		SDL_RenderCopy(renderer, textTexture, nullptr, &margin);
	}
}