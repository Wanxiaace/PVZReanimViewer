#pragma once
#ifndef __SIMPLE_REANIMATION__
#define __SIMPLE_REANIMATION__

#include "Common.h"
#include "Graphics.h"
#include "ResourceManager.h"
#include <vector>
#include <set>

namespace sgf {
	class Reanimation;

	struct TrackFrameTransform {
		float x;
		float y;
		float kx;
		float ky;
		float sx;
		float sy;
		float a;
		int f;
		sgf::String i;
	};

	struct TrackInfo {
		sgf::String mTrackName = "";
		bool mAvailable = true;
		std::vector<TrackFrameTransform> mFrames;
	};

	class Reanimation {
	public:
		float mFPS = 0.0f;
		std::shared_ptr<std::vector<TrackInfo>> mTracks = nullptr;
		std::shared_ptr<std::set<sgf::String>> mImagesSet = nullptr;
		bool mIsLoaded = false;
		sgf::ResourceManager* mResourceManager;

	public:
		Reanimation();
		~Reanimation();

		void LoadFromFile(const char* filePath);
		void Present(Graphics* g, int frameIndex);
	};

	void TransformToMatrixEx(sgf::TrackFrameTransform& src, glm::mat4x4& dest, float ScaleX, float ScaleY, float pX, float pY);

}

#endif