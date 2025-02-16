#pragma once
#ifndef __SIMPLE_MUSIC_SYSTEM__
#define __SIMPLE_MUSIC_SYSTEM__

#include "Common.h"

#include <unordered_map>

namespace sgf {

	class MusicManager {
	public:
		std::unordered_map<String,Mix_Chunk*> mChunkMap;
		std::unordered_map<String,Mix_Music*> mMusicMap;

	public:
		MusicManager();
		~MusicManager();

		void PlayChunk(const String& chunkKey,int channel = -1);
		void PlayMusic(const String& musicKey);

		void PauseMusic();
		void ResumeMusic();
		void FadeOutMusic(int ms = 1000);
		void FadeInMusic(const String& musicKey,int ms = 1000);

		void SetMusicVolume(float volume);//range from 0~1
		void SetChunkVolume(float volume, const String& key);
		void SetChannelVolume(int channel,float volume);

		void MixPlayChunk(Mix_Chunk* chunk, int channel);
		void MixPlayMusic(Mix_Music* music);

		void LoadChunkFormFile(const char* path, const String& key);
		void LoadMusicFormFile(const char* path, const String& key);

		Mix_Chunk* TryGetChunkByKey(const String& key);
		Mix_Music* TryGetMusicByKey(const String& key);

	};
}

#endif // !__SIMPLE_MUSIC_SYSTEM__
