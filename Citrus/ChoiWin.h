#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include "Error.h"
#include "Graphics.h"

class ChoiWin
{
public:
	ChoiWin() = default;
	~ChoiWin()=default;
	bool InitializeWindow(std::string wndName, std::string className, HINSTANCE hInstance, const int width, const int height) noexcept;
	bool InitializeGraphics(const int width, const int height);
	HWND GetHWND() const noexcept;
	std::unique_ptr<Graphics> gfx;
private:
	HWND hwnd = NULL;
	HINSTANCE hInstance = NULL;

};

