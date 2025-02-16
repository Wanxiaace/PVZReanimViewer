#pragma once
#ifndef __SIMPLE_GRAPHICS__
#define __SIMPLE_GRAPHICS__
#include "SimpleApp.h"
#include "SimpleImage.h"
#include "Program.h"
#include "Common.h"
#include <vector>
#include "GameApp.h"

namespace sgf {
	class GameApp;
	class Graphics;

	class Graphics {
	protected:
		void ResizeCube(float targetX,float targetY,float targetWidth,float targetHeight);
		void SetCubeImagePostion(float u,float v,float width,float height);
		void ReSetCubeImagePostion();
		void SetCubeTextureIndex(float index);
		void SetCubeMatrixIndex(float index);

	public:
		unsigned int mCubeVAO;
		unsigned int mCubeVBO;

		///利用更多的纹理单元和批处理渲染减少drawcall，从而优化在图片多的时候卡顿的问题
		bool mUseBufferOptimize = true;

		Vertex mCubeVertices[6] = {
			{100,	100,	0.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	-1,		-1},
			{100,	0,		0.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	0.0f,	-1,		-1},
			{0,		0,		0.0f,	1.0f,	1.0f,	1.0f,	1.0f,	0.0f,	0.0f,	-1,		-1},
			{100,	100,	0.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	1.0f,	-1,		-1},
			{0,		100,	0.0f,	1.0f,	1.0f,	1.0f,	1.0f,	0.0f,	1.0f,	-1,		-1},
			{0,		0,		0.0f,	1.0f,	1.0f,	1.0f,	1.0f,	0.0f,	0.0f,	-1,		-1},
		};

	public:
		sgf::GameApp* mGameApp = nullptr;
		sgf::Color mCubeColor = { 255,255,255 };
		SimpleProgram mTextureProgram;
		SimpleProgram mBlurProgram;


		glm::mat4x4 mProjectMatrix;

		Point mTransformPosition = {0,0};
		Point mModelTransformPosition = {0,0};
		SimpleProgram* mNowProgram = nullptr;

		std::vector<Vertex> mVerticesBuffer;
		std::vector<SimpleImage*> mImagesBuffer;
		std::vector<glm::mat4x4> mMatrixsBuffer;

		int mVerticesNumber = 0;
		int mImagesNumber = 0;
		int mMatrixsNumber = 0;
		int mTexturesNumber = 0;


	public:
		Graphics(GameApp* gameApp);
		~Graphics() {};

		/// @brief 清除屏幕
		void Clear();

		/// @brief 设置颜色
		/// @param srcColor 目标颜色
		void SetCubeColor(const sgf::Color& srcColor);

		/// @brief 以相乘的方式混合颜色
		/// @param srcColor 目标颜色
		void MulCubeColor(const sgf::Color& srcColor);

		/// @brief 将顶点加入缓冲区
		/// @param vertices 目标顶点数组
		/// @param vertexCount 目标顶点个数
		void AppendVertices(const Vertex* vertices,int vertexCount);

		/// @brief 绘制三角形
		void DrawTriangleArrays();

		/// @brief 启用纹理着色器
		void ActiveTextureShader();

		// Filter
		/// @brief 启用BoxBlur着色器
		void ActiveBlurShader();

		/// @brief 切换着色器
		void SwitchShader(SimpleProgram* prog);

		/// @brief 填充矩形,调用后会调用Graphics::Move()
		/// @param width 宽度
		/// @param height 高度
		void FillRect(int width, int height);
		/// @brief 填充矩形
		/// @param src 目标矩形
		void FillRect(const FloatRect& src);

		/// @brief 绑定一张贴图到缓冲区
		/// @param src 目标贴图
		/// @return 如果绑定重复的贴图则返回索引，如果绑定新的贴图则尝试绑定并返回索引
		int TryToBindNewTexture(sgf::SimpleImage* src);

		/// @brief 绑定一个矩阵到缓冲区
		/// @param src 目标矩阵
		/// @return 尝试绑定矩阵并返回索引
		int TryToBindNewMatrix(glm::mat4x4 src);


		/// @brief 绘制图片
		/// @param src 目标图片
		void DrawImage(sgf::SimpleImage* src);

		/// @brief 绘制图片拉伸
		/// @param src 目标图片
		/// @param scaleX 横向比例拉伸
		/// @param scaleY 纵向比例拉伸
		void DrawImageScaleF(sgf::SimpleImage* src,float scaleX = 1.0f, float scaleY = 1.0f);

		/// @brief 绘制图片并撑满矩形
		/// @param src 目标图片
		/// @param width 目标宽度
		/// @param height 目标高度
		void DrawImageInRect(sgf::SimpleImage* src, float width, float height);

		/// @brief 绘制图片并应用矩阵
		/// @param src 目标图片
		/// @param matrix 目标矩阵
		/// @param oriX 原点X(暂时无用)
		/// @param oriY 原点Y(暂时无用)
		void DrawImageMatrix(sgf::SimpleImage* src, glm::mat4x4 matrix, float oriX = 0, float oriY = 0);

		/// @brief 根据份数进行切片
		/// @param src 目标图片
		/// @param rowNum 行总数
		/// @param columnNum 列总数
		/// @param targetRow 目标行
		/// @param targetColumn 目标列
		void DrawImageGridAtlas(sgf::SimpleImage* src, float rowNum, float columnNum, float targetRow, float targetColumn);
		
		/// @brief 根据份数进行切片，并拉伸变换
		/// @param src 目标图片
		/// @param rowNum 行总数
		/// @param columnNum 列总数
		/// @param targetRow 目标行
		/// @param targetColumn 目标列
		/// @param scaleX 横向比例拉伸
		/// @param scaleY 纵向比例拉伸
		void DrawImageGridAtlasScaleF(sgf::SimpleImage* src, float rowNum, float columnNum, float targetRow, float targetColumn, float scaleX, float scaleY);

		/// @brief 绘制纹理图集的一部分,所有参数采用uv坐标系，范围为0~1
		/// @param src 目标图片
		/// @param u 横坐标
		/// @param v 纵坐标
		/// @param width 宽度
		/// @param height 高度
		void DrawImageAtlas(sgf::SimpleImage* src, float u, float v, float width, float height);
		
		/// @brief 绘制纹理图集的一部分，并拉伸变换，所有参数采用uv坐标系，范围为0~1
		/// @param src 目标图片
		/// @param u 横坐标
		/// @param v 纵坐标
		/// @param width 宽度
		/// @param height 高度
		/// @param scaleX 横向比例拉伸
		/// @param scaleY 纵向比例拉伸
		void DrawImageAtlasScaleF(sgf::SimpleImage* src, float u, float v, float width, float height, float scaleX, float scaleY);

		/// @brief 平移gra临时原点到某处
		/// @param x
		/// @param y
		void Translate(float x, float y);

		void DrawTexture(unsigned int tex, float width, float height);

		void DrawTextureReversed(unsigned int tex, float width, float height);

		/// @brief 移动gra临时原点到某处
		/// @param x
		/// @param y
		void MoveTo(float x, float y);

		/// @brief 移动gra原点到某处
		/// @param x
		/// @param y
		void ModelMoveTo(float x, float y);

		/// @brief 重新设置渲染范围
		/// @param width 宽度
		/// @param height 高度
		void ProjectionResize(float width, float height);

		/// @brief 提交所有顶点并渲染
		void Present();

		/// @brief 获得最大的纹理单元数量
		/// @return 返回最大支持的纹理单元数量，一般为32
		int GetMaxTextureUnitCount() const;

		/// @brief 提交所有顶点
		void Submit();
		/// @brief 检查缓冲区是否达到上限，如果是就调用Present
		void CheckSubmit();

		/// @brief 检查缓冲区是否有元素
		bool CheckIsBatchEmpty() const;

		/// @brief 创建一个Color帧缓冲并绑定到上去
		/// @param fbo 创建的帧缓冲储存地址
		/// @param tex 创建的帧缓冲纹理储存地址
		/// @param width
		/// @param height
		/// @return 会将fbo指向的值设置为新的帧缓冲，tex指向的值设为新的纹理
		void GenFrameBuffer(unsigned int* fbo, unsigned int* tex, int width, int height);

		/// @brief 绑定一个帧缓冲对象
		/// @param fbo 目标帧缓冲
		void BindFrameBuffer(unsigned int fbo);

		/// @brief 切换为屏幕默认的帧缓冲对象
		void ResetFrameBuffer();

		void ReleaseFrameBuffer(unsigned int fbo);

		void ReleaseTexture(unsigned int tex);

		/// @brief 获得Gra的实际坐标（原点坐标+临时原点坐标）
		/// @return x和y组成的pair
		Point GetGraphicsTransformPosition();

		/// @brief 主要用于获得屏幕帧缓冲的像素数据，用于截图之类的，实际上是读取当前绑定的帧缓存的数据
		/// @param x 开始读取的x
		/// @param y 开始读取的y
		/// @param width 读取的宽度
		/// @param height 读取高度
		/// @return 返回当前绑定的帧缓存的图像
		SimpleImage* CaptureScreen(int x, int y, int width, int height);

		/// @brief 设置裁切矩形
		void SetClipRect(const sgf::FloatRect& src);

		void ClearClipRect();
	};

	static Graphics* CreateFromApp(GameApp* gameApp) { return new Graphics(gameApp); };

}

#endif
