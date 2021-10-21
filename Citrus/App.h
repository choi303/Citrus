#pragma once
#include "WndProc.h"
#include <wrl.h>
#include "SaveSystem.h"
namespace wrl = Microsoft::WRL;

class App : WndProc
{
public:
	void Init(const std::string wndName, const std::string className, const HINSTANCE hInstance, const int width, const int height,
		double& version);
	App() = default;
	App(const App&) = default;
	~App() = default;
public:
	void Update() noexcept; // this function work every second
	void RenderFrame();
	bool ProcessMessages(HINSTANCE hInstance) noexcept;
	void SetSavedValues();
	void SetSavedValues2();
	void SaveValues();
private:
	void FPSCounter();
	int width;
	int height;
	mutable SaveSystem cameraSetting;
	mutable std::vector<std::string> pCameraSavedItems;
	mutable SaveSystem devMenuSettings;
	mutable std::vector<std::string> pDevMenuSavedItems;
	mutable SaveSystem fxMenuSettings;
	mutable std::vector<std::string> pFxMenuSavedItems;
	mutable SaveSystem fireMenuSettings;
	mutable std::vector<std::string> pFireMenuSavedItems;
	mutable SaveSystem settings;
	mutable std::vector<std::string> pSettings;
	mutable SaveSystem paths;
	mutable std::vector<std::string> pPaths;
	std::string versionStr;
};

