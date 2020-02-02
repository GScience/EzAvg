#pragma once
#include <string>

typedef struct _TTF_Font TTF_Font;

struct eaStringSize
{
	int width = 0;
	int height = 0;
};

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

	int GetLineHeight();
	eaStringSize GetStringSize(const std::string str);

	operator TTF_Font* () const
	{
		return font;
	}
};