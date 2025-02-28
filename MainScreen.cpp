#include "MainScreen.h"
#include "Constant.h"

MainScreen::MainScreen()
	: Widget(MAIN_SCREEN_ID)
{
	Resize(0, 0, GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT);
}

MainScreen::~MainScreen()
{

}

void MainScreen::Draw(sgf::Graphics* g)
{

}
