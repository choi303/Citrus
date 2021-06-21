#pragma once
#include <windows.h>
#include <string>

class ChoiWin
{
public:
	ChoiWin() = default;
	bool InitializeWindow(std::string wndName, std::string className, HINSTANCE hInstance, const int width, const int height) noexcept;
	HWND GetHWND() const noexcept;
	HWND SetHWND(HWND hwnd) noexcept;
	~ChoiWin()=default;
private:
	HWND hwnd = nullptr;
	HINSTANCE hInstance = nullptr;

};

