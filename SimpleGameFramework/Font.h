#pragma once
#ifndef __SIMPLE_FONT__
#define __SIMPLE_FONT__


#include "Common.h"
#include "Graphics.h"

namespace sgf {
	enum TEXT_ALIGN {
		TEXT_LEFT,
		TEXT_CENTER,
		TEXT_RIGHT,
	};

	class Font {
	public:
		int mSize = 0;
		TTF_Font* mFont = nullptr;

	public:
		Font();
		~Font();

		void LoadFromFile(const char* path);
		void SetFontSize(int fontSize);
		SimpleImage* GenTextImage(const String& text) const;
	};
}

#endif