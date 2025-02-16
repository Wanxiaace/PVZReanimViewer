#include "FileStream.h"

sgf::FileStream::FileStream()
{

}

sgf::FileStream::~FileStream()
{
	Close();
}

void sgf::FileStream::OpenFile(const sgf::String& path, const sgf::String& mode,bool throwError)
{
	mIStream = SDL_RWFromFile(path.c_str(), mode.c_str());

	if (mIStream)
	{
		mIsFile = true;
		mIsOpen = true;
	}
	else if(throwError) {
		SHOW_ERROR_ABORT_EXIT(("Failed To Load File At: " + path + " in Mode:" + mode).c_str());
	}
}

void sgf::FileStream::OpenFileIfExist(const sgf::String& path, const sgf::String& mode)
{
	OpenFile(path, mode, false);
}

void sgf::FileStream::OpenMemory(void* pbuffer, int size)
{
	mIStream = SDL_RWFromMem(pbuffer,size);
	mIsOpen = true;
}

void sgf::FileStream::Seek(CursorStyle style, int cur)
{
	CHECK_LOADED;

	switch (style)
	{
	case sgf::FileStream::CursorStyle::CURSOR_SET:
		SDL_RWseek(mIStream, cur, RW_SEEK_SET);
		break;
	case sgf::FileStream::CursorStyle::CURSOR_END:
		SDL_RWseek(mIStream, cur, RW_SEEK_END);
		break;
	case sgf::FileStream::CursorStyle::CURSOR_CUR:
		SDL_RWseek(mIStream, cur, RW_SEEK_CUR);
		break;
	default:
		break;
	}
}

void sgf::FileStream::Pass(int sride)
{
	Seek(CURSOR_CUR,sride);
}

void sgf::FileStream::Read(void* pbuffer, int size)
{
	CHECK_LOADED;
	SDL_RWread(mIStream,pbuffer,size,1);
}

void sgf::FileStream::Write(const void* src, int size)
{
	CHECK_LOADED;
	SDL_RWwrite(mIStream,src,size,1);
}

void sgf::FileStream::Close()
{
	CHECK_LOADED;
	if(mIsOpen)
		SDL_RWclose(mIStream);
	mIsOpen = false;
}

int sgf::FileStream::Tell() const
{
	CHECK_LOADED_X(-1);
	return SDL_RWtell(mIStream);
}

int sgf::FileStream::GetSize() const
{
	return SDL_RWsize(mIStream);
}

sgf::String sgf::FileStream::ReadString(int size)
{
	CHECK_LOADED_X("");
	char* buf = new char[size+1];
	memset(buf,0,size+1);
	Read(buf,size);
	std::string result = buf;
	delete[] buf;
	return result;
}

