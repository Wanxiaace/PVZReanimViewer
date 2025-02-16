#include "Particle.h"
#include <map>
#include "GamePacker/GamePacker.h"

sgf::Particle* sgf::Emitter::Emitt()
{
	Particle* particle = new Particle();

	particle->mImageType = mImageType;
	particle->mLifeTimeMax = mLifeTimeMax;
	particle->mImages = mImages.data();
	particle->mImageOffsetX = mImageOffsetX;
	particle->mImageOffsetY = mImageOffsetY;
	particle->mX = mX;
	particle->mY = mY;
	particle->mZ = mZ;
	particle->mScaleF = mScaleF;
	float scRange = RandF(-mImageScaleRangeF, mImageScaleRangeF);
	particle->mScaleF += scRange;
	
	particle->mTransScaleF = mTransScaleF;
	particle->mGravity = mGravity;
	particle->mImageNumber = mImageNumber;
	particle->mUseFade = mUseFade;
	particle->mRotateSpeed = mRotateSpeed;
	particle->mEnergyLossRate = mEnergyLossRate;

	switch (mMotionType)
	{
	case sgf::EMITTER_NORMAL:
	{
		particle->mMotionType = PARTICLE_MOTION_NORMAL;
		particle->Init();
		particle->mHasShadowed = mUseShadowed;
		particle->mCostingLifeTime = true;
		particle->mMoving = false;
		break;
	}
	case sgf::EMITTER_THROW_FAST:
	{
		particle->mMotionType = PARTICLE_MOTION_THROW;
		particle->Init();
		particle->mHasShadowed = mUseShadowed;
		particle->mCostingLifeTime = true;
		particle->mMoving = true;
		float angle = RandF(mAngleMin, mAngleMax);

		float power = RandF(mPowerMin, mPowerMax);
		particle->mSpeedX = cosf(angle) * 30.0f * power;
		particle->mSpeedZ = sinf(angle) * -50.0f * power;
		particle->mShadowImage = mShadowImage;
		break;
	}
	case sgf::EMITTER_THROW: 
	{
		particle->mMotionType = PARTICLE_MOTION_THROW;
		particle->Init();
		particle->mHasShadowed = mUseShadowed;
		particle->mCostingLifeTime = false;
		particle->mMoving = true;
		float angle = RandF(mAngleMin, mAngleMax);

		float power = RandF(mPowerMin, mPowerMax);
		particle->mSpeedX = cosf(angle) * 30.0f * power;
		particle->mSpeedZ = sinf(angle) * -50.0f * power;
		particle->mShadowImage = mShadowImage;
		break;
	}
	default:
		break;
	}

	return particle;
}

std::map<sgf::String, sgf::ParticleImageType> gImageTypeMap = { 
	{"IMAGE_SINGLE",sgf::PARTICLE_IMAGE_SINGLE},
	{"IMAGE_GROUP",sgf::PARTICLE_IMAGE_GROUP_ANIMARION},
	{"IMAGE_GROUP_RANDOM",sgf::PARTICLE_IMAGE_GROUP_RANDOM},
};

std::map<sgf::String, sgf::EmitterMotionType> gEmitterMotionTypeMap = {
	{"EMITTER_THROW",sgf::EMITTER_THROW},
	{"EMITTER_NORMAL",sgf::EMITTER_NORMAL},
	{"EMITTER_THROW_FAST",sgf::EMITTER_THROW_FAST},
};

void sgf::Emitter::LoadFromFile(const char* paxmlPath)
{
	pugi::xml_parse_result result;
	pugi::xml_document doc = FileManager::TryToLoadXMLFile(paxmlPath,&result);

	if (!result) {
		std::cout << "Loading " << paxmlPath << " Error with: " << std::endl;
		std::cout << result.description() << std::endl;
	}

	const auto& particle = doc.root().child("Particle");
	int imageIndex = 0;
	for (const auto & x : particle)
	{
		sgf::String tag = x.name();
		if (tag == "ImageType") {
			mImageType = gImageTypeMap[x.text().as_string()];
			if (mImageType == sgf::PARTICLE_IMAGE_GROUP_ANIMARION || mImageType == sgf::PARTICLE_IMAGE_GROUP_RANDOM) {
				mImageNumber = x.attribute("size").as_int();
				mImageIDs = new sgf::String[mImageNumber];
			}
			else {
				mImageNumber = 1;
				mImageIDs = new sgf::String();
			}
		}
		if (tag == "Image") {
			mImageIDs[imageIndex] = x.text().as_string();
			imageIndex++;
		}
		if (tag == "LifeTime") {
			mLifeTimeMax = x.text().as_int();
		}
		if (tag == "LossRate") {
			mLossRate = x.text().as_float();
		}
		if (tag == "AngleMax") {
			mAngleMax = x.text().as_float() / 180 * 3.1415f;
		}
		if (tag == "AngleMin") {
			mAngleMin = x.text().as_float() / 180 * 3.1415f;
		}
		if (tag == "PowerMax") {
			mPowerMax = x.text().as_float();
		}
		if (tag == "PowerMin") {
			mPowerMin = x.text().as_float();
		}
		if (tag == "ImageOffsetX") {
			mImageOffsetX = x.text().as_float();
		}
		if (tag == "ImageOffsetY") {
			mImageOffsetY = x.text().as_float();
		}
		if (tag == "ImageScaleF") {
			mScaleF = x.text().as_float();
		}
		if (tag == "EmitterMotionType") {
			mMotionType = gEmitterMotionTypeMap[x.text().as_string()];
		}
		if (tag == "ImageTransScaleF") {
			mTransScaleF = x.text().as_float();
		}
		if (tag == "Gravity") {
			mGravity = x.text().as_float();
		}
		if (tag == "UseFade") {
			mUseFade = x.text().as_bool();
		}
		if (tag == "UseShadowed") {
			mUseShadowed = x.text().as_bool();
		}
		if (tag == "ImageRangeF") {
			mImageScaleRangeF = x.text().as_float();
		}
		if(tag == "RotateSpeed")
		{
			mRotateSpeed = x.text().as_float();
		}
		if (tag == "EnergyLossRate")
		{
			mEnergyLossRate = x.text().as_float();
		}
	}
}

void sgf::Emitter::Init(EmitterMotionType motionType)
{
	mMotionType = motionType;
	
}

void sgf::Emitter::InitDefault()
{

}

sgf::Particle::Particle()
{
}

sgf::Particle::~Particle()
{

}

void sgf::Particle::Init()
{
	mTickCache = TryGetTicks();
	mLifeTime = mLifeTimeMax;

	switch (mMotionType)
	{
	case sgf::PARTICLE_MOTION_NORMAL:
		break;
	case sgf::PARTICLE_MOTION_THROW:
		break;
	default:
		break;
	}

	switch (mImageType)
	{
	case PARTICLE_IMAGE_GROUP_RANDOM:
		mImages = &mImages[Rand(0, mImageNumber)];
		break;
	default:
		break;
	}

}

void sgf::Particle::Update()
{
	if (!mVisible)
		return;

	mScaleF += mTransScaleF * mTickDelta / 1000.0f;

	switch (mImageType)
	{
	case sgf::PARTICLE_IMAGE_GROUP_ANIMARION://PARTICLE_IMAGE_GROUP_ANIMARION
		mLifeTimeTick += mTickDelta;
		if (mLifeTime == 0) {
			mLifeTimeTick = 0;
		}
		mLifeTime = mLifeTimeMax - mLifeTimeTick / mEachFrameStride;
		break;
	}

	switch (mMotionType)
	{
	case sgf::PARTICLE_MOTION_THROW:
		mSpeedZ += mGravity * mTickDelta;
		if (mZ > 0) {
			mZ = 0;
			mSpeedZ *= -1.0f * mEnergyLossRate;
			if (int(mSpeedZ / 5.0f) == 0)
				mSpeedZ = 0;
			mSpeedX *= mEnergyLossRate;
			mRotateSpeed *= mEnergyLossRate;
			mCostingLifeTime = true;
		}


		break;
	default:
		break;
	}


	if (mCostingLifeTime) {
		if (mLifeTime > mTickDelta)
			mLifeTime -= mTickDelta;
		else
			mLifeTime = 0;
	}

	if (mLifeTime <= 0) {
		mVisible = false;
	}

	mZ += mSpeedZ * mTickDelta / 100.0f;
	mX += mSpeedX * mTickDelta / 100.0f;
	mY += mSpeedY * mTickDelta / 100.0f;
	mAngle += mRotateSpeed * mTickDelta / 100.0f;
}

void sgf::Particle::Draw(Graphics* g) const
{
	if (!mVisible)
		return;

	switch (mMotionType)
	{
	case sgf::PARTICLE_MOTION_NORMAL:
		TryToRenderImage(g);
		break;
	case sgf::PARTICLE_MOTION_THROW:
		TryToRenderImage(g);
		break;
	default:
		break;
	}
}

void sgf::Particle::TryToRenderImage(Graphics* g) const
{
	if (mUseFade) {
		g->SetCubeColor({ 1,1,1,1.0f * float(mLifeTime) / float(mLifeTimeMax) });
	}else
		g->SetCubeColor({ 1,1,1,1});

	switch (mImageType)
	{
	case PARTICLE_IMAGE_GROUP_RANDOM:
	{
		g->MoveTo(mX + mImageOffsetX* mScaleF, mY + mZ + mImageOffsetY* mScaleF);
		//g->DrawImageScaleF(mImages[0], mScaleF, mScaleF);
		g->DrawImageMatrix(mImages[0], glm::rotate(glm::scale(glm::mat4x4(1.0f), glm::vec3(mScaleF, mScaleF, 1.0f)), mAngle, glm::vec3(0.0f, 0.0f, 1.0f)), mImages[0]->GetWidth() / 2.0f, mImages[0]->GetHeight() / 2.0f);

		break;
	}
	case sgf::PARTICLE_IMAGE_SINGLE:
		g->SetCubeColor({ 1,1,1,1 });
		g->MoveTo(mX + mImageOffsetX, mY + mZ + mImageOffsetY);
		g->DrawImageMatrix(mImages[0],
			glm::rotate(
				glm::scale(glm::mat4x4(1.0f),glm::vec3(mScaleF, mScaleF,1.0f)), 
				mAngle, glm::vec3(0.0f, 0.0f, 1.0f)), mImages[0]->GetWidth() / 2.0f, mImages[0]->GetHeight() / 2.0f
		);
		break;

	case sgf::PARTICLE_IMAGE_GROUP_ANIMARION:
		g->SetCubeColor({ 1,1,1,1 });
		g->MoveTo(mX + mImageOffsetX * mScaleF, mY + mZ + mImageOffsetY * mScaleF);
		g->DrawImageScaleF(mImages[mLifeTimeMax - mLifeTime], mScaleF, mScaleF);
		break;
	default:
		break;
	}

	if (mHasShadowed && mShadowImage) {
		g->MoveTo(mX, mY + mImages[0]->GetHeight() - mShadowImage->GetHeight());
		g->DrawImage(mShadowImage);
	}
}


sgf::ParticleManager::ParticleManager()
{

}

sgf::ParticleManager::~ParticleManager()
{
	//TODO ÊÍ·Å×ÊÔ´
	for (auto x : mReanimParticles)
	{
		delete x;
	}

	for (auto x : mParticles)
	{
		delete x;
	}
}

void sgf::ParticleManager::Update()
{
	size_t length = mParticles.size();
	for (size_t i = 0; i < length; i++)
	{
		if (!mParticles[i]->mVisible) {
			mParticles.erase(mParticles.begin() + i);
			i--;
			length--;
		}
	}

	length = mReanimParticles.size();
	for (size_t i = 0; i < length; i++)
	{
		if (!mReanimParticles[i]->mAnimator->mIsPlaying) {
			mReanimParticles.erase(mReanimParticles.begin() + i);
			i--;
			length--;
		}
	}

	for (auto& x : mParticles)
	{
		x->mTickDelta = mTickDelta;
		x->Update();
	}

	for (auto& x : mReanimParticles)
	{
		x->Update();
	}
}

void sgf::ParticleManager::Draw(sgf::Graphics* g)
{
	for (auto& x :mParticles)
	{
		g->MoveTo(0,0);
		x->Draw(g);
	}

	for (auto& x : mReanimParticles)
	{
		g->MoveTo(0, 0);
		x->Draw(g);
	}
}

sgf::Particle* sgf::ParticleManager::EmittParticle(Emitter* srcEmitter)
{
	srcEmitter->MoveTo(mX, mY, mZ);
	Particle* dstParticle = srcEmitter->Emitt();
	mParticles.push_back(dstParticle);
	return dstParticle;
}

void sgf::ParticleManager::EmittParticles(Emitter* srcEmitter, int number)
{
	for (int i = 0; i < number; i++)
	{
		EmittParticle(srcEmitter);
	}
}

sgf::AnimatorParticle* sgf::ParticleManager::EmittReanimParticle(sgf::Reanimation* anim, float scale)
{
	AnimatorParticle* par = new AnimatorParticle(anim);
	par->MoveTo(mX,mY);
	par->Start(Animator::PLAY_ONCE);
	par->mScale = scale;
	mReanimParticles.push_back(par);
	return par;
}

sgf::AnimatorParticle::AnimatorParticle(Reanimation* anim)
{
	mAnimator = new Animator(anim);
}

sgf::AnimatorParticle::~AnimatorParticle()
{
	if (mAnimator)
		delete mAnimator;
}

void sgf::AnimatorParticle::MoveTo(float x, float y)
{
	mX = x;
	mY = y;
}

void sgf::AnimatorParticle::Update()
{
	if(mAnimator)
		mAnimator->Update();
}

void sgf::AnimatorParticle::Start(Animator::PlayState state, float speed)
{
	mAnimator->mSpeed = speed;
	mAnimator->Play(state);
}

void sgf::AnimatorParticle::Draw(Graphics* g)
{
	g->MoveTo(mX,mY);
	g->SetCubeColor({ 1,1,1,1 });
	glm::mat4x4 scaleMat = glm::scale(glm::mat4x4(1.0f), { mScale,mScale,1.0f });
	if (mAnimator)
		mAnimator->PresentMatrix(g, scaleMat);

}
