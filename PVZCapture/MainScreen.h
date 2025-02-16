#ifndef __CAPTRUE_MAIN_SCREEN__
#define __CAPTRUE_MAIN_SCREEN__

#include "WidgetSystem.h"
#include "GameApp.h"

constexpr int MAIN_SCREEN_ID = 0;

class MainScreen: public sgf::Widget {
public:

public:
	MainScreen();
	~MainScreen();

public:
	virtual void Draw(sgf::Graphics* g);
};

#endif // !__CAPTRUE_MAIN_SCREEN__
