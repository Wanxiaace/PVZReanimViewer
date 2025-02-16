#pragma once
#ifndef __SIMPLE_PARTICLE__
#define __SIMPLE_PARTICLE__

#include "SimpleImage.h"
#include "Graphics.h"
#include "Reanimation.h"
#include "Animator.h"
#include <vector>

namespace sgf {
	enum ParticleMotionType {
		PARTICLE_MOTION_NORMAL,
		PARTICLE_MOTION_THROW,
		PARTICLE_MOTION_THROW_FAST,
	};

	enum ParticleImageType {
		PARTICLE_IMAGE_NONE,
		PARTICLE_IMAGE_SINGLE,
		PARTICLE_IMAGE_GROUP_ANIMARION,
		PARTICLE_IMAGE_GROUP_RANDOM,
	};

	enum EmitterMotionType {
		EMITTER_NORMAL,//直接不动
		EMITTER_THROW,//抛物线投掷
		EMITTER_THROW_FAST,//直接减少lifetime的快速投掷
		//EMITTER_BOMB,//四散爆开（不规则）
		//EMITTER_BOMB_PHYSIC,//四散爆开（不规则），物理掉落
	};
	class Graphics;


	class Particle {
	public:
		float mX = 0;
		float mY = 0;
		float mZ = 0;
		unsigned int mLifeTime = 0;
		unsigned int mLifeTimeMax = 0;
		unsigned int mLifeTimeTick = 0;
		unsigned int mEachFrameStride = 100;

		bool mCostingLifeTime = true;
		bool mMoving = true;
		bool mVisible = true;

		unsigned int mTickCache = 0;
		SimpleImage** mImages = nullptr;
		ParticleMotionType mMotionType = PARTICLE_MOTION_NORMAL;
		ParticleImageType mImageType = PARTICLE_IMAGE_NONE;
		int mImageNumber = 1;
		float mSpeedX = 0;
		float mSpeedY = 0;
		float mSpeedZ = 0;
		float mImageOffsetX = 0;
		float mImageOffsetY = 0;
		bool mHasShadowed = false;
		float mGravity = 0;//重力加速度
		float mEnergyLossRate = 1;//能量损耗率，决定回弹高度，范围0~1
		float mRotateSpeed = 0;
		float mAngle = 0;
		float mScaleF = 1.0f;
		float mTransScaleF = 0.0f;
		bool mUseFade = true;
		SimpleImage* mShadowImage = nullptr;
		float mTickDelta = 0;

	public:
		Particle();
		~Particle();

		void Init();

		void Update();
		void Draw(Graphics* g) const;
		void TryToRenderImage(Graphics* g) const;
		
	};

	class Emitter {
	public:
		EmitterMotionType mMotionType = EMITTER_NORMAL;
		ParticleImageType mImageType;
		sgf::String* mImageIDs = nullptr;
		std::vector<SimpleImage*> mImages;
		int mImageNumber = 0;
		SimpleImage* mShadowImage = nullptr;
		unsigned int mLifeTimeMax;
		int mX = 0;
		int mY = 0;
		int mZ = 0;
		float mAngleMin = 0;
		float mAngleMax = 0;
		float mPowerMax = 0;
		float mPowerMin = 0;
		float mSpeedX = 0;
		float mSpeedY = 0;
		float mSpeedZ = 0;
		float mLossRate = 0;
		float mImageOffsetX = 0;
		float mImageOffsetY = 0;
		float mScaleF = 1.0f;
		float mTransScaleF = 0.0f;
		float mGravity = 0.1f;
		float mImageScaleRangeF = 0;
		float mRotateSpeed = 0;
		float mEnergyLossRate = 0.3f;
		bool mUseFade = true;
		bool mUseShadowed = false;

	public:

		void Init(EmitterMotionType motionType);
		void InitDefault();
		void SetAngleRange(float angleMin, float angleMax) { mAngleMin = angleMin; mAngleMax = angleMax; };
		Particle* Emitt();
		void AttachImage(SimpleImage* image) { mImages.push_back(image); };

		void AttachShadowImage(SimpleImage* image) { mShadowImage = image; };
		void MoveTo(int targetX, int targetY, int targetZ) { mX = targetX; mY = targetY; mZ = targetZ; };
		//加载paxml
		void LoadFromFile(const char* paxmlPath);
	};

	class AnimatorParticle {
	public:
		Animator* mAnimator = nullptr;
		float mX = 0;
		float mY = 0;
		float mScale = 1.0f;
		float mSpeed = 1.0f;


	public:
		AnimatorParticle(Reanimation* anim);
		~AnimatorParticle();
		void MoveTo(float x,float y);

		void Update();
		void Start(Animator::PlayState state = Animator::PlayState::PLAY_ONCE, float speed = 1.0f);
		void Draw(Graphics* g);
	};

	class ParticleManager {
	public:
		std::vector<Particle*> mParticles;
		std::vector<AnimatorParticle*> mReanimParticles;
		int mX = 0;
		int mY = 0;
		int mZ = 0;
		int mTickDelta = 0;

	public:
		ParticleManager();
		~ParticleManager();

		void Update();
		void Update(float tickDelta) { mTickDelta = tickDelta; Update(); };
		void Draw(sgf::Graphics* g);
		void MoveTo(int x, int y, int z) { mX = x; mY = y; mZ = z; };
		void MoveTo(int x,int y) { mX = x; mY = y;};

		Particle* EmittParticle(Emitter* srcEmitter);
		void EmittParticles(Emitter* srcEmitter, int number);

		AnimatorParticle* EmittReanimParticle(Reanimation* anim, float scale);
	};
}


#endif // !__SIMPLE_PARTICLE__
