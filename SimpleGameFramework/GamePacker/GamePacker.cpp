#include "GamePacker.h"
#include "../GameApp.h"

std::vector<sgf::GamePacker*> sgf::gPaks;

sgf::GamePacker::GamePacker()
{
	UpdateFileInfo();
}

sgf::GamePacker::~GamePacker()
{
	if (mFileStream)
		mFileStream->Close();
}

void sgf::GamePacker::ReadFromFile(const sgf::String& path)
{
	mIsReadingMode = true;
	mFileStream = new FileStream();
	mFileStream->OpenFile(path.c_str());
	mFileStream->Seek(FileStream::CURSOR_SET,0);

	mFileStream->Read(
		this, offsetof(GamePacker, mFiles));
	
	for (int i = 0; i < mOriginalFileTotalNumber; i++)
	{
		PakInFile file;
		int pathSize = mFileStream->ReadInt();
		file.mSize = mFileStream->ReadInt();
		file.mFilePointer = mFileStream->ReadInt();
		file.mPath = mFileStream->ReadString(pathSize);
		mFiles[file.mPath] = file;
	}

}

bool sgf::GamePacker::ReadFromFileIfExist(const sgf::String& path)
{
	SDL_RWops* rwops = SDL_RWFromFile(path.c_str(),"r");
	if (rwops)
	{
		SDL_RWclose(rwops);
		ReadFromFile(path);
		return true;
	}
	return false;
}


void sgf::GamePacker::WriteToFile(const sgf::String& path)
{
	if (mIsReadingMode)
	{
		SHOW_ERROR_ABORT_EXIT("Dont write pak in reading mode");
		return;
	}
	FileStream outFile;
	outFile.OpenFile(path.c_str(), "wb");
	outFile.Write(this, offsetof(GamePacker, mFiles));
	
	UpdateAllFilePointer();
	GetPackageFileSize();

	for (auto& x : mFiles)
	{
		int pathsize = x.second.mPath.size();
		outFile.Write(&pathsize, 4);
		int fsize = x.second.mSize;
		outFile.Write(&fsize, 4);
		int ptr = x.second.mFilePointer;
		outFile.Write(&ptr, 4);

		outFile.Write(x.second.mPath.c_str(), pathsize);
	}

	for (auto& x : mFiles)
	{
		outFile.Write(x.second.mData,x.second.mSize);
	}


	outFile.Close();
}

sgf::FileStream* sgf::GamePacker::TryToLoadFilePointer(const sgf::String& path)
{
	if (!mIsReadingMode)
	{
		SHOW_ERROR_ABORT_EXIT("You can only load file in reading mode");
		return nullptr;
	}

	try {
		auto& x = mFiles.at(path);
		FileStream* result = new FileStream();
		mFileStream->Seek(FileStream::CURSOR_SET, x.mFilePointer);
		char* file = new char[x.mSize];
		mFileStream->Read(file, x.mSize);
		result->OpenMemory(file, x.mSize);
		return result;
	}
	catch (std::out_of_range) {
		return nullptr;
	}

	return nullptr;
}

void sgf::GamePacker::Close()
{
	mFileStream->Close();
}

int sgf::GamePacker::GetDesOffset() const
{
	int poffset = offsetof(GamePacker, mFiles);
	for (auto& x : mFiles)
	{
		poffset += x.second.mPath.size() + 12;
	}
	return poffset;
}

void sgf::GamePacker::UpdateAllFilePointer()
{
	int poffset = GetDesOffset();
	for (auto& x : mFiles)
	{
		x.second.mFilePointer = poffset;
		poffset += x.second.mSize;
	}
}

void sgf::GamePacker::AppendNewFile(const sgf::String& path, void* pfile, int size)
{
	if (mIsReadingMode)
	{
		SHOW_ERROR_ABORT_EXIT("Dont write pak in reading mode");
		return;
	}
	PakInFile file;
	file.mPath = path;
	file.mData = pfile;
	file.mSize = size;
	mOriginalFileTotalSize += size;
	mOriginalFileTotalNumber += 1;
	mFiles[file.mPath] = file;
	UpdateFileInfo();
}

int sgf::GamePacker::GetPackageFileSize() const
{
	int strSize = GetDesOffset();
	for (auto& x : mFiles)
	{
		strSize += x.second.mSize;
	}
	return strSize;
}

void sgf::GamePacker::UpdateFileInfo()
{
	mPackageSize = GetPackageFileSize();
	mFilePointer = GetDesOffset();
}


void sgf::FileManager::TryToLoadPak(const sgf::String& path)
{
	GamePacker* pkg = new GamePacker();
	if (pkg->ReadFromFileIfExist(path))
		gPaks.push_back(pkg);
	else {
		delete pkg;
		gGameApp->Log() << "Faild to load package at: " << path << std::endl;
	}
}

//线程安全修复

static std::mutex gLoadingMutex;

sgf::FileStream* sgf::FileManager::TryToLoadFilePointer(const sgf::String& path)
{
	if (IsRealFileExist(path)) {
		FileStream* result = new FileStream();
		result->OpenFile(path.c_str());
		return result;
	}

	gLoadingMutex.lock();
	sgf::FileStream* file = nullptr;
	for (auto& x : gPaks)
	{
		file = x->TryToLoadFilePointer(path);
		if (file)
		{
			gLoadingMutex.unlock();
			return file;
		}
	}

	gLoadingMutex.unlock();
	return nullptr;
}


pugi::xml_document* sgf::FileManager::TryToLoadXMLFilePointer(const sgf::String& path, pugi::xml_parse_result* error)
{
	sgf::FileStream* file = TryToLoadFilePointer(path);
	pugi::xml_document* result = new pugi::xml_document();
	if (error)
		*error = result->load_string(file->ReadString(file->GetSize()).c_str());
	else
		result->load_string(file->ReadString(file->GetSize()).c_str());
	delete file;

	return result;
}

pugi::xml_document sgf::FileManager::TryToLoadXMLFile(const sgf::String& path, pugi::xml_parse_result* error)
{
	sgf::FileStream* file = TryToLoadFilePointer(path);
	pugi::xml_document result;
	if (error)
		*error = result.load_string(file->ReadString(file->GetSize()).c_str());
	else
		result.load_string(file->ReadString(file->GetSize()).c_str());
	delete file;

	return result;
}

nlohmann::json sgf::FileManager::TryToLoadJsonFile(const sgf::String& path)
{
	sgf::FileStream* file = TryToLoadFilePointer(path);
	nlohmann::json json = nlohmann::json::parse(file->ReadString(file->GetSize()));
	delete file;
	return json;
}

bool sgf::FileManager::IsRealFileExist(const sgf::String& path)
{
	std::ifstream ifs;
	ifs.open(path);
	if (ifs.is_open())
	{
		ifs.close();
		return true;
	}
	return false;
}
