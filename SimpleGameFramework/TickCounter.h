#pragma once
#ifndef __SGF_TICK_COUNTER__
#define __SGF_TICK_COUNTER__

#include "Common.h"

namespace sgf {
	class TickCounter
	{
	protected:
		unsigned int mTickCache = 0;
		unsigned int mTickDelta = 0;

	public:
		TickCounter* mBind = nullptr;

	public:
		TickCounter() { mTickCache = sgf::TryGetTicks(); };
		~TickCounter() {};

		void BindToCounter(TickCounter* tar) { mBind = tar; };
		unsigned int GetDeltaTick() const { return mTickDelta; };
		float GetDeltaTickFloat() const { return mTickDelta; };

		void Update() {
			if (mBind)
				mTickDelta = mBind->GetDeltaTick();
			else {
				unsigned int tickNow = sgf::TryGetTicks();
				mTickDelta = tickNow - mTickCache;
				mTickCache = tickNow;
			}
		}
	};
}

#endif // !__SGF_TICK_COUNTER__
