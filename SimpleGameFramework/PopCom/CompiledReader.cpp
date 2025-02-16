#include "CompiledReader.h"
#include <iostream>
#include <fstream>

PopCom::Data PopCom::UncompressCompiledFromFile(const char* path)
{
	std::fstream file;
	Data result = {};
	file.open(path, std::ios_base::in | std::ios_base::binary);


	if (!file.is_open())
	{
#ifdef __POP_COM_USE_CONSOLE_LOG__
		std::cout << "Failed to open file at: " << path << std::endl;
#endif // __POP_COM_USE_CONSOLE_LOG__
		return result;
	}

	file.seekg(0,std::ios_base::end);
	int fileSize = file.tellg();
	file.seekg(0, std::ios_base::beg);
	char* comData = new char[fileSize];
	file.read(comData,fileSize);
	file.close();

	DataHeader* header = (DataHeader*)comData;
	if (header->mCookie != CURRENT_COOKILE) {
#ifdef __POP_COM_USE_CONSOLE_LOG__
		std::cout << "Incorrect Cookie !!! in file: " << path << std::endl;
#endif // __POP_COM_USE_CONSOLE_LOG__
		return result;
	}
	
	char* dst = new char[header->mUncompressedSize];
	memset(dst, 0, header->mUncompressedSize);
	ZlibUncompress(&comData[sizeof(DataHeader)], dst, fileSize - sizeof(DataHeader), header->mUncompressedSize);

	result.mData = dst;
	result.mHeader = *header;

	return result;
}

static int ReadInt32(const char* data,int* cursor)
{
	int result = *(int*) & data[*cursor];
	*cursor += 4;
	return result;
};

static int ReadFloat32(const char* data, int* cursor)
{
	float result = *(float*)&data[*cursor];
	*cursor += 4;
	return result;
};

std::string ReadStringWithHead(const char* data, int* cursor)
{
	int len = ReadInt32(data,cursor);

	char* strBuffer = new char[len+1];
	memcpy_s(strBuffer,len,data + *cursor,len);
	strBuffer[len] = '\0';
	std::string str = strBuffer;
	*cursor += len;

	return str;
};

PopCom::ReanimDefinition PopCom::DecodeReanim(const char* xml, int xmlSize)
{
	ReanimDefinition reanim = {};
	int cursor = 8;

	reanim.mTrackCount = ReadInt32(xml,&cursor);
	reanim.mTracks = new ReanimTrack[reanim.mTrackCount];
	reanim.mFPS = ReadFloat32(xml, &cursor);
	cursor += 8;

	for (size_t i = 0; i < reanim.mTrackCount; i++)
	{
		cursor += 8;
		ReanimTrack t;
		int transformCount = ReadInt32(xml, &cursor);
		t.mTransforms = new ReanimTransform[transformCount]{};
		t.mTransformCount = transformCount;
		reanim.mTracks[i] = t;
	}

	for (int i = 0; i < reanim.mTrackCount; i++) {
		ReanimTrack& t = reanim.mTracks[i];
		t.mName = ReadStringWithHead(xml,&cursor);
		cursor += 4;
		int times = t.mTransformCount;

		ReanimTransform ts = {};

		for (size_t j = 0; j < times; j++)
		{
			//ReanimTransform& ts = t.mTransforms[j];
			float tempfloat = ReadFloat32(xml,&cursor);
			if (abs(tempfloat - -10000.0f) > 0.001f)
			{
				ts.mTransX = tempfloat;
			}
			tempfloat = ReadFloat32(xml, &cursor);
			if (abs(tempfloat - -10000.0f) > 0.001f)
			{
				ts.mTransY = tempfloat;
			}
			tempfloat = ReadFloat32(xml, &cursor);
			if (abs(tempfloat - -10000.0f) > 0.001f)
			{
				ts.mSkewX = tempfloat;
			}
			tempfloat = ReadFloat32(xml, &cursor);
			if (abs(tempfloat - -10000.0f) > 0.001f)
			{
				ts.mSkewY = tempfloat;
			}
			tempfloat = ReadFloat32(xml, &cursor);
			if (abs(tempfloat - -10000.0f) > 0.001f)
			{
				ts.mScaleX = tempfloat;
			}
			tempfloat = ReadFloat32(xml, &cursor);
			if (abs(tempfloat - -10000.0f) > 0.001f)
			{
				ts.mScaleY = tempfloat;
			}
			tempfloat = ReadFloat32(xml, &cursor);
			if (abs(tempfloat - -10000.0f) > 0.001f)
			{
				ts.mFrame = tempfloat;
			}
			tempfloat = ReadFloat32(xml, &cursor);
			if (abs(tempfloat - -10000.0f) > 0.001f)
			{
				ts.mAlpha = tempfloat;
			}
			cursor += 12;
			t.mTransforms[j] = ts;
		}

		for (int j = 0; j < times; j++)
		{
			//ReanimTransform& ts = t.mTransforms[j];
			std::string tempstring = ReadStringWithHead(xml, &cursor);
			if (tempstring != "")
			{
				ts.mImage = tempstring;
			}
			tempstring = ReadStringWithHead(xml, &cursor);
			if (tempstring != "")
			{
				ts.mFont = tempstring;
			}
			tempstring = ReadStringWithHead(xml, &cursor);
			if (tempstring != "")
			{
				ts.mText = tempstring;
			}
			t.mTransforms[j] = ts;
		}
	}

	return reanim;
}

void PopCom::ZlibUncompress(const char* src, char* dst, unsigned long srcSize, unsigned long dstSize)
{
	uncompress((Bytef*)dst, &dstSize, (const Bytef*)src, srcSize);
}
