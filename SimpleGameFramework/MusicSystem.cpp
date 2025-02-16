#include "MusicSystem.h"
#include "GameApp.h"

sgf::MusicManager::MusicManager()
{

}

sgf::MusicManager::~MusicManager()
{
	for (auto& x :mMusicMap)
	{
		Mix_FreeMusic(x.second);
	}

	for (auto& x : mChunkMap)
	{
		Mix_FreeChunk(x.second);
	}
}

void sgf::MusicManager::PlayChunk(const String& chunkKey, int channel)
{
	MixPlayChunk(mChunkMap[chunkKey],channel);
}

void sgf::MusicManager::PlayMusic(const String& musicKey)
{
	MixPlayMusic(mMusicMap[musicKey]);
}

void sgf::MusicManager::PauseMusic()
{
	Mix_PauseMusic();
}

void sgf::MusicManager::ResumeMusic()
{
	Mix_ResumeMusic();
}

void sgf::MusicManager::FadeOutMusic(int ms)
{
	Mix_FadeOutMusic(ms);
}

void sgf::MusicManager::FadeInMusic(const String& musicKey, int ms)
{
	Mix_FadeInMusic(mMusicMap[musicKey],0,ms);
}

void sgf::MusicManager::SetMusicVolume(float volume)
{
	Mix_VolumeMusic(volume * MIX_MAX_VOLUME);
}

void sgf::MusicManager::SetChunkVolume(float volume,const String& key)
{
	Mix_Chunk* chunk = TryGetChunkByKey(key);
	Mix_VolumeChunk(chunk, volume * MIX_MAX_VOLUME);
}

void sgf::MusicManager::SetChannelVolume(int channel, float volume)
{
	Mix_Volume(channel, volume * MIX_MAX_VOLUME);
}

void sgf::MusicManager::MixPlayChunk(Mix_Chunk* chunk,int channel)
{
	Mix_PlayChannel(channel,chunk,0);
}

void sgf::MusicManager::MixPlayMusic(Mix_Music* music)
{
	Mix_PlayMusic(music, -1);
}

#include "GamePacker/GamePacker.h"

void sgf::MusicManager::LoadChunkFormFile(const char* path,const String& key)
{
	FileStream* chunkFile = FileManager::TryToLoadFilePointer(path);
	Mix_Chunk* chunk = Mix_LoadWAV_RW(chunkFile->mIStream,1);
	
	if (chunk) {
		mChunkMap[key] = chunk;
	}
	else {
		gGameApp->Log() << "Failed to load Chunk At: " << path << std::endl;
		gGameApp->Log() << Mix_GetError() << std::endl;
		SHOW_ERROR_ABORT_EXIT((sgf::String("Failed to load Chunk At: ") + path).c_str());
		//std::cout << "Failed to load Chunk At: " << path << std::endl;
	}

	//delete chunkFile;
}

void sgf::MusicManager::LoadMusicFormFile(const char* path, const String& key)
{
	FileStream* chunkFile = FileManager::TryToLoadFilePointer(path);
	Mix_Music* chunk = Mix_LoadMUS_RW(chunkFile->mIStream, 1);
	
	if (chunk) {
		mMusicMap[key] = chunk;
	}
	else {
		gGameApp->Log() << "Failed to load Music At: " << path << std::endl;
		gGameApp->Log() << Mix_GetError() << std::endl;
		SHOW_ERROR_ABORT_EXIT((sgf::String("Failed to load Music At: ") + path).c_str());
	}

	//delete chunkFile;
}

Mix_Chunk* sgf::MusicManager::TryGetChunkByKey(const String& key)
{
	return mChunkMap[key];
}


Mix_Music* sgf::MusicManager::TryGetMusicByKey(const String& key)
{
	return mMusicMap[key];
}
