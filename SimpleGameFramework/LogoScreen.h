#pragma once
#ifndef __SGF_LOGO_SCREEN__
#define __SGF_LOGO_SCREEN__

#include "WidgetSystem.h"

namespace sgf {
	class LogoScreen : public Widget {
	public:
		std::function<void()> mNextFunc;
		unsigned int mShowCounter = 0;
		unsigned int mShowCounterMax = 0;
		GameApp* mApp = nullptr;
		bool mIsEnterNextScreen = false;
		SimpleImage* mLogoImage = nullptr;
		float mScaleF = 0;

	public:
		LogoScreen(GameApp* app);

		void SetNextScreenFunc(std::function<void()> func) { mNextFunc = func; };
		void AppendLogoImage(SimpleImage* logo, float scale=1.0f);
		void Join();

	public:
		virtual void Update() override;
		virtual void Draw(Graphics* g) override;
	};
}

#endif // !__SGF_LOGO_SCREEN__
