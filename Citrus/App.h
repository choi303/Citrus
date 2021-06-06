#pragma once
#include "WndProc.h"
#include <wrl.h>
namespace wrl = Microsoft::WRL;

class App : WndProc
{
public:
	void Init(std::string wndName, std::string className, HINSTANCE hInstance, const int width, const int height);
	App() = default;
	App(const App&) = default;
	~App() = default;
public:
	void Update() noexcept; // this function work every second
	void RenderFrame();
	bool ProcessMessages(HINSTANCE hInstance) const noexcept;
private:
	void FPSCounter();
};

