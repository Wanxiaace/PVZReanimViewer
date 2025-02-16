#pragma once
#ifndef __SGF_EFFECT_HOLDER__
#define __SGF_EFFECT_HOLDER__

#include "Common.h"
#include <functional>

namespace sgf {

	template<typename T>
	static T EaseInOutCurve(T t, T min, T max, T duration) {
		t /= (duration / 2.0);
		if (t < 1.0) {
			return (max - min) / 2.0 * t * t * t + min;
		}
		t -= 2.0;
		return (max - min) / 2.0 * (t * t * t + 2.0) + min;
	}

	enum CurveType {
		CURVE_LINER,
		CURVE_EASE_IN_OUT,
	};

	template<typename T>
	class EffectHolder {
	public:
		float mEffectCounter = 0;
		float mEffectCounterMax = 0;
		TickCounter mTick;
		CurveType mType = CURVE_LINER;
		std::function<void()> mNextFunc;
		T* mSpot = nullptr;
		T mRangeMax;
		T mRangeMin;
		T mSpeed;
		T mProcess;
		T mValue;
		T mBaseValue;
		bool mIsRunning = false;
		bool mIsBindNextFunc = false;

	public:
		EffectHolder() {};
		~EffectHolder() {};

		/// @brief After calling SetSpeed(),Bind a var to apply
		void BindSpot(T* spot, T max, T min,CurveType type = CURVE_LINER) {
			mType = type;
			mSpot = spot; mRangeMax = max; mRangeMin = min;
			if(mSpeed > 0)
				mBaseValue = mRangeMin;
			else 
				mBaseValue = mRangeMax;
			mValue = mBaseValue;
			mProcess = 0;
			Start();
		};
		void SetSpeed(T speed) { mSpeed = speed; };

	public:
		void Update() {
			mTick.Update();
			if (!mIsRunning)
				return;

			T tar = mProcess + (T)mTick.GetDeltaTick() / 1000.0 * mSpeed;

			switch (mType)
			{
			case sgf::CURVE_LINER:
				mValue = mProcess + mBaseValue;//Япад
				break;
			case sgf::CURVE_EASE_IN_OUT:
			{
				if (mSpeed >= 0)
					mValue = EaseInOutCurve<T>(mProcess, mRangeMin, mRangeMax, 1000);
				else
					mValue = EaseInOutCurve<T>(mProcess, mRangeMax, mRangeMin, -1000);
				break;
			}
			}

			if (mSpeed >= 0) {
				
				if (mValue > mRangeMax) {
					mProcess = 0;
					mValue = mRangeMax;
					mIsRunning = false;
					if (mIsBindNextFunc)
					{
						mIsBindNextFunc = false;
						mNextFunc();
					}
				}
				else {
					mProcess = tar;
				}
			}
			else {
				if (mValue < mRangeMin) {
					mProcess = 0;
					mValue = mRangeMin;
					mIsRunning = false;
					if (mIsBindNextFunc)
					{
						mIsBindNextFunc = false;
						mNextFunc();
					}
				}
				else {
					mProcess = tar;
				}
			}
			if (mSpot)
				*mSpot = mValue;
		};

		void Start() { mIsRunning = true;};
		void SetNextFunction(std::function<void()> func) { mNextFunc = func; mIsBindNextFunc = true; };
	};

	using EffectHolderFloat = EffectHolder<float>;
	using EffectHolderDouble = EffectHolder<double>;
	using EffectHolderInt = EffectHolder<int>;
}

#endif // !__SGF_EFFECT_HOLDER__
