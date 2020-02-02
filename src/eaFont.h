#pragma once
#include <string>

typedef struct _TTF_Font TTF_Font;

class eaFont
{
	TTF_Font* font;
	int fontSize;

public:
	eaFont(const std::string& fontName);
	~eaFont();

	int GetFontSize()
	{
		return fontSize;
	}

	operator TTF_Font* () const
	{
		return font;
	}
};