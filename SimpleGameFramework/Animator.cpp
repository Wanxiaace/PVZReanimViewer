#include "Animator.h"

sgf::Animator::Animator()
{
	mReanim = nullptr;
}

sgf::Animator::Animator(sgf::Reanimation* reanim)
{
	Init(reanim);
}

void sgf::Animator::Play(PlayState state)
{
	mPlayingState = state;
	mIsPlaying = true;
}

void sgf::Animator::Init(Reanimation* reanim)
{
	mReanim = reanim;
	mFPS = mReanim->mFPS;
	mDeltaRate = 1000.0f / mFPS;

	for (auto& x : *reanim->mTracks)
	{
		mTrackIndicesMap[x.mTrackName] = mExtraInfos.size();
		mExtraInfos.push_back({});
	}

	mFrameIndexEnd = reanim->mTracks->at(0).mFrames.size() - 1;
}

void sgf::Animator::Pause()
{
	mIsPlaying = false;
}

std::pair<int, int> sgf::Animator::GetTrackRange(const sgf::String& trackName)
{
	TrackInfo* aTrack = nullptr;
	for (auto& x : *mReanim->mTracks) {
		if (x.mTrackName == trackName) {
			aTrack = &x;
			break;
		}
	}
	if (!aTrack)
		return { 0,0 };

	int rStart = -1;
	int rEnd = -1;
	for (size_t i = 0; i < aTrack->mFrames.size(); i++) {
		TrackFrameTransform& x = aTrack->mFrames[i];
		if (rStart == -1) {
			if (x.f == 0)
				rStart = i;
		}
		else {
			if (x.f == -1) {
				rEnd = i;
				break;
			}
		}
	}
	if (rEnd == -1) {
		rEnd = aTrack->mFrames.size();
	}
	return { rStart,rEnd - 1 };
}

void sgf::Animator::SetFrameRange(int frameBegin, int frameEnd)
{
	mFrameIndexBegin = frameBegin;
	mFrameIndexEnd = frameEnd;
}

void sgf::Animator::SetFrameRangeByTrackName(const sgf::String& trackName)
{
	auto range = GetTrackRange(trackName);
	SetFrameRange(range.first, range.second);
	mFrameIndexNow = range.first;
}

void sgf::Animator::SetFrameRangeToDefault()
{
	mFrameIndexBegin = 0;
	mFrameIndexEnd = mReanim->mTracks->at(0).mFrames.size() - 1;
}

void sgf::Animator::SetFrameRangeByTrackNameOnce(const sgf::String& trackName, const sgf::String& oriTrackName)
{
	SetFrameRangeByTrackName(trackName);
	mPlayingState = PLAY_ONCE_TO;
	mTargetTrack = oriTrackName;
}

void sgf::Animator::SetTrackVisibleByTrackName(const sgf::String& trackName, bool visible)
{
	for (auto& x : GetAllTracksExtraByName(trackName))
	{
		x->mVisible = visible;
	}
	//mExtraInfos[mTrackIndicesMap[trackName]].mVisible = visible;
}

void sgf::Animator::SetTrackVisible(int index, bool visible)
{
	mExtraInfos[index].mVisible = visible;
}

void sgf::Animator::TrackAttachImageByTrackName(const sgf::String& trackName, SimpleImage* target)
{
	for (auto& x : GetAllTracksExtraByName(trackName))
	{
		x->mAttachedImage = target;
	}
	//mExtraInfos[mTrackIndicesMap[trackName]].mAttachedImage = target;
}

void sgf::Animator::TrackAttachImage(int index, SimpleImage* target)
{
	mExtraInfos[index].mAttachedImage = target;
}

void sgf::Animator::TrackAttachAnimator(const sgf::String& trackName, Animator* target)
{
	for (auto& x : GetAllTracksExtraByName(trackName))
	{
		x->mAttachedReanim = target;
	}
	//mExtraInfos[mTrackIndicesMap[trackName]].mAttachedReanim = target;
}

void sgf::Animator::TrackAttachAnimatorMatrix(const sgf::String& trackName, glm::mat4x4* target)
{
	for (auto& x : GetAllTracksExtraByName(trackName))
	{
		x->mAttachedReanimMatrix = target;
	}
	//mExtraInfos[mTrackIndicesMap[trackName]].mAttachedReanimMatrix = target;
}

void sgf::Animator::TrackAttachOffsetByTrackName(const sgf::String& trackName, float offsetX, float offsetY)
{
	for (auto& x : GetAllTracksExtraByName(trackName)) {
		x->mOffsetX = offsetX;
		x->mOffsetY = offsetY;
	}
}

void sgf::Animator::TrackAttachOffset(int index, float offsetX, float offsetY)
{
	mExtraInfos[index].mOffsetX = offsetX;
	mExtraInfos[index].mOffsetY = offsetY;
}

void sgf::Animator::TrackAttachFlashSpot(int index, float spot)
{
	mExtraInfos[index].mFlashSpotSingle = spot;
}

void sgf::Animator::TrackAttachFlashSpotByTrackName(const sgf::String& trackName, float spot)
{
	for (auto& x : GetAllTracksExtraByName(trackName))
	{
		x->mFlashSpotSingle = spot;
	}
}

float sgf::Animator::GetTrackVelocity(const sgf::String& trackName)
{
	TrackInfo* aTrack = nullptr;
	aTrack = &mReanim->mTracks->at(mTrackIndicesMap[trackName]);
	if (int(mFrameIndexNow) == int(mFrameIndexBegin))
		return aTrack->mFrames[mFrameIndexNow + 1].x - aTrack->mFrames[mFrameIndexNow].x;
	return aTrack->mFrames[mFrameIndexNow + 1].x - aTrack->mFrames[mFrameIndexNow - 1].x;
}

bool sgf::Animator::GetTrackVisible(const sgf::String& trackName)
{
	int pos = mTrackIndicesMap[trackName];
	if (mExtraInfos[pos].mVisible)
		return ! mReanim->mTracks->at(pos).mFrames[mFrameIndexNow].f;
	else 
		return false;
}

std::vector<sgf::TrackExtraInfo*> sgf::Animator::GetAllTracksExtraByName(const sgf::String& trackName)
{
	auto result = std::vector<sgf::TrackExtraInfo*>();
	for (size_t i = 0; i < mReanim->mTracks->size(); i++)
	{
		if (mReanim->mTracks->at(i).mTrackName == trackName)
			result.push_back(&mExtraInfos[i]);
	}
	return result;
}

std::vector<sgf::TrackInfo*> sgf::Animator::GetAllTracksByName(const sgf::String& trackName)
{
	auto result = std::vector<sgf::TrackInfo*>();
	for (size_t i = 0; i < mReanim->mTracks->size(); i++)
	{
		if (mReanim->mTracks->at(i).mTrackName == trackName)
			result.push_back(&mReanim->mTracks->at(i));
	}
	return result;
}

int sgf::Animator::GetFirstTrackExtraIndexByName(const sgf::String& trackName)
{
	for (size_t i = 0; i < mReanim->mTracks->size(); i++)
	{
		if (mReanim->mTracks->at(i).mTrackName == trackName)
			return i;
	}
	return -1;
}

void sgf::Animator::Update()
{
	if (mIsPlaying) {
		unsigned int delta = gGameApp->mTick.GetDeltaTick();
		float num = ((float)delta) / mDeltaRate;

		if (mReanimBlendCounter > 0) {
			mReanimBlendCounter -= delta;
		}
		else {
			float final = (mFrameIndexNow + num * mSpeed);
			if (final >= mFrameIndexEnd) {
				switch (mPlayingState)
				{
				case PLAY_REPEAT:
					mFrameIndexNow = mFrameIndexBegin;
					break;
				case PLAY_ONCE_TO:
					SetFrameRangeByTrackName(mTargetTrack);
					mPlayingState = PLAY_REPEAT;
					break;
				case PLAY_ONCE:
					Pause();
					mFrameIndexNow = mFrameIndexEnd;
					break;
				}
			}
			else {
				mFrameIndexNow = final;
			}
		}
	}
}

static void GetDeltaTransformEx(const sgf::TrackFrameTransform& tSrc, const sgf::TrackFrameTransform& tDst, float tDelta, sgf::TrackFrameTransform& tOutput,bool useDestFrame = false) {
	tOutput.a = (tDst.a - tSrc.a) * tDelta + tSrc.a;
	tOutput.x = (tDst.x - tSrc.x) * tDelta + tSrc.x;
	tOutput.y = (tDst.y - tSrc.y) * tDelta + tSrc.y;
	tOutput.sx = (tDst.sx - tSrc.sx) * tDelta + tSrc.sx;
	tOutput.sy = (tDst.sy - tSrc.sy) * tDelta + tSrc.sy;
	tOutput.kx = (tDst.kx - tSrc.kx) * tDelta + tSrc.kx;
	tOutput.ky = (tDst.ky - tSrc.ky) * tDelta + tSrc.ky;
	if(useDestFrame)
		tOutput.f = tDst.f;
	else
		tOutput.f = tSrc.f;
	tOutput.i = tSrc.i;

	if (tDst.kx > tSrc.kx + 180.0f)
		tOutput.kx = tSrc.kx;
	if (tDst.kx < tSrc.kx - 180.0f)
		tOutput.kx = tSrc.kx;
	if (tDst.ky > tSrc.ky + 180.0f)
		tOutput.ky = tSrc.ky;
	if (tDst.ky < tSrc.ky - 180.0f)
		tOutput.ky = tSrc.ky;
}

void sgf::Animator::Present(Graphics* g)
{
	PresentMatrix(g, glm::mat4x4(1.0f));
}

void sgf::Animator::PresentMatrix(Graphics* g,const glm::mat4x4& mat)
{
	Reanimation* targetReanim = mReanim;
	float OffsetX = 0;
	float OffsetY = 0;
	float fScale = 1.0f;

	Color colorTotalTemp = g->mCubeColor;

	for (size_t i = 0; i < targetReanim->mTracks->size(); i++) {
		g->SetCubeColor(colorTotalTemp);
		auto& x = (targetReanim->mTracks)->at(i);
		if (!x.mAvailable)
			continue;
		if (!mExtraInfos[i].mVisible)
			continue;

		float transformDelta = mFrameIndexNow - int(mFrameIndexNow);
		TrackFrameTransform aSource = x.mFrames[mFrameIndexNow];
		float rate = float(mFrameIndexNow - mFrameIndexBegin) / float(mFrameIndexEnd - mFrameIndexBegin);

		OffsetX = mExtraInfos[i].mOffsetX;
		OffsetY = mExtraInfos[i].mOffsetY;

		Color colorTemp = { g->mCubeColor.r ,g->mCubeColor.g ,g->mCubeColor.b ,g->mCubeColor.a + mExtraInfos[i].mFlashSpotSingle };
		//g->SetCubeColor(colorTotalTemp);

		if (mReanimBlendCounter > 0) {
			GetDeltaTransformEx(x.mFrames[mFrameIndexBlendBuffer], x.mFrames[mFrameIndexBegin], 1 - mReanimBlendCounter / mReanimBlendCounterMax, aSource, true);
		}
		else {
			if (int(mFrameIndexNow) > mFrameIndexBegin)//线性插值
				GetDeltaTransformEx(x.mFrames[mFrameIndexNow-1], x.mFrames[mFrameIndexNow], transformDelta, aSource);
		}

		if (!aSource.f) {
			if (!mExtraInfos[i].mAttachedReanim) {
				SimpleImage* targetImage;
				if (mExtraInfos[i].mAttachedImage)
					targetImage = mExtraInfos[i].mAttachedImage;
				else
				{
					if (aSource.i == "")
						continue;
					targetImage = targetReanim->mResourceManager->GetResourceWithWarning<SimpleImage>(aSource.i);
				}

				glm::mat4x4 animMatrix = glm::mat4x4(1.0f);
				Point graPos = g->GetGraphicsTransformPosition();
				TransformToMatrixEx(aSource, animMatrix, fScale, fScale, OffsetX, OffsetX);

				animMatrix = mat * animMatrix;
				animMatrix = glm::translate(animMatrix, glm::vec3(0.0f, 0.0f, -1.0f));


				//归位颜色
				g->SetCubeColor({ colorTemp.r ,colorTemp.g ,colorTemp.b ,
					colorTemp.a * aSource.a });

				if (targetImage) {
					g->DrawImageMatrix(targetImage, animMatrix);
				}
			}
			else {
				glm::mat4x4 animMatrix = glm::mat4x4(1.0f);
				TransformToMatrixEx(aSource, animMatrix, fScale, fScale, OffsetX, OffsetX);
				if (mExtraInfos[i].mAttachedReanimMatrix)
					animMatrix *= (*mExtraInfos[i].mAttachedReanimMatrix);
				mExtraInfos[i].mAttachedReanim->PresentMatrix(g, animMatrix);
			}
		}
		else {
			if (mExtraInfos[i].mAttachedReanim) {
				glm::mat4x4 animMatrix = glm::mat4x4(1.0f);
				TransformToMatrixEx(aSource, animMatrix, fScale, fScale, OffsetX, OffsetX);
				if (mExtraInfos[i].mAttachedReanimMatrix)
					animMatrix *= (*mExtraInfos[i].mAttachedReanimMatrix);
				mExtraInfos[i].mAttachedReanim->PresentMatrix(g, animMatrix);
			}
		}
	}
}

sgf::Point sgf::Animator::GetTrackPos(const sgf::String& trackname)
{
	for (auto& x : GetAllTracksByName(trackname))
	{
		if(x->mFrames[mFrameIndexNow].f == 0)
			return sgf::Point{
		x->mFrames[mFrameIndexNow].x,
		x->mFrames[mFrameIndexNow].y
		};
	};
	return {0,0};
}

float sgf::Animator::GetTrackRotate(const sgf::String& trackname)
{
	return mReanim->mTracks->at(mTrackIndicesMap[trackname]).mFrames[mFrameIndexNow].kx;
}
