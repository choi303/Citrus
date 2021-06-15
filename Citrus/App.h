#pragma once
#include "WndProc.h"
#include <wrl.h>
#include "SaveSystem.h"
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
	void SetSavedValues();
private:
	void FPSCounter();
	int width;
	int height;
	mutable SaveSystem cameraSetting;
	mutable std::vector<std::string> pCameraSavedItems;
	mutable SaveSystem devMenuSettings;
	mutable std::vector<std::string> pDevMenuSavedItems;
};

