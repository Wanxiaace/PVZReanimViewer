#include "Dictionary.h"
#include "GamePacker/GamePacker.h"
#include <fstream>

sgf::Dictionary::Dictionary()
{
}

sgf::Dictionary::~Dictionary()
{

}

void sgf::Dictionary::LoadFromFile(const char* path)
{
	mJson = FileManager::TryToLoadJsonFile(path);
}

sgf::String sgf::Dictionary::Dump() const
{
	return mJson.dump();
}

void sgf::Dictionary::Parse(const sgf::String& src)
{
	mJson = nlohmann::json::parse(src);
}

sgf::String sgf::Dictionary::GetString(const sgf::String& key)
{
	return mJson.value<sgf::String>(key, "<Missing \"" + key + "\" >");
}


sgf::String sgf::Dictionary::operator[](const sgf::String& key)
{
	return mJson[key];
}
