#include <SDL_ttf.h>
#include "eaFont.h"

using namespace std;

eaFont::eaFont(const string& fontType)
{
	string fontSizeStr;
	bool hasReadToArg = false;
	for (auto c : fontType)
	{
		if (c == ':' && !hasReadToArg)
		{
			hasReadToArg = true;
			continue;
		}
		if (hasReadToArg)
			fontSizeStr += c;
		else
			fontName += c;
	}
	fontSize = stoi(fontSizeStr);
	font = TTF_OpenFont(("font/" + fontName).c_str(), fontSize);
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