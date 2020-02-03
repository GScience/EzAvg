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

	std::string fontName;
	int fontSize;

public:

	const std::string& GetFontName()
	{
		return fontName;
	}

	int GetFontSize()
	{
		return fontSize;
	}

	eaFont(const std::string& fontType);
	~eaFont();

	int GetLineHeight();
	eaStringSize GetStringSize(const std::string str);

	operator TTF_Font* () const
	{
		return font;
	}
};