#pragma once
#ifndef __SIMPLE_PLAYERINFO__
#define __SIMPLE_PLAYERINFO__

#include "Common.h"


namespace sgf {
	class PlayerInfo {
	public:
		int mFileSize = 0;
		nlohmann::json mJson;
		sgf::String mPath;

		///  «∑Ò∆Ù”√º”√‹
		bool mUseEncode = true;

	public:
		PlayerInfo();
		~PlayerInfo();


		void TryLoadFromFile(const sgf::String& path);

		void SaveAs(const sgf::String& dstPath);

		void Encode(unsigned char* src, int size);

		void Decode(unsigned char* src, int size);

		template<typename T>
		void GetElement(const sgf::String& key, T& dst) 
		{ 
			dst = mJson.at(key);
		};

		template<typename T>
		void SetElement(const sgf::String& key, const T& dst) 
		{ 
			mJson[key] = dst;
		};

		template<typename T>
		void SetElementDefault(const sgf::String& key, T& src, const T& defaultValue)
		{
			mJson[key] = defaultValue;
			src = defaultValue;
		};

		//void SaveAs();

	public:
		virtual void LoadDatas();
		virtual void CreateDefaultPlayerInfo(const sgf::String& path);
	};
}

#endif // !__SIMPLE_PLAYERINFO__
