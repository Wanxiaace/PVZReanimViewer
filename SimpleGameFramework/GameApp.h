#pragma once
#ifndef __SIMPLE_GAME_APP__
#define __SIMPLE_GAME_APP__

#include "SimpleApp.h"
#include "Graphics.h"
#include "WidgetSystem.h"
#include "MusicSystem.h"
#include "ResourceManager.h"
#include "Dictionary.h"
#include "CursorObject.h"
#include <iostream>

namespace sgf {
	class Graphics;
	class WidgetManager;
	class CursorObject;
	class Widget;

	class GameApp : public GameAppBase {
	public:
		Graphics* mGraphics = nullptr;
		WidgetManager* mWidgetManager = nullptr;
		bool mDragAllowed = false;
		bool mRotateAllowed = false;
		bool mShowWidgetHitBoxAllowed = false;
		
		float mMovingSpeed = 1.0f;
		float mRenderScale = 1.0f;
		float mRunningSpeed = 1.0f;

		ResourceList mResourceList;
		ResourceManager mResourceManager;
		MusicManager mMusicManager;
		Dictionary mDictionary;

		float mMusicVolume = 100.0f;
		float mSoundVolume = 100.0f;
		bool mUseFullScreen = false;

	public:
		GameApp(int width, int height, const sgf::String& windowCaptain, bool enabledASync = true,bool resiziable = false);
		~GameApp();

		void SafeDeleteWidget(Widget* target);
		void SafeAppendWidget(Widget* target);
		void LoadResources(const char* resourcesListPath);
		/// 提供一些多语言支持
		void LoadDict(const char* dictPath);
		void SetWindowIcon(const char* path);
		void SetWindowIconFromImage(sgf::SimpleImage* image);
		void SetWindowCaptain(const sgf::String& title);
		void ScreenShot(const sgf::String& outPath);
		void UseFullScreen();
		void ExitFullScreen();
		void DoInGraphicsThread(std::function<void()>* func);
		void LockUpdate();
		void UnLockUpdate();
		void ShowHandCursor();

		void UpdateMusicVolume();
		void UpdateSoundVolume();
		Graphics* LoadGraphics();
		float GetMouseWheelY();

	public:
		virtual void DrawImgui();
		virtual void Update() override;
		virtual void Draw() override;
		virtual void CopeEvent(SDL_Event& theEvent) override;
	};

	extern sgf::GameApp* gGameApp;

}


#endif