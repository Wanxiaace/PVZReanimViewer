#include "GameApp.h"
sgf::GameApp* sgf::gGameApp = nullptr;


sgf::GameApp::GameApp(int width, int height, const sgf::String& windowCaptain, bool enabledASync,bool resiziable):
	GameAppBase::GameAppBase(width, height, windowCaptain, enabledASync, resiziable)
{
	mWidgetManager = new WidgetManager();
	gGameApp = this;
}

sgf::GameApp::~GameApp()
{
	delete mGraphics;
	delete mWidgetManager;

	//GameAppBase::~GameAppBase();
}

void sgf::GameApp::UpdateMusicVolume()
{
	mMusicManager.SetMusicVolume(mMusicVolume / 100.0f);
}

void sgf::GameApp::UpdateSoundVolume()
{
	mMusicManager.SetChannelVolume(-1, mSoundVolume / 100.0f);
}

sgf::Graphics* sgf::GameApp::LoadGraphics()
{
	if(!mGraphics)
		mGraphics = new Graphics(this);
	return mGraphics;
}

float sgf::GameApp::GetMouseWheelY()
{
	return mMouseWheelY;
}



void sgf::GameApp::SafeDeleteWidget(Widget* target)
{
	target->mVisible = false;
	mMessageManager.SendGameMessage({ sgf::GameMessage::MessageType::MSG_TYPE_DELETE_WIDGET,mWidgetManager,target});
}

void sgf::GameApp::SafeAppendWidget(Widget* target)
{
	mMessageManager.SendGameMessage({ sgf::GameMessage::MessageType::MSG_TYPE_APPEND_WIDGET,mWidgetManager,target });
}

void sgf::GameApp::DrawImgui()
{
	if (!mShowDebugWindow)
		return;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Debug Tools")) {
			ImGui::Checkbox("Analyze Window", &mShowAnalyzeWindow);
			ImGui::EndMenu();
		}
		ImGui::Text("FPS: %d",mFPS);
		ImGui::EndMainMenuBar();
	}

	if (mShowAnalyzeWindow) {
		ImGui::Begin("Analyse Window");
		ImGui::Text("FPS: %d", mFPS);
		ImGui::Text("MouseX: %d", mMouseX);
		ImGui::Text("MouseY: %d", mMouseY);
		ImGui::Checkbox("Allow Moving", &mDragAllowed);
		ImGui::Checkbox("Show Widget Box", &gShowWidgetHitBoxAllowed);
		ImGui::Checkbox("Rotate", &mRotateAllowed);
		ImGui::SliderFloat("Moving Speed", &mMovingSpeed, 1.0f, 5.0f);
		ImGui::SliderFloat("Render Scale", &mRenderScale, 0.5f, 10.0f);
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void sgf::GameApp::LoadResources(const char* resourcesListPath)
{
	mResourceList.Load(resourcesListPath);
	mResourceManager.LoadFromResouceList(&mResourceList, &mMusicManager);
	UpdateMusicVolume();
	UpdateSoundVolume();

}

void sgf::GameApp::LoadDict(const char* dictPath)
{
	mDictionary.LoadFromFile((mResourceManager.mBasePath + dictPath).c_str());
}

void sgf::GameApp::SetWindowIcon(const char* path)
{
	sgf::SimpleImage icon;
	icon.LoadFromFile(path);
	SetWindowIconFromImage(&icon);
}

void sgf::GameApp::SetWindowIconFromImage(sgf::SimpleImage* image)
{
	SDL_SetWindowIcon(mGameWindow, image->mSurface);
}

void sgf::GameApp::SetWindowCaptain(const sgf::String& title)
{
	SDL_SetWindowTitle(mGameWindow, title.c_str());
}

void sgf::GameApp::ScreenShot(const sgf::String& outPath)
{
	GameMessage srceenShotMessage = {};
	srceenShotMessage.mMessageType = sgf::GameMessage::MessageType::MSG_TYPE_SCREEN_SHOT;
	srceenShotMessage.mParamPointer1 = this;
	mMessageManager.SendGameMessage(srceenShotMessage);

}

void sgf::GameApp::UseFullScreen()
{
	GameMessage fullMessage = {};
	fullMessage.mMessageType = sgf::GameMessage::MessageType::MSG_TYPE_FULL_SCREEN;
	fullMessage.mParamPointer1 = this;
	mMessageManager.SendGameMessage(fullMessage);
}

void sgf::GameApp::ExitFullScreen()
{
	GameMessage exitFullMessage = {};
	exitFullMessage.mMessageType = sgf::GameMessage::MessageType::MSG_TYPE_EXIT_FULL_SCREEN;
	exitFullMessage.mParamPointer1 = this;
	mMessageManager.SendGameMessage(exitFullMessage);
}

void sgf::GameApp::DoInGraphicsThread(std::function<void()>* func)
{
	GameMessage dolaterMessage = {};
	dolaterMessage.mMessageType = sgf::GameMessage::MessageType::MSG_TYPE_DO_LATER;
	dolaterMessage.mParamPointer1 = this;
	dolaterMessage.mParamPointer2 = func;
	mMessageManager.SendGameMessage(dolaterMessage);
}

void sgf::GameApp::LockUpdate()
{
	gLoopMutex.lock();
}

void sgf::GameApp::UnLockUpdate()
{
	gLoopMutex.unlock();
}

void sgf::GameApp::ShowHandCursor()
{
	mMouseStyleEach = MOUSE_STYLE_HAND;
}


void sgf::GameApp::Update()
{
	GameAppBase::Update();
	mWidgetManager->Update(this);

	if (mMouseWheelY != 0)
		mMouseWheelY = 0;

	if (mDragAllowed) {
		const unsigned char* keyStates = SDL_GetKeyboardState(NULL);
		if(keyStates[SDL_SCANCODE_W])
			mViewMatrixBuf = glm::translate(mViewMatrixBuf,glm::vec3(0, mMovingSpeed,0));
		if (keyStates[SDL_SCANCODE_S])
			mViewMatrixBuf = glm::translate(mViewMatrixBuf, glm::vec3(0, -mMovingSpeed, 0));
		if (keyStates[SDL_SCANCODE_A])
			mViewMatrixBuf = glm::translate(mViewMatrixBuf, glm::vec3(mMovingSpeed, 0, 0));
		if (keyStates[SDL_SCANCODE_D])
			mViewMatrixBuf = glm::translate(mViewMatrixBuf, glm::vec3(-mMovingSpeed, 0, 0));
	}

	if (mRotateAllowed) {
		mViewMatrixBuf = glm::rotate(mViewMatrix, 0.05f, glm::vec3(0, 0, 1.0f));
	}
	
	mViewMatrix = glm::scale(mViewMatrixBuf, glm::vec3(mRenderScale, mRenderScale, 1.0f));
}

void sgf::GameApp::Draw()
{
	mGraphics->Clear();

	mGraphics->ActiveTextureShader();
	mWidgetManager->Draw(mGraphics);

	if (mDisplay)
		mDisplay(this, mTick.GetDeltaTick());

	DrawImgui();
	SDL_GL_SwapWindow(mGameWindow);
}

void sgf::GameApp::CopeEvent(SDL_Event& theEvent)
{
	GameAppBase::CopeEvent(theEvent);

	switch (theEvent.type) {
	case SDL_EventType::SDL_WINDOWEVENT:
		break;
	case SDL_EventType::SDL_MOUSEWHEEL: {
		mMouseWheelY = theEvent.wheel.preciseY;
		//std::cout << mMouseWheelY << std::endl;
		break;
	}
	}
}
