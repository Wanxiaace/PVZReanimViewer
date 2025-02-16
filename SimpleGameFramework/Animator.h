#ifndef __SIMPLE_ANIMATOR__
#define __SIMPLE_ANIMATOR__
#include "Common.h"
#include "Reanimation.h"
#include <unordered_map>

namespace sgf {
	class Animator;

	struct TrackExtraInfo {
		bool mVisible = true;
		Animator* mAttachedReanim = nullptr;
		SimpleImage* mAttachedImage = nullptr;
		glm::mat4x4* mAttachedReanimMatrix = nullptr;
		float mOffsetX = 0;
		float mOffsetY = 0;
		float mFlashSpotSingle = 0;//单独的闪光程度
	};

	class Animator
	{
	public:
		enum PlayState {
			PLAY_NONE,
			PLAY_REPEAT,
			PLAY_ONCE,
			PLAY_ONCE_TO,
		};

	public:
		Reanimation* mReanim;
		float mFPS = 0;
		float mFrameIndexNow =0;
		float mFrameIndexBegin = 0;
		float mFrameIndexEnd = 0;
		float mSpeed = 1.0f;
		float mDeltaRate = 0.0f;
		//过渡动画倒计时
		float mReanimBlendCounter = -1.0f;
		float mReanimBlendCounterMax = 100.0f;
		//过渡动画缓存
		int mFrameIndexBlendBuffer = 0;

		bool mIsPlaying = 0;
		PlayState mPlayingState = PLAY_NONE;
		std::vector<TrackExtraInfo> mExtraInfos;
		std::unordered_map<sgf::String, int> mTrackIndicesMap;
		sgf::String mTargetTrack;

	public:
		Animator();
		Animator(Reanimation* reanim);
		~Animator() {};

		void Play(PlayState state = PLAY_REPEAT);
		void Init(Reanimation* reanim);
		void Pause();

		std::pair<int, int> GetTrackRange(const sgf::String& trackName);
		void SetFrameRange(int frameBegin,int frameEnd);
		void SetFrameRangeByTrackName(const sgf::String& trackName);
		void SetFrameRangeToDefault();

		//自动恢复
		void SetFrameRangeByTrackNameOnce(const sgf::String& trackName, const sgf::String& oriTrackName);
		void SetTrackVisibleByTrackName(const sgf::String& trackName,bool visible);
		void SetTrackVisible(int index,bool visible);

		void TrackAttachImageByTrackName(const sgf::String& trackName, SimpleImage* target);
		void TrackAttachImage(int index, SimpleImage* target);

		void TrackAttachAnimator(const sgf::String& trackName, Animator* target);
		void TrackAttachAnimatorMatrix(const sgf::String& trackName, glm::mat4x4* target);

		void TrackAttachOffsetByTrackName(const sgf::String& trackName, float offsetX,float offsetY);
		void TrackAttachOffset(int index, float offsetX, float offsetY);

		void TrackAttachFlashSpot(int index, float spot);
		void TrackAttachFlashSpotByTrackName(const sgf::String& trackName, float spot);

		float GetTrackVelocity(const sgf::String& trackName);
		bool GetTrackVisible(const sgf::String& trackName);
		std::vector<sgf::TrackExtraInfo*> GetAllTracksExtraByName(const sgf::String& trackName);
		std::vector<sgf::TrackInfo*> GetAllTracksByName(const sgf::String& trackName);

		int GetFirstTrackExtraIndexByName(const sgf::String& trackName);

		void Update();
		void Present(Graphics* g);
		void PresentMatrix(Graphics* g, const glm::mat4x4& mat);
		sgf::Point GetTrackPos(const sgf::String& trackname);
		float GetTrackRotate(const sgf::String& trackname);
	};

}

#endif
