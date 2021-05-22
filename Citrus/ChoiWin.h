#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include "Error.h"

class ChoiWin
{
public:
	ChoiWin() = default;
	bool InitializeWindow(std::string wndName, std::string className, HINSTANCE hInstance, const int width, const int height) noexcept;
	HWND GetHWND() const noexcept;
	~ChoiWin()=default;
private:
	HWND hwnd = NULL;
	HINSTANCE hInstance = NULL;

};

