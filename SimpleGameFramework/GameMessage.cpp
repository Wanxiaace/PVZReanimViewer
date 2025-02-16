#include "GameMessage.h"
#include "WidgetSystem.h"

std::mutex gMessageMutex;

void sgf::GameMessageManager::SendGameMessage(GameMessage src)
{
	gMessageMutex.lock();
	mMessages.push_back(src);
	gMessageMutex.unlock();
}

void sgf::GameMessageManager::CopeAllMessage()
{
	size_t length = mMessages.size();
	for (size_t i = 0; i < length; i++)
	{
		switch (mMessages[i].mMessageType)
		{
		case sgf::GameMessage::MessageType::MSG_TYPE_DELETE_WIDGET:
		{
			Widget* widget = (Widget*)mMessages[i].mParamPointer2;
			((WidgetManager*)mMessages[i].mParamPointer1)->RemoveWidget(widget);
			delete widget;
			break;
		}
		case sgf::GameMessage::MessageType::MSG_TYPE_APPEND_WIDGET:
		{
			Widget* widget = (Widget*)mMessages[i].mParamPointer2;
			((WidgetManager*)mMessages[i].mParamPointer1)->AddWidget(widget);
			break;
		}
		case sgf::GameMessage::MessageType::MSG_TYPE_SCREEN_SHOT: {
			GameApp* app = static_cast<GameApp*>(mMessages[i].mParamPointer1);
			auto screenShot = app->mGraphics->CaptureScreen(0, 0, app->mWidth, app->mHeight);
			screenShot->DumpToFiles("ScreenShot.png");
			delete screenShot;
			break;
		}
		case sgf::GameMessage::MessageType::MSG_TYPE_FULL_SCREEN: {
			GameApp* app = static_cast<GameApp*>(mMessages[i].mParamPointer1);
			SDL_Rect screenRect = {};
			SDL_GetDisplayBounds(0,&screenRect);
			SDL_SetWindowSize(app->mGameWindow,screenRect.w, screenRect.h);
			SDL_SetWindowPosition(app->mGameWindow,0,0);
			glViewport(0, 0, screenRect.w, screenRect.h);
			app->mMouseXScale = float(app->mWidth) / float(screenRect.w);
			app->mMouseYScale = float(app->mHeight) / float(screenRect.h);
			break;
		}
		case sgf::GameMessage::MessageType::MSG_TYPE_EXIT_FULL_SCREEN: {
			GameApp* app = static_cast<GameApp*>(mMessages[i].mParamPointer1);

			SDL_SetWindowSize(app->mGameWindow, app->mWidth, app->mHeight);
			SDL_SetWindowPosition(app->mGameWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
			glViewport(0, 0, app->mWidth, app->mHeight);
			
			app->mMouseXScale = 1.0f;
			app->mMouseYScale = 1.0f;
			break;
		}
		case sgf::GameMessage::MessageType::MSG_TYPE_DO_LATER:
		{
			GameApp* app = static_cast<GameApp*>(mMessages[i].mParamPointer1);
			std::function<void()>* func = (std::function<void()>*)(mMessages[i].mParamPointer2);
			(*func)();
			delete func;
			break;
		}
		default:
			break;
		}
	}
	mMessages.clear();
}

void sgf::GameMessageManager::CopeMessage(int index)
{
}

sgf::GameMessageManager::GameMessageManager()
{
}

sgf::GameMessageManager::~GameMessageManager()
{
}
