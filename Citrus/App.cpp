#include "App.h"
#pragma warning(disable : 4996)

void App::Init(std::string wndName, std::string className, HINSTANCE hInstance, const int width, const int height)
{
	this->width = width;
	this->height = height;
	timer.Start();
	//Overloaded initialize window
	if (!wnd.InitializeWindow(wndName, className, hInstance, width, height))
	{
		Error::Log("Something happened when initialize the window (overload)");
	}

	if (!gfx.InitializeGraphics(wnd.GetHWND(), width, height))
	{
		Error::Log("Failed to initialize graphics");
	}

	SetSavedValues();
}

void App::Update() noexcept
{
	const float deltaTime = static_cast<float>(timer.GetMilisecondsElapsed());
	timer.Restart();
	//Set any char to any event
	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
	}

	//Set any key to any event
	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();

		if (keycode == VK_ESCAPE) {
			exit(0);
		}
	}
	float cameraSpeed = 0.0005f;
	//Set any mouse event to any event
	while (!mouse.EventBufferIsEmpty())
	{
		MouseEvent me = mouse.ReadEvent();
		if (mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				gfx.cam3D.AdjustRotation(static_cast<float>(me.GetPosY()) * 0.009f, static_cast<float>(me.GetPosX()) * 0.009f, 0);
			}
		}
	}

	if (keyboard.KeyIsPressed(VK_SHIFT))
	{
		cameraSpeed = 0.1f * deltaTime;
	}
	if (keyboard.KeyIsPressed('F'))
	{
		cameraSpeed = 0.001f * deltaTime;
	}
	if (keyboard.KeyIsPressed('W'))
	{
		gfx.cam3D.AdjustPosition(gfx.cam3D.GetForwardVector() * cameraSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('S'))
	{
		gfx.cam3D.AdjustPosition(gfx.cam3D.GetBackwardVector() * cameraSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('A'))
	{
		gfx.cam3D.AdjustPosition(gfx.cam3D.GetLeftVector() * cameraSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('D'))
	{
		gfx.cam3D.AdjustPosition(gfx.cam3D.GetRightVector() * cameraSpeed * deltaTime);
	}
	if (keyboard.KeyIsPressed('Q'))
	{
		gfx.cam3D.AdjustPosition(0.0f, cameraSpeed * deltaTime, 0.0f);
	}
	if (keyboard.KeyIsPressed('E'))
	{
		gfx.cam3D.AdjustPosition(0.0f, -cameraSpeed * deltaTime, 0.0f);
	}

	gfx.pSkyBox.SetPos(gfx.cam3D.GetPositionFloat3(), deltaTime);
}

void App::RenderFrame()
{
	//Render Frame/sec (refresh rate)
	//Draw test triangle all shaded
	gfx.BeginFrame();
	FPSCounter();
	if (!gfx.SceneGraph())
	{
		Error::Log("Something happened to run the test triangle");
	}
	gfx.EndFrame();
}

bool App::ProcessMessages(HINSTANCE hInstance) const noexcept
{
	MSG msg;
	HWND hwnd = wnd.GetHWND();
	ZeroMemory(&msg, sizeof(MSG));
	while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	// Check if the window was closed
	if (msg.message == WM_NULL)
	{
		if (!IsWindow(hwnd))
		{
			pointLightSetting.OpenFileWrite("pointlight_settings.txt");
			pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetAmbientIntensity()));
			pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetDiffuseIntensity()));
			pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetSpecularIntensity()));
			pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetObjectPositionX()));
			pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetObjectPositionY()));
			pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetObjectPositionZ()));
			pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetNormalMapEnabled()));
			pointLightSetting.AddInfo(pPointLightSavedItems);
			pointLightSetting.CloseFile();
			cameraSetting.OpenFileWrite("camera_settings.txt");
			pCameraSavedItems.push_back(std::to_string(gfx.cam3D.GetPositionFloat3().z));
			pCameraSavedItems.push_back(std::to_string(gfx.cam3D.GetPositionFloat3().y));
			pCameraSavedItems.push_back(std::to_string(gfx.cam3D.GetPositionFloat3().x));
			pCameraSavedItems.push_back(std::to_string(gfx.cam3D.GetRotationFloat3().z));
			pCameraSavedItems.push_back(std::to_string(gfx.cam3D.GetRotationFloat3().y));
			pCameraSavedItems.push_back(std::to_string(gfx.cam3D.GetRotationFloat3().x));
			cameraSetting.AddInfo(pCameraSavedItems);
			pointLightSetting.CloseFile();
			hwnd = nullptr; //Message processing loop takes care of destroying this window
			UnregisterClass("janus", hInstance);
			return false;
		}
	}

	return true;
}

void App::SetSavedValues()
{
	pointLightSetting.OpenFileRead("pointlight_settings.txt");
	gfx.pPointLight.SetAmbientIntensity(std::atoi(pointLightSetting.GetInfo(0).c_str()));
	gfx.pPointLight.SetDiffuseIntensity(std::atoi(pointLightSetting.GetInfo(1).c_str()));
	gfx.pPointLight.SetSpecularIntensity(std::atoi(pointLightSetting.GetInfo(2).c_str()));
	gfx.pPointLight.SetObjectPosition(std::atoi(pointLightSetting.GetInfo(3).c_str()), std::atoi(pointLightSetting.GetInfo(4).c_str()),
		std::atoi(pointLightSetting.GetInfo(5).c_str()));
	if (pointLightSetting.GetInfo(6) == "1")
		gfx.pPointLight.SetNormalMapEnabled(TRUE);
	else
		gfx.pPointLight.SetNormalMapEnabled(FALSE);
	pointLightSetting.CloseFile();

	cameraSetting.OpenFileRead("camera_settings.txt");
	gfx.cam3D.SetPosition(std::atoi(cameraSetting.GetInfo(0).c_str()), std::atoi(cameraSetting.GetInfo(1).c_str()),
		std::atoi(cameraSetting.GetInfo(2).c_str()));
	gfx.cam3D.SetRotation(std::atoi(cameraSetting.GetInfo(3).c_str()), std::atoi(cameraSetting.GetInfo(4).c_str()),
		std::atoi(cameraSetting.GetInfo(3).c_str()));
	cameraSetting.CloseFile();
}

void App::FPSCounter()
{
	//fps counter
	static int fpsCounter = 0;
	fpsCounter += 1;
	static std::string fps;
	if (gfx.timer.GetMilisecondsElapsed() > 1000.0f)
	{
		fps = "FPS: " + std::to_string(fpsCounter) + ")";
		fpsCounter = 0;
		gfx.timer.Restart();
	}
	char tempString[17];
	char cpuString[17];
	_itoa_s(gfx.pCPU.GetCpuPercentage(), tempString, 11);
	strcpy_s(cpuString, "CPU Usage: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");
	std::string a = cpuString;
	UI::DeveloperUI(std::to_string(gfx.timer.GetMilisecondsElapsed()), a.c_str() ,fps.c_str(), &gfx.cam3D, GameObject::GetDepthBufferEnabled(), GameObject::GetBlurEnabled(), GameObject::GetWireframeEnabled(),
		GameObject::GetWireColor());
}
