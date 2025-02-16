#pragma once
#ifndef __SIMPLE_APP__
#define __SIMPLE_APP__
#include "Common.h"
#include "TickCounter.h"
#include "GameMessage.h"
#include <mutex>
#include <thread>


namespace sgf {
	extern std::mutex gLoopMutex;
	extern std::thread* gUpdateThread;

	class GameAppBase {
	public:
		std::function<void(GameAppBase*, int)> mDisplay;
		std::function<void(GameAppBase*, SDL_Event&)> mCallback;

	public:
		int mWidth = 0;//不会随窗口缩放改变的固定值
		int mHeight = 0;//不会随窗口缩放改变的固定值
		bool mIsOpen = false;
		bool mShowDebugWindow = false;
		bool mShowAnalyzeWindow = false;
		bool mMultiThreadUpdate = false;
		int mMouseX = 0;
		int mMouseY = 0;
		float mMouseXScale = 1.0f;
		float mMouseYScale = 1.0f;
		float mMouseWheelY = 0.0f;
		TickCounter mTick;

		unsigned int mDeltaFrameTime = 0;

		unsigned int mLastSecondBuffer = 0;
		unsigned int mFramePerSecondBuffer = 0;
		unsigned int mFPS = 0;
		
		bool mIsMouseLeftDown = false;
		bool mIsMouseMiddleDown = false;
		bool mIsMouseRightDown = false;

		int mTextureNumberMax = 0;
		SDL_Window* mGameWindow;
		GameMessageManager mMessageManager;
		SDL_GLContext mGLContext;
		MouseStyle mMouseStyleEach = MOUSE_STYLE_ARROW;
		glm::mat4x4 mViewMatrix;
		glm::mat4x4 mViewMatrixBuf;
		bool mEnabledASync = false;
		std::ostream* mLog = nullptr;
		int mFPSMax = 100;

	public:
		GameAppBase(int width, int height, const sgf::String& windowCaptain,bool enabledASync=true, bool resiziable = false);
		virtual ~GameAppBase();

		
		void SetDisplayFunction(std::function<void(GameAppBase*, int)>& display);
		void SetCallBackFunction(std::function<void(GameAppBase*, SDL_Event&)>& callback);
		void EnableMultThreadUpdate(std::function<void()> updatethread);
		
		std::ostream& Log() { return *mLog; };

	public:
		virtual void CopeEvent(SDL_Event& theEvent);
		virtual void EnterMainLoop();

		virtual void Update();
		virtual void Draw();
	};
}

#endif // !__SIMPLE_APP__
