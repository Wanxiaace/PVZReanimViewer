#include "GameApp.h"
#include "Constant.h"
#include "Graphics.h"
#include "ReanimViewer.h"

#include "MainScreen.h"

#undef main

ViewerApp* gApp;

sgf::Graphics* gGraphics;

int main() {
	gApp = new ViewerApp();
	gGraphics = gApp->LoadGraphics();


	gApp->EnterMainLoop();
	
	delete gApp;
	return EXIT_SUCCESS;
}

