#pragma once
#ifndef __SIMPLE_COMMON__
#define __SIMPLE_COMMON__

#ifdef _DEBUG
#define SGF_ASSERT(x) _ASSERT(x);
#else
#define SGF_ASSERT(x) x;
#endif

#define SGF_CONFIRM_LOG(x,y) if(!x)printf(y);

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

#include <GL/glew.h>

#include <iostream>

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <pugixml.hpp>
#include <nlohmann/json.hpp>

#include <random>
#include <thread>
#include <chrono>
#include <functional>
using namespace std::chrono;

const int VERTEX_BUFFER_MAX_COUNT = 1200;
const int MATRIX_BUFFER_MAX_COUNT = 200;

const int LOADING_THREAD_NUM_MAX = 10;


static void CheckGLError(const char* file, int line)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        switch (error)
        {

        case GL_INVALID_ENUM:
            printf("GL Error: GL_INVALID_ENUM %s : %d \n", file, line);
            break;
        case GL_INVALID_VALUE:
            printf("GL Error: GL_INVALID_VALUE %s : %d \n", file, line);
            break;
        case GL_INVALID_OPERATION:
            printf("GL Error: GL_INVALID_OPERATION %s : %d \n", file, line);
            break;
        case GL_STACK_OVERFLOW:
            printf("GL Error: GL_STACK_OVERFLOW %s : %d \n", file, line);
            break;
        case GL_STACK_UNDERFLOW:
            printf("GL Error: GL_STACK_UNDERFLOW %s : %d \n", file, line);
            break;
        case GL_OUT_OF_MEMORY:
            printf("GL Error: GL_OUT_OF_MEMORY %s : %d \n", file, line);
            break;
        default:
            printf("GL Error: 0x%x %s : %d \n", error, file, line);
            break;
        }
    }
};
#define  GL_CALL(x) do{x;CheckGLError(__FILE__,__LINE__);}while(0) 

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define SHOW_ERROR(x) SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Error",x,nullptr);
#ifdef _DEBUG
#define SHOW_ERROR_ABORT_EXIT(x) { SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Error",x,nullptr); __debugbreak(); abort(); }
#else
#define SHOW_ERROR_ABORT_EXIT(x) { SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Error",x,nullptr); exit(EXIT_FAILURE); }
#endif

#include "SString.h"

namespace sgf {
    static void SetStdOutStream(std::ostream& src)
    {
        std::cout.rdbuf(src.rdbuf());
        std::cerr.rdbuf(src.rdbuf());
    };

	template<typename T>
	class Rect {
	public:
		T mX, mY;
		T mWidth, mHeight;

		Rect operator*(T mul) {
			return { mX * mul ,mY * mul,mWidth * mul,mHeight * mul };
		}

        Rect operator+(Rect t2) {
            return { mX + t2.mX , mY + t2.mY,mWidth + t2.mWidth,mHeight + t2.mHeight };
        }

        void operator+=(Rect t2) {
            mX += t2.mX;
            mY += t2.mY;
            mWidth += t2.mWidth;
            mHeight += t2.mHeight;
        }

        bool IsPointIn(T pointX, T pointY) {
            if (pointX > mX && pointX < mX + mWidth && pointY > mY && pointY < mY + mHeight)
                return true;
            return false;
        };

        bool IsOverlap(const Rect& other) const {
            if (mX + mWidth <= other.mX || other.mX + other.mWidth <= mX) return false;
            if (mY + mHeight <= other.mY || other.mY + other.mHeight <= mY) return false;
            return true;
        }
        
	};

	typedef Rect<int> IntRect;
	typedef Rect<float> FloatRect;

    class Color {
    public:
        float r, g, b, a;

        Color& operator*=(const Color& src) {
            r *= src.r;
            g *= src.g;
            b *= src.b;
            a *= src.a;
            return *this;
        }

        Color operator*(const Color& src) {
            return { r * src.r, g * src.g, b * src.b, a * src.a };
        }
    };

    struct Point
    {
        float x;
        float y;

        void operator*(float mul) { x *= mul, y *= mul; };
        Point operator+(Point p2) { return{ x + p2.x, y + p2.y }; };
        Point operator-(Point p2) { return{ x - p2.x, y - p2.y }; };
    };

    struct GameMat44 {
        union {
            float m[4][4];
            struct
            {
                float x1, x2, x3, x4;
                float y1, y2, y3, y4;
                float z1, z2, z3, z4;
                float t1, t2, t3, t4;
            };
        };
    };


    static unsigned int TryGetTicks() {
        //可调多倍速
        return SDL_GetTicks();
    }

    static unsigned long long TryGetTicks64() {
        return SDL_GetTicks64();
    }

    static std::default_random_engine m_randomAdapter;

    static void SRand(unsigned int seed) {
        m_randomAdapter.seed(seed);
    };

    //注意，该函数无法达到rangeMax，最大值是rangeMax-1
    static int Rand(int rangeMin,int rangeMax) {
        int range = rangeMax - rangeMin;
        return m_randomAdapter() % range + rangeMin;
    };

    static float RandF(float rangeMin, float rangeMax) {
        float range = rangeMax - rangeMin;
        if (!int(range * 1000.0f))
            return rangeMin;
        return float(m_randomAdapter() % int(range * 1000.0f)) / 1000.0f + rangeMin;
    };

    static void WaitUntil(std::function<bool()> req) {
        while (!req())
        {
            std::this_thread::sleep_for(100ms);
        }
    };

    enum MouseStyle {
        MOUSE_STYLE_ARROW,
        MOUSE_STYLE_WAIT,
        MOUSE_STYLE_HAND,
        MOUSE_STYLE_NONE,
    };

    static void SetMouseStyle(MouseStyle style) {
#ifdef _WIN32
        static HCURSOR hArrowCur = LoadCursor(NULL, IDC_ARROW);
        static HCURSOR hWaitCur = LoadCursor(NULL, IDC_WAIT);
        static HCURSOR hHandCur = LoadCursor(NULL, IDC_HAND);

        static SDL_Cursor* hSDLArrowCur = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        static SDL_Cursor* hSDLWaitCur = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
        static SDL_Cursor* hSDLHandCur = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

        switch (style)
        {
        case sgf::MOUSE_STYLE_ARROW:
            SDL_SetCursor(hSDLArrowCur);
            SetCursor(hArrowCur);
            break;
        case sgf::MOUSE_STYLE_WAIT:
            SDL_SetCursor(hSDLWaitCur);
            SetCursor(hWaitCur);
            break;
        case sgf::MOUSE_STYLE_HAND:
            SDL_SetCursor(hSDLHandCur);
            SetCursor(hHandCur);
            break;
        case sgf::MOUSE_STYLE_NONE:
            SDL_ShowCursor(false);
            ShowCursor(FALSE);
            break;
        default:
            break;
        }
#endif
    }

}
#endif
