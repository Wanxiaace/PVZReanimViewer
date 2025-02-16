#include "ResourceManager.h"
#include "SimpleImage.h"
#include "Reanimation.h"
#include "Font.h"
#include "Particle.h"

sgf::ResourceManager::ResourceManager()
{

}

sgf::ResourceManager::~ResourceManager()
{
	for (auto & x: mResourcePool)
	{
		delete x.second;
	}
}

void sgf::ResourceManager::LoadImageWithID(const sgf::String& path, sgf::String id)
{
	SimpleImage* image = new SimpleImage();
	image->LoadFromFile(path.c_str());

	mResLoadMutex.lock();
	mResourcePool[id] = image;
	mResLoadMutex.unlock();
}

#include "GamePacker/GamePacker.h"

void sgf::ResourceManager::LoadImageAtlas(const sgf::String& aPath, const sgf::String& folder, sgf::String id_info)
{
	nlohmann::json json = FileManager::TryToLoadJsonFile(aPath);

	nlohmann::json& frameArray = json.at("frames");
	sgf::String texPath = json.at("meta").at("image");

	LoadImageWithID(folder + "/" + texPath, id_info + SString::StringtoUpper(SString::StringRemoveFileExtension(texPath)));
	
	mResLoadMutex.lock();
	SimpleImage* tex = GetResourceFast<SimpleImage>(id_info + SString::StringtoUpper(SString::StringRemoveFileExtension(texPath)));
	mResLoadMutex.unlock();

	for (auto& x : frameArray)
	{
		SimpleImage* atlasUnit = new SimpleImage();
		atlasUnit->mSurface = tex->mSurface;

		sgf::String rid = id_info + SString::StringtoUpper(SString::StringRemoveFileExtension(x.at("filename")));
		atlasUnit->mIsLoadedTexture = true;
		atlasUnit->mIsAtlasUnit = true;
		atlasUnit->mAtlasSrc = tex;

		mResLoadMutex.lock();
		mResourcePool[rid] = atlasUnit;
		mResLoadMutex.unlock();
		

		atlasUnit->mAtlasUnitU = float(x.at("frame").at("x")) / tex->mSurface->w;
		atlasUnit->mAtlasUnitV = float(x.at("frame").at("y")) / tex->mSurface->h;

		atlasUnit->mAtlasUnitWidth = float(x.at("frame").at("w")) / tex->mSurface->w;
		atlasUnit->mAtlasUnitHeight = float(x.at("frame").at("h")) / tex->mSurface->h;

		//std::cout << rid << std::endl;

	}

}

void sgf::ResourceManager::LoadReanimWithID(const sgf::String& path, sgf::String id)
{
	Reanimation* reanim = new Reanimation();
	reanim->LoadFromFile(path.c_str());
	reanim->mResourceManager = this;
	mResLoadMutex.lock();
	mResourcePool[id] = reanim;
	mResLoadMutex.unlock();
	//mResourcePool[id] = reanim;
}

void sgf::ResourceManager::LoadReanimByDefinitionWithID(const PopCom::ReanimDefinition& def, sgf::String id)
{
	Reanimation* reanim = new Reanimation();
	reanim->mFPS = def.mFPS;

	for (int i = 0; i < def.mTrackCount;i++) {
		TrackInfo aInfo = TrackInfo{ "" };
		int aCounter = 0;

		aInfo.mTrackName = def.mTracks[i].mName;

		for (int j = 0; j < def.mTracks[i].mTransformCount; j++) {
			TrackFrameTransform aFInfo;
			PopCom::ReanimTransform& trans = def.mTracks[i].mTransforms[j];
			aFInfo.x = trans.mTransX;
			aFInfo.y = trans.mTransY;
			aFInfo.kx = trans.mSkewX;
			aFInfo.ky = trans.mSkewY;
			aFInfo.sx = trans.mScaleX;
			aFInfo.sy = trans.mScaleY;
			aFInfo.a = trans.mAlpha;
			aFInfo.f = trans.mFrame;
			aFInfo.i = trans.mImage;
			reanim->mImagesSet->insert(aFInfo.i);
			aInfo.mFrames.push_back(aFInfo);
		}

		reanim->mTracks->push_back(aInfo);
	}

	reanim->mIsLoaded = true;
	reanim->mResourceManager = this;

	mResLoadMutex.lock();
	mResourcePool[id] = reanim;
	mResLoadMutex.unlock();
}

void sgf::ResourceManager::LoadCompiledReanimWithID(const sgf::String& aPath, sgf::String id)
{
	
	PopCom::Data data = PopCom::UncompressCompiledFromFile(aPath.c_str());
	PopCom::ReanimDefinition def = PopCom::DecodeReanim(data.mData, data.mHeader.mUncompressedSize);
	LoadReanimByDefinitionWithID(def,id);
}

void sgf::ResourceManager::LoadFontWithID(const sgf::String& aPath, sgf::String id)
{
	Font* font = new Font();
	font->LoadFromFile(aPath.c_str());
	//mResourcePool[id] = font;
	mResLoadMutex.lock();
	mResourcePool[id] = font;
	mResLoadMutex.unlock();
}

//请在加载完所有图片后调用此函数
void sgf::ResourceManager::LoadParticleWithID(const sgf::String& aPath, sgf::String id)
{
	Emitter* particle = new Emitter();
	particle->LoadFromFile(aPath.c_str());
	int length = particle->mImageNumber;
	
	mEmitters.push_back(particle);
	mResLoadMutex.lock();
	mResourcePool[id] = particle;
	mResLoadMutex.unlock();
	//mResourcePool[id] = particle;
}

void sgf::ResourceManager::LoadParticleImages()
{
	for (auto& x : mEmitters)
	{
		Emitter* emitter = (Emitter*)x;
		int length = emitter->mImageNumber;
		for (int i = 0; i < length; i++)
		{
			emitter->AttachImage(GetResource<SimpleImage>(emitter->mImageIDs[i]));
		}
		emitter->AttachShadowImage(GetResource<SimpleImage>("IMAGE_PLANTSHADOW"));

	}
}


static void LoadFromResouceListFunc(sgf::ResourceManager* tar,sgf::ResourceList* src, sgf::MusicManager* mus,int beginIndex,int endIndex)
{
	//int len = src->mResouces.size();
	float delta = 100.0f / src->mResouces.size();

	for (int i = beginIndex; i < endIndex; i++) {
		tar->mNowFile += 1;
		auto& x = src->mResouces[i];
		tar->mNowInfo = &x;


		if (x.folder.size() >= 5 && x.folder.substr(x.folder.size() - 5) == "atlas" && x.path.substr(x.path.size() - 4) == "json") {
			//std::cout << "Atlas" << std::endl;
			tar->LoadImageAtlas((tar->mBasePath + x.path), (tar->mBasePath + x.folder), sgf::SString::StringtoUpper(x.folder.substr(0, x.folder.size() - 5)));
		}

		if (x.folder == "image" || x.folder == "image_reanim" || x.folder == "particle") {
			tar->LoadImageWithID((tar->mBasePath + x.path), x.id);
		}
		else if (x.folder == "raxml" || x.folder == "praxml") {
			tar->LoadReanimWithID((tar->mBasePath + x.path), x.id);
		}
		else if (x.folder == "reanim_compiled") {
			tar->LoadCompiledReanimWithID((tar->mBasePath + x.path), x.id);
		}

		if (x.folder == "font") {
			tar->LoadFontWithID((tar->mBasePath + x.path), x.id);
		}

		if (x.folder == "chunk") {
			mus->LoadChunkFormFile((tar->mBasePath + x.path).c_str(), x.id);
		}

		if (x.folder == "music") {
			mus->LoadMusicFormFile((tar->mBasePath + x.path).c_str(), x.id);
		}

		if (x.folder == "paxml") {
			tar->LoadParticleWithID((tar->mBasePath + x.path).c_str(), x.id);
		}
		//输出加载的路径
		//std::cout << x.path << std::endl;
		tar->mProcess += delta;
	}
}

void sgf::ResourceManager::LoadFromResouceList(ResourceList* src,MusicManager* mus)
{
	mIsLoaded = false;
	mProcess = 0.0f;
	float delta = 100.0f / src->mResouces.size();
	mTotalFile = src->mResouces.size();

	if (mTotalFile <= LOADING_THREAD_NUM_MAX + 50) {
		LoadFromResouceListFunc(this, src, mus, 0, mTotalFile);
	}
	else {
		mNowFile = 0;
		int len = src->mResouces.size();

		int stride = len / (LOADING_THREAD_NUM_MAX + 1);

		std::vector<std::thread*> loadingThreads;
		for (size_t i = 0; i < LOADING_THREAD_NUM_MAX; i++)
		{
			loadingThreads.push_back(new std::thread(LoadFromResouceListFunc, this, src, mus, i * stride, i * stride + stride));
		}

		LoadFromResouceListFunc(this, src, mus, LOADING_THREAD_NUM_MAX * stride, len);

		for (size_t i = 0; i < LOADING_THREAD_NUM_MAX; i++)
		{
			loadingThreads[i]->join();
			delete loadingThreads[i];
		}
	}

	LoadParticleImages();
	mIsLoaded = true;
}

#include "GamePacker/GamePacker.h"

void sgf::ResourceList::Load(const char* path)
{
	mPath = sgf::String(path);
	pugi::xml_parse_result err;
	pugi::xml_document doc = FileManager::TryToLoadXMLFile(path,&err);

	if (!err) {
		gGameApp->Log() << "Faild to load ResourceList: " 
			<< path << std::endl;
		return;
	}

	for (auto& x : *doc.children().begin()) {
		for (auto& y : x.children()) {
			ResouceInfo aInf;
			sgf::String fPath = x.name() + sgf::String("/") + y.attribute("path").as_string();
			aInf.path = fPath;
			aInf.id = y.attribute("id").as_string();
			aInf.folder = x.attribute("folder").as_string();
			mResouces.push_back(aInf);
		}
	}
}
