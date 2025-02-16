#pragma once
#ifndef __SIMPLE_CURSOR_OBJECT__
#define __SIMPLE_CURSOR_OBJECT__

#include "Common.h"
#include "Graphics.h"

namespace sgf {
	class Graphics;
	class GameApp;
	class CursorObject {
	public:
		bool mVisible = true;
		SDL_Cursor* mCursor = nullptr;
	public:
		CursorObject();
		~CursorObject();

		void SetVisible(bool visible);

	public:
		virtual void Update();
		virtual void Draw(Graphics* g);
	};
}

#endif // !__SIMPLE_CURSOR_OBJECT__