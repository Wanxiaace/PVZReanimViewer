#pragma once
#ifndef __SIMPLE_GAME_MESSAGE__
#define __SIMPLE_GAME_MESSAGE__

#include <vector>

namespace sgf {
	class GameMessage {
	public:
		enum MessageType {
			MSG_TYPE_NONE,
			MSG_TYPE_DELETE_WIDGET,
			MSG_TYPE_APPEND_WIDGET,
			MSG_TYPE_SCREEN_SHOT,
			MSG_TYPE_FULL_SCREEN,
			MSG_TYPE_EXIT_FULL_SCREEN,
			MSG_TYPE_DO_LATER,
		};
	public:
		MessageType mMessageType;
		void* mParamPointer1;
		void* mParamPointer2;
		int mParam32;
		long long mParam64;
	};

	class GameMessageManager {
	public:
		std::vector<GameMessage> mMessages;

	public:
		void SendGameMessage(GameMessage src);
		void CopeAllMessage();
		void CopeMessage(int index);
		GameMessage PeekMessage(int index) { return mMessages[index]; };

		GameMessageManager();
		~GameMessageManager();
	};
}

#endif // !__SIMPLE_GAME_MESSAGE__
