#ifndef __SGF_GAME_PACKER__
#define __SGF_GAME_PACKER__

#include <pugixml.hpp>
#include <nlohmann/json.hpp>

#include "../Common.h"
#include "../FileStream.h"
#include <iostream>
#include <vector>
#include <unordered_map>

#pragma pack(4)
namespace sgf {
	struct PakInFile {
		sgf::String mPath = "";
		void* mData = nullptr;
		int mSize = 0;
		int mFilePointer = 0;
	};

	class GamePacker
	{
	public:
		const char mSymbol[4] = "PKG";//文件特征头
		const int mCookie = 557667;//文件特征头
		int mFilePointer = 0;//文件内容区指针
		int mPackageSize = 0;//
		int mOriginalFileTotalNumber;//总文件个数
		int mOriginalFileTotalSize;//总文件大小

		std::unordered_map<sgf::String, PakInFile> mFiles;
		bool mIsReadingMode = false;
		FileStream* mFileStream = nullptr;

	public:
		GamePacker();
		~GamePacker();

		void UpdateFileInfo();

		void ReadFromFile(const sgf::String& path);
		bool ReadFromFileIfExist(const sgf::String& path);
		void WriteToFile(const sgf::String& path);
		FileStream* TryToLoadFilePointer(const sgf::String& path);
		void AppendNewFile(const sgf::String& path, void* pfile, int size);


		void Close();

		int GetDesOffset() const;
		int GetPackageFileSize() const;
		void UpdateAllFilePointer();
	};

	extern std::vector<GamePacker*> gPaks;

	namespace FileManager {
		void TryToLoadPak(const sgf::String& path);
		FileStream* TryToLoadFilePointer(const sgf::String& path);
		pugi::xml_document* TryToLoadXMLFilePointer(const sgf::String& path, pugi::xml_parse_result* error);
		pugi::xml_document TryToLoadXMLFile(const sgf::String& path, pugi::xml_parse_result* error);
		nlohmann::json TryToLoadJsonFile(const sgf::String& path);
		bool IsRealFileExist(const sgf::String& path);
	}

}

#endif // !__SGF_GAME_PACKER__
