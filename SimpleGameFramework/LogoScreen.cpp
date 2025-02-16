#include "LogoScreen.h"
#include <thread>
#include <chrono>

using namespace std::chrono;

sgf::LogoScreen::LogoScreen(GameApp* app):
	Widget(0x5500)
{
	mApp = app;
	mShowCounterMax = 2500;
	mShowCounter = mShowCounterMax;
}

void sgf::LogoScreen::AppendLogoImage(SimpleImage* logo,float scale)
{
	mScaleF = scale;
	mLogoImage = logo;
}

void sgf::LogoScreen::Update()
{
	mScaleF += 0.05f * float(mTick.GetDeltaTick()) / mShowCounterMax;

	if (mShowCounter > mTick.GetDeltaTick())
		mShowCounter -= mTick.GetDeltaTick();
	else
	{
		if (!mIsEnterNextScreen)
		{
			mNextFunc();
			mShowCounter = 0;
			mApp->SafeDeleteWidget(this);
			mIsEnterNextScreen = true;
		}
	}
}

void sgf::LogoScreen::Draw(Graphics* g)
{
	float offsetX = (mApp->mWidth - mLogoImage->GetWidth() * mScaleF) / 2.0f;
	float offsetY = (mApp->mHeight - mLogoImage->GetHeight() * mScaleF) / 2.0f;

	g->MoveTo(offsetX, offsetY);
	g->SetCubeColor({1,1,1,float(mShowCounter) / mShowCounterMax });
	if (mLogoImage) {
		g->DrawImageScaleF(mLogoImage, mScaleF, mScaleF);
	}
	
}

void sgf::LogoScreen::Join()
{
	WaitUntil(
		[this]()->bool {
			return mIsEnterNextScreen;
		}
	);
}
