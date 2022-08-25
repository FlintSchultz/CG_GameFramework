#pragma once

#include "export.h"
#include "InputDevice.h"

class DisplayWin32 {

private:
	LPCWSTR applicationName;
	HINSTANCE hInstance;
	HWND hWnd;
	WNDCLASSEX wc;

	int screenWidth;
	int screenHeight;
	int posX;
	int posY;

	long dwStyle;

public:
	HWND getHWND();

	int getScreenWidth();
	int getScreenHeight();

	void CreateDisplay(InputDevice* iDev);
	void ShowClientWindow(InputDevice* iDev);
	void OnChangeScreenSize(const ScreenSize& args);
};