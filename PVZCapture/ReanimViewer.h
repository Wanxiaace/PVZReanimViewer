#ifndef __REANIM_VIEWER__
#define __REANIM_VIEWER__


#include "GameApp.h"
#include "Animator.h"
#include "Reanimation.h"
#include "ResourceManager.h"

class ViewerApp: public sgf::GameApp {
public:
	sgf::Reanimation* mReanimPtr = nullptr;
	sgf::Animator* mAnimator = nullptr;

	sgf::String mTargetPath;
	sgf::String mImagesDir;

	bool mImageShowList[100] = { 0 };
	bool mTrackShowList[100] = { 0 };

	ImVec2 mWindowSize;
	ImVec2 mWindowPos;
	float mScale = 1.0f;
	ImVec2 mDragDelta;
	ImVec2 mMapPos;
	bool mFocused;
	float mReanimSpeed = 1.0f;


public:
	ViewerApp();
	~ViewerApp();

public:
	virtual void DrawImgui() override;
	virtual void Draw() override;
	virtual void Update() override;
	virtual void CopeEvent(SDL_Event& theEvent) override;

	void DisplayBenchLayer();
	void DisplayMenuBar();
	void DrawTopLayer();

	void LoadReanim(const char* reanimPath,const char* resDirPath);

	void PresentAnimator();
	void UpdateAnimatorState();
};

extern ViewerApp* gViewerApp;

#endif // __REANIM_VIEWER__
