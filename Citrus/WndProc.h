#pragma once
#include "ChoiWin.h"
#include "KeyboardClass.h"
#include "MouseClass.h"
#include "Timer.h"
#include "Graphics.h"

class WndProc
{
public:
	WndProc();
	LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
	ChoiWin wnd;
	KeyboardClass keyboard;
	MouseClass mouse;
	Graphics gfx;
	Timer timer;
};