#include "Graphics.h"
#include <string>


void sgf::Graphics::ResizeCube(float targetX, float targetY, float targetWidth, float targetHeight)
{
	for (size_t i = 0; i < 6; i++)
	{
		mCubeVertices[i].x = targetX;
		mCubeVertices[i].y = targetY;
	}

	mCubeVertices[0].x += targetWidth;
	mCubeVertices[0].y += targetHeight;
	mCubeVertices[1].x += targetWidth;

	mCubeVertices[3].x += targetWidth;
	mCubeVertices[3].y += targetHeight;
	mCubeVertices[4].y += targetHeight;
}

void sgf::Graphics::SetCubeImagePostion(float u, float v, float width, float height)
{
	for (size_t i = 0; i < 6; i++)
	{
		mCubeVertices[i].u *= width;
		mCubeVertices[i].v *= height;

		mCubeVertices[i].u += u;
		mCubeVertices[i].v += v;
	}
}

void sgf::Graphics::ReSetCubeImagePostion()
{
	mCubeVertices[0].u = 1.0f;
	mCubeVertices[0].v = 1.0f;

	mCubeVertices[1].u = 1.0f;
	mCubeVertices[1].v = 0.0f;

	mCubeVertices[2].u = 0.0f;
	mCubeVertices[2].v = 0.0f;

	mCubeVertices[3].u = 1.0f;
	mCubeVertices[3].v = 1.0f;

	mCubeVertices[4].u = 0.0f;
	mCubeVertices[4].v = 1.0f;

	mCubeVertices[5].u = 0.0f;
	mCubeVertices[5].v = 0.0f;
}

void sgf::Graphics::SetCubeTextureIndex(float index)
{
	for (size_t i = 0; i < 6; i++)
	{
		mCubeVertices[i].texIndex = index;
	}
}

void sgf::Graphics::SetCubeMatrixIndex(float index)
{
	for (size_t i = 0; i < 6; i++)
	{
		mCubeVertices[i].matrixIndex = index;
	}
}

sgf::Graphics::Graphics(GameApp* gameApp)
{

	mTextureProgram.LoadFromFile("shaders/TextureVertexShader.glsl", "shaders/TextureFragmentShader.glsl");
	mBlurProgram.LoadFromFile("shaders/TextureVertexShader.glsl", "shaders/TextureFragmentShaderBlur.glsl");
	
	glGenVertexArrays(1, &mCubeVAO);

	glGenBuffers(1, &mCubeVBO);

	glBindVertexArray(mCubeVAO);

	ProjectionResize(gameApp->mWidth, gameApp->mHeight);
	mGameApp = gameApp;
}

void sgf::Graphics::Clear()
{
	glClearColor(0,0,0,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}


void sgf::Graphics::SetCubeColor(const sgf::Color& srcColor)
{
	mCubeColor = srcColor;
	for (size_t i = 0; i < 6; i++)
	{
		mCubeVertices[i].r = srcColor.r;
		mCubeVertices[i].g = srcColor.g;
		mCubeVertices[i].b = srcColor.b;
		mCubeVertices[i].a = srcColor.a;
	}
}

void sgf::Graphics::MulCubeColor(const sgf::Color& srcColor)
{
	mCubeColor *= srcColor;
	for (size_t i = 0; i < 6; i++)
	{
		mCubeVertices[i].r *= srcColor.r;
		mCubeVertices[i].g *= srcColor.g;
		mCubeVertices[i].b *= srcColor.b;
		mCubeVertices[i].a *= srcColor.a;
	}
}

void sgf::Graphics::AppendVertices(const Vertex* vertices, int vertexCount)
{
	for (int i = 0; i < vertexCount; i++)
	{
		mVerticesBuffer.push_back(vertices[i]);
		
	}
	mVerticesNumber += vertexCount;
	/*mSimpleVerticesCount = vertexCount;
	glBindVertexArray(mCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER,mCubeVBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(Vertex) * vertexCount, vertices,GL_STATIC_DRAW);
	sgf::Vertex::InformationInit();*/
}


void sgf::Graphics::ActiveTextureShader()
{
	SwitchShader(&mTextureProgram);
}

void sgf::Graphics::ActiveBlurShader()
{
	SwitchShader(&mBlurProgram);
}

void sgf::Graphics::SwitchShader(SimpleProgram* prog)
{
	if (mNowProgram == prog)
		return;

	if (mNowProgram && !CheckIsBatchEmpty())
		Present();

	mNowProgram = prog;
	prog->Use();

	glUniformMatrix4fv(glGetUniformLocation(mNowProgram->mProgram, "proj"), 1, GL_FALSE, glm::value_ptr(mProjectMatrix));

	
}

void sgf::Graphics::DrawTriangleArrays()
{
	glBindVertexArray(mCubeVAO);

	/*glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(5.0f);*/

	GL_CALL(glDrawArrays(GL_TRIANGLES, 0, mVerticesNumber));
	
}


void sgf::Graphics::FillRect(int width, int height)
{
	Point graPos = GetGraphicsTransformPosition();
	ResizeCube(graPos.x, graPos.y, width, height);

	AppendVertices(mCubeVertices,6);

	CheckSubmit();
}

void sgf::Graphics::FillRect(const FloatRect& src)
{
	MoveTo(src.mX, src.mY);
	FillRect(src.mWidth,src.mHeight);
}


int sgf::Graphics::TryToBindNewTexture(sgf::SimpleImage* src)
{
	auto itr = std::find(mImagesBuffer.begin(), mImagesBuffer.end(), src);
	if (itr != mImagesBuffer.end())
		return itr - mImagesBuffer.begin();//mTexturesNumbe
	int unitMaxCount = GetMaxTextureUnitCount();
	GLenum textureUnit = GL_TEXTURE0 + mImagesNumber;
	if (mImagesNumber < unitMaxCount) {
		unsigned int texureHandle = src->GenerateTexture();
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, texureHandle);

		GLint textureArrayIndex = glGetUniformLocation(mNowProgram->mProgram, ("textures[" + std::to_string(mImagesNumber) + "]").c_str());

		glUniform1i(textureArrayIndex, mImagesNumber);
		mImagesBuffer.push_back(src);

	}
	mImagesNumber++;
	return mImagesNumber - 1;
}

int sgf::Graphics::TryToBindNewMatrix(glm::mat4x4 src)
{

	mMatrixsBuffer.push_back(src);
	mMatrixsNumber++;
	return mMatrixsNumber - 1;
}

void sgf::Graphics::DrawImage(sgf::SimpleImage* src)
{
	int tex = TryToBindNewTexture(src);
	for (size_t i = 0; i < 6; i++)
	{
		mCubeVertices[i].texIndex = tex;
	}

	if (src->mIsAtlasUnit)
	{
		SetCubeImagePostion(src->mAtlasUnitU, src->mAtlasUnitV, src->mAtlasUnitWidth, src->mAtlasUnitHeight);
	}

	FillRect(src->GetWidth(), src->GetHeight());

	if (src->mIsAtlasUnit)
	{
		ReSetCubeImagePostion();
	}

	for (size_t i = 0; i < 6; i++)
	{
		mCubeVertices[i].texIndex = -1;
	}
}

void sgf::Graphics::DrawImageScaleF(sgf::SimpleImage* src, float scaleX, float scaleY)
{
	int tex = TryToBindNewTexture(src);
	for (size_t i = 0; i < 6; i++)
	{
		mCubeVertices[i].texIndex = tex;
	}

	if (src->mIsAtlasUnit)
	{
		SetCubeImagePostion(src->mAtlasUnitU, src->mAtlasUnitV, src->mAtlasUnitWidth, src->mAtlasUnitHeight);
	}

	FillRect(src->GetWidth() * scaleX, src->GetHeight() * scaleY);

	if (src->mIsAtlasUnit)
	{
		ReSetCubeImagePostion();
	}

	for (size_t i = 0; i < 6; i++)
	{
		mCubeVertices[i].texIndex = -1;
	}
}

void sgf::Graphics::DrawImageInRect(sgf::SimpleImage* src, float width, float height)
{
	int tex = TryToBindNewTexture(src);
	SetCubeTextureIndex(tex);

	if (src->mIsAtlasUnit)
	{
		SetCubeImagePostion(src->mAtlasUnitU, src->mAtlasUnitV, src->mAtlasUnitWidth, src->mAtlasUnitHeight);
	}

	FillRect(width, height);


	if (src->mIsAtlasUnit)
	{
		ReSetCubeImagePostion();
	}

	SetCubeTextureIndex(-1);
}

void sgf::Graphics::DrawImageMatrix(sgf::SimpleImage* src, glm::mat4x4 matrix, float oriX, float oriY)
{
	Point graPos = GetGraphicsTransformPosition();
	glm::vec3 pivotPoint = glm::vec3(oriX,oriY,0);

	glm::mat4 finalTransform;
	if (!oriX && !oriY)
		finalTransform = matrix;
	else
		finalTransform = glm::translate(glm::mat4(1.0f), pivotPoint) *
			matrix *
			glm::translate(glm::mat4(1.0f), -pivotPoint);

	float matrixPosition = TryToBindNewMatrix(glm::translate(glm::mat4x4(1.0f), glm::vec3(graPos.x, graPos.y, 0)) * finalTransform);
	int tex = TryToBindNewTexture(src);

	SetCubeTextureIndex(tex);
	SetCubeMatrixIndex(matrixPosition);

	if (src->mIsAtlasUnit)
	{
		SetCubeImagePostion(src->mAtlasUnitU, src->mAtlasUnitV, src->mAtlasUnitWidth, src->mAtlasUnitHeight);
	}
	ResizeCube(0, 0, src->GetWidth(), src->GetHeight());

	AppendVertices(mCubeVertices, 6);

	if (src->mIsAtlasUnit)
	{
		ReSetCubeImagePostion();
	}
	
	CheckSubmit();

	SetCubeTextureIndex(-1);
	SetCubeMatrixIndex(-1);
}

void sgf::Graphics::DrawImageGridAtlas(sgf::SimpleImage* src, float rowNum, float columnNum, float targetRow, float targetColumn)
{
	DrawImageAtlas(src, targetColumn / columnNum, targetRow / rowNum, 1.0f / columnNum, 1.0f / rowNum);
}

void sgf::Graphics::DrawImageGridAtlasScaleF(sgf::SimpleImage* src, float rowNum, float columnNum, float targetRow, float targetColumn, float scaleX, float scaleY)
{
	DrawImageAtlasScaleF(src, targetColumn / columnNum, targetRow / rowNum, 1.0f / columnNum, 1.0f / rowNum, scaleX, scaleY);
}

void sgf::Graphics::DrawImageAtlas(sgf::SimpleImage* src,float u, float v,float width,float height)
{

	SetCubeImagePostion(u,v,width,height);

	DrawImageScaleF(src, width, height);

	ReSetCubeImagePostion();

}

void sgf::Graphics::DrawImageAtlasScaleF(sgf::SimpleImage* src, float u, float v, float width, float height, float scaleX, float scaleY)
{

	SetCubeImagePostion(u, v, width, height);

	DrawImageScaleF(src, width * scaleX, height * scaleY);

	ReSetCubeImagePostion();

}

void sgf::Graphics::Translate(float x, float y)
{
	mTransformPosition.x += x;
	mTransformPosition.y += y;
}

void sgf::Graphics::DrawTexture(unsigned int tex, float width, float height)
{
	int unitMaxCount = GetMaxTextureUnitCount();

	GLenum textureUnit = GL_TEXTURE0 + mImagesNumber;
	if (mImagesNumber < unitMaxCount) {
		unsigned int texureHandle = tex;
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, texureHandle);

		GLint textureArrayIndex = glGetUniformLocation(mNowProgram->mProgram, ("textures[" + std::to_string(mImagesNumber) + "]").c_str());

		glUniform1i(textureArrayIndex, mImagesNumber);
	}

	SetCubeTextureIndex(mImagesNumber);

	mImagesNumber++;
	mTexturesNumber++;

	FillRect(width, height);
	SetCubeTextureIndex(-1);
}

void sgf::Graphics::DrawTextureReversed(unsigned int tex, float width, float height)
{
	int unitMaxCount = GetMaxTextureUnitCount();

	float matrixPosition = TryToBindNewMatrix(glm::translate(glm::scale(glm::mat4x4(1.0f), glm::vec3(1, -1, 1)),
		glm::vec3(0, -height,0)));

	GLenum textureUnit = GL_TEXTURE0 + mImagesNumber;
	if (mImagesNumber < unitMaxCount) {
		unsigned int texureHandle = tex;
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, texureHandle);

		GLint textureArrayIndex = glGetUniformLocation(mNowProgram->mProgram, ("textures[" + std::to_string(mImagesNumber) + "]").c_str());

		glUniform1i(textureArrayIndex, mImagesNumber);
	}

	SetCubeTextureIndex(mImagesNumber);
	SetCubeMatrixIndex(matrixPosition);

	mImagesNumber++;
	mTexturesNumber++;

	FillRect(width, height);

	SetCubeTextureIndex(-1);
	SetCubeMatrixIndex(-1);
}

void sgf::Graphics::MoveTo(float x, float y)
{
	mTransformPosition.x = x;
	mTransformPosition.y = y;
}

void sgf::Graphics::ModelMoveTo(float x, float y)
{
	mModelTransformPosition.x = x;
	mModelTransformPosition.y = y;
}

void sgf::Graphics::ProjectionResize(float width, float height)
{
	mProjectMatrix = glm::ortho(0.0f, width,
		height, 0.0f, -1.0f, 1.0f);
	mTextureProgram.Use();
	GL_CALL(glUniformMatrix4fv(glGetUniformLocation(mTextureProgram.mProgram, "proj"), 1, GL_FALSE, glm::value_ptr(mProjectMatrix)););
}

void sgf::Graphics::Present()
{
	Submit();
	DrawTriangleArrays();

	mVerticesNumber = 0;
	mImagesNumber = 0;
	mMatrixsNumber = 0;
	mTexturesNumber = 0;

	mVerticesBuffer.clear();
	mImagesBuffer.clear();
	mMatrixsBuffer.clear();
}

int sgf::Graphics::GetMaxTextureUnitCount() const
{
	return mGameApp->mTextureNumberMax;
}

void sgf::Graphics::Submit()
{
	glBindVertexArray(mCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER,mCubeVBO);
	//调试
	glBufferData(GL_ARRAY_BUFFER,sizeof(Vertex)*mVerticesNumber,mVerticesBuffer.data(),GL_STATIC_DRAW);
	glUniformMatrix4fv(glGetUniformLocation(mNowProgram->mProgram,"view"),1,GL_FALSE, (float*) & mGameApp->mViewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(mNowProgram->mProgram,"posMatrix"),mMatrixsNumber,GL_FALSE, (float*) mMatrixsBuffer.data());

	Vertex::InformationInit();
}

void sgf::Graphics::CheckSubmit()
{
	if (mVerticesNumber >= VERTEX_BUFFER_MAX_COUNT)
		Present();
	if (mMatrixsNumber >= MATRIX_BUFFER_MAX_COUNT)
		Present();
	if (mImagesNumber >= GetMaxTextureUnitCount())
		Present();
}

bool sgf::Graphics::CheckIsBatchEmpty() const
{
	return !(mVerticesNumber || mMatrixsNumber || mImagesNumber);
}

void sgf::Graphics::GenFrameBuffer(unsigned int* fbo, unsigned int* tex,int width, int height)
{
	glGenFramebuffers(1, fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

	unsigned int currentBoundTexture = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&currentBoundTexture);

	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex, 0);
	glBindTexture(GL_TEXTURE_2D, currentBoundTexture);

}

void sgf::Graphics::BindFrameBuffer(unsigned int fbo)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void sgf::Graphics::ResetFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void sgf::Graphics::ReleaseFrameBuffer(unsigned int fbo)
{
	glDeleteFramebuffers(1,&fbo);
}

void sgf::Graphics::ReleaseTexture(unsigned int tex)
{
	glDeleteTextures(1,&tex);
}

sgf::Point sgf::Graphics::GetGraphicsTransformPosition()
{
	return { mTransformPosition.x + mModelTransformPosition.x,
		mTransformPosition.y + mModelTransformPosition.y,};
}


static void FlipPixelsVertically(unsigned int* pixels, int width, int height) {
	for (int row = 0; row < height / 2; ++row) {
		int rowIndex = row * width;
		int oppositeRowIndex = (height - row - 1) * width;

		for (int col = 0; col < width; ++col) {
			unsigned int temp = pixels[rowIndex + col];
			pixels[rowIndex + col] = pixels[oppositeRowIndex + col];
			pixels[oppositeRowIndex + col] = temp;
		}
	}
}

sgf::SimpleImage* sgf::Graphics::CaptureScreen(int x, int y, int width, int height)
{
	sgf::SimpleImage* result = new sgf::SimpleImage();
	unsigned int* pixels = new unsigned int[width * height];//四字节一个像素，可以视为int
	glReadBuffer(GL_FRONT);
	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	FlipPixelsVertically(pixels, width, height);

	result->LoadFromSurface(SDL_CreateRGBSurfaceFrom(pixels,
		width, height,
		32,
		width * 4,
		0xff,
		0xff00,
		0xff0000,
		0xff000000));

	return result;
}

void sgf::Graphics::SetClipRect(const sgf::FloatRect& src)
{
	Present();

	int w, h;//实际宽高
	SDL_GetWindowSize(mGameApp->mGameWindow, &w, &h);

	ProjectionResize(src.mWidth, 
		src.mHeight);

	glViewport(src.mX / mGameApp->mMouseXScale,
		(gGameApp->mHeight - (src.mY + src.mHeight))
		/ mGameApp->mMouseYScale,
		src.mWidth / mGameApp->mMouseXScale,
		src.mHeight / mGameApp->mMouseYScale);

	//FillRect({ 0,0,1000,1000 }); //Debug用
}


void sgf::Graphics::ClearClipRect()
{
	Present();
	int w, h;
	SDL_GetWindowSize(mGameApp->mGameWindow,&w,&h);
	//ProjectionResize(GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT);
	ProjectionResize(mGameApp->mWidth, mGameApp->mHeight);
	glViewport(0, 0, w, h);
	//glViewport(0, 0, w, h);
}
