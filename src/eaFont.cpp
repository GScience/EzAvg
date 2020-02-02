#include <SDL_ttf.h>
#include "eaFont.h"

using namespace std;

eaFont::eaFont(const string& fontName)
{
	string fileName;
	string fontSizeStr;
	bool hasReadToArg = false;
	for (auto c : fontName)
	{
		if (c == ':' && !hasReadToArg)
		{
			hasReadToArg = true;
			continue;
		}
		if (hasReadToArg)
			fontSizeStr += c;
		else
			fileName += c;
	}
	fontSize = stoi(fontSizeStr);
	font = TTF_OpenFont(("font/" + fileName).c_str(), fontSize);
	TTF_SetFontHinting(font, TTF_HINTING_MONO);
}

eaFont::~eaFont()
{
	TTF_CloseFont(font);
}

int eaFont::GetLineHeight()
{
	return TTF_FontLineSkip(font);
}

eaStringSize eaFont::GetStringSize(const std::string str)
{
	eaStringSize size;
	TTF_SizeUTF8(font, str.c_str(), &size.width, &size.height);
	return size;
}