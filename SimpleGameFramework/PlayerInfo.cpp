#include "PlayerInfo.h"
#include <fstream>
#include "GamePacker/GamePacker.h"

sgf::PlayerInfo::PlayerInfo()
{

}

sgf::PlayerInfo::~PlayerInfo()
{

}

void sgf::PlayerInfo::TryLoadFromFile(const sgf::String& path)
{
	if (!FileManager::IsRealFileExist(path))
	{
		CreateDefaultPlayerInfo(path);
		SaveAs(path);
		return;
	}

	mPath = path;
	std::ifstream file;
	file.open(path.c_str(),std::ios_base::binary);
	file.seekg(0, std::ios::end);
	mFileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	unsigned char* data = new unsigned char[mFileSize + 1];
	memset(data,0, mFileSize + 1);
	file.read((char*)data, mFileSize);

	if (mUseEncode) {
		Decode(data, mFileSize);
	}

	mJson = nlohmann::json::parse(data);

	delete[] data;
	file.close();

	LoadDatas();
}

void sgf::PlayerInfo::SaveAs(const sgf::String& dstPath)
{
	std::ofstream out;
	out.open(dstPath, std::ios_base::binary);
	sgf::String jsonStr = mJson.dump();
	if (mUseEncode) {
		unsigned char* data = new unsigned char[jsonStr.size()];
		memcpy_s(data, jsonStr.size(), jsonStr.c_str(), jsonStr.size());
		Encode(data, jsonStr.size());

		out.write((const char*)data, jsonStr.size());
		delete[] data;

		out.close();
	}
	else {
		out.write(jsonStr.c_str(), jsonStr.size());
		out.close();
	}

	
}

void sgf::PlayerInfo::Encode(unsigned char* src, int size)
{
	for (int i = 0; i < size; i++)
	{
		src[i] ^= 0xF7;
	}
}

void sgf::PlayerInfo::Decode(unsigned char* src, int size)
{
	for (int i = 0; i < size; i++)
	{
		src[i] ^= 0xF7;
	}
}


void sgf::PlayerInfo::LoadDatas()
{

}

void sgf::PlayerInfo::CreateDefaultPlayerInfo(const sgf::String& path)
{
	
}

