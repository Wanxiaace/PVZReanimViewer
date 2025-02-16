#pragma once

#ifndef __POPCAP_COMPILED_READER__
#define __POPCAP_COMPILED_READER__

#define __POP_COM_USE_CONSOLE_LOG__

#include <iostream>
#include <zlib.h>

namespace PopCom {
	struct DataHeader
	{
		unsigned int mCookie;// 校验用
		unsigned int mUncompressedSize;// 源数据的长度
	};

	struct Data {
		DataHeader mHeader;
		const char* mData;
	};

	class ReanimTransform
	{
	public:
		float mTransX;
		float mTransY;
		float mSkewX;
		float mSkewY;
		float mScaleX;
		float mScaleY;
		float mFrame = 0;
		float mAlpha;
		std::string mImage;
		std::string mFont;
		std::string mText;
	};

	struct ReanimTrack
	{
		std::string mName;
		ReanimTransform* mTransforms;
		int	mTransformCount;
	};

	struct ReanimDefinition {
		ReanimTrack* mTracks = nullptr;
		int	mTrackCount = 0;
		float mFPS = 12;
	};

	constexpr int CURRENT_COOKILE = 0xDEADFED4;

	Data UncompressCompiledFromFile(const char* path);
	ReanimDefinition DecodeReanim(const char* xml, int xmlSize);

	void ZlibUncompress(const char* src, char* dst, unsigned long srcSize, unsigned long dstSize);

}


#endif // !__POPCAP_COMPILED_READER__
