#include "Font.h"

sgf::Font::Font()
{


}

sgf::Font::~Font()
{
	if (mFont)
		TTF_CloseFont(mFont);
}

#include "GamePacker/GamePacker.h"

void sgf::Font::LoadFromFile(const char* path)
{
	FileStream* file = FileManager::TryToLoadFilePointer(path);
	mFont = TTF_OpenFontRW(file->mIStream,1,30);
}

void sgf::Font::SetFontSize(int fontSize)
{
	if(mFont)
		TTF_SetFontSize(mFont, fontSize);
}

sgf::SimpleImage* sgf::Font::GenTextImage(const String& text) const
{
	if (mFont) {
		SDL_Surface* textSurface = TTF_RenderUTF8_Blended(mFont, text.c_str(), {255,255,255});
		SimpleImage* img = new SimpleImage();
		img->LoadFromSurface(textSurface);
		return img;
	}
	return nullptr;
}
