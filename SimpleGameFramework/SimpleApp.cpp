#include "SimpleApp.h"
#include <iostream>
#include <mutex>

std::mutex sgf::gLoopMutex;
std::thread* sgf::gUpdateThread;

sgf::GameAppBase::GameAppBase(int width, int height, const sgf::String& windowCaptain,bool enableASync, bool resiziable)
{
    mLog = &std::cout;
    mWidth = width;
    mHeight = height;
    mViewMatrix = glm::mat4x4(1.0f);
    mViewMatrixBuf = glm::mat4x4(1.0f);
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    Mix_Init(MIX_InitFlags::MIX_INIT_MP3 | MIX_InitFlags::MIX_INIT_OGG | MIX_InitFlags::MIX_INIT_WAVPACK | MIX_InitFlags::MIX_INIT_MID);
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_CHANNELS, 4096);

    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_WEBP);
    Uint32 windowFlags = SDL_WindowFlags::SDL_WINDOW_OPENGL;
    if(resiziable)
        windowFlags |= SDL_WindowFlags::SDL_WINDOW_RESIZABLE;
    //windowFlags |= SDL_WindowFlags::SDL_WINDOW_FULLSCREEN;
    
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    mGameWindow = SDL_CreateWindow(windowCaptain.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,width,height, windowFlags);
    mGLContext = SDL_GL_CreateContext(mGameWindow);
    SDL_GL_MakeCurrent(mGameWindow,mGLContext);

    glewInit();

    int samples;
    glGetIntegerv(GL_SAMPLES, &samples);
    if (samples > 0) {
        std::cout << "Multisampling is enabled with " << samples << " samples per pixel." << std::endl;
    }
    else {
        std::cout << "Multisampling is not supported or not enabled." << std::endl;
    }
   
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &mTextureNumberMax);

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // glEnable(GL_MULTISAMPLE); //开启多重采样

    Log() << "Texture Number Max: " << mTextureNumberMax << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); //(void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(mGameWindow,mGLContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    mDisplay = nullptr;
    mCallback = nullptr;

    SDL_GL_SetSwapInterval(false);//关闭自带的逆天垂直同步
    mEnabledASync = enableASync;

    glViewport(0,0,width,height);
}

sgf::GameAppBase::~GameAppBase()
{
	if (mGameWindow)
		SDL_DestroyWindow(mGameWindow);
    //ImGui_ImplSDL2_Shutdown();
    //ImGui_ImplOpenGL3_Shutdown();
    //ImGui::DestroyContext();

    IMG_Quit();
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
}

void sgf::GameAppBase::EnterMainLoop()
{
    mIsOpen = true;
    mLastSecondBuffer = sgf::TryGetTicks();
    mFramePerSecondBuffer = 0;

    Log() << "Application Running" << std::endl;
    Log() << "--------------------------" << std::endl;

    
    while (mIsOpen)
    {
        SDL_Event _event;
        if (SDL_PollEvent(&_event)) {
            ImGui_ImplSDL2_ProcessEvent(&_event);
            CopeEvent(_event);
            if (mCallback)
                mCallback(this, _event);
        }

        Uint32 tick = sgf::TryGetTicks();

        if (!mMultiThreadUpdate)
        {
            mTick.Update();
        }

        if (tick - mLastSecondBuffer >= 1000) {
            mLastSecondBuffer = tick;
            mFPS = mFramePerSecondBuffer;
            mFramePerSecondBuffer = 0;
        }
        else {
            mFramePerSecondBuffer++;
        }

        gLoopMutex.lock();

        Draw();
        mMessageManager.CopeAllMessage();
        

        gLoopMutex.unlock();

        if (!mMultiThreadUpdate) {
            mMouseStyleEach = MOUSE_STYLE_ARROW;
            Update();
            sgf::SetMouseStyle(mMouseStyleEach);
        }

        static const Uint32 FPS = 1000 / mFPSMax;//可替换为限制的帧速
        static Uint32 _FPS_Timer;
        if (mEnabledASync) {
            if (sgf::TryGetTicks() - _FPS_Timer < FPS) {
                SDL_Delay(FPS - sgf::TryGetTicks() + _FPS_Timer);
            }
            _FPS_Timer = sgf::TryGetTicks();
        }


    }
}

void sgf::GameAppBase::SetDisplayFunction(std::function<void(GameAppBase*, int)>& display)
{
    mDisplay = display;
}

void sgf::GameAppBase::SetCallBackFunction(std::function<void(GameAppBase*, SDL_Event&)>& callback)
{
    mCallback = callback;
}

void sgf::GameAppBase::EnableMultThreadUpdate(std::function<void()> updatethread)
{
    mMultiThreadUpdate = true;
    gUpdateThread = new std::thread(updatethread);
}

void sgf::GameAppBase::CopeEvent(SDL_Event& theEvent)
{
    switch (theEvent.type) {
    case SDL_EventType::SDL_QUIT:
        mIsOpen = false;
        break;
    case SDL_EventType::SDL_WINDOWEVENT:
        if (theEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
            glViewport(0, 0, theEvent.window.data1, theEvent.window.data2);
            mMouseXScale = float(mWidth) / float(theEvent.window.data1);
            mMouseYScale = float(mHeight) / float(theEvent.window.data2);
        }
        break;
    }
}


void sgf::GameAppBase::Update()
{
    auto state = SDL_GetMouseState(&mMouseX,&mMouseY);
    mMouseX *= mMouseXScale;
    mMouseY *= mMouseYScale;

    mIsMouseLeftDown = (state & SDL_BUTTON_LMASK) != 0;
    mIsMouseRightDown = (state & SDL_BUTTON_RMASK) != 0;
    mIsMouseMiddleDown = (state & SDL_BUTTON_MMASK) != 0;
}


void sgf::GameAppBase::Draw()
{

}


