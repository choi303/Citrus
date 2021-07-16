#include "App.h"
#pragma warning(disable : 4996)

void App::Init(const std::string wndName, const std::string className, const HINSTANCE hInstance, const int width, const int height)
{
	this->width = width;
	this->height = height;
	timer.Start();
	//set saved values
	SetSavedValues();
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

	//Set any key to any event
	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();

		if (keycode == VK_ESCAPE) {
			wnd.SetHWND(nullptr);
			SaveValues();
		}
	}
	float camera_speed = 0.0005f;
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
		//increase speed when pressed left shift
		camera_speed = 0.1f * deltaTime;
	}
	if (keyboard.KeyIsPressed('F'))
	{
		//increase speed when pressed F
		camera_speed = 0.001f * deltaTime;
	}
	if (keyboard.KeyIsPressed('W'))
	{
		//move forward when pressed W
		gfx.cam3D.AdjustPosition(gfx.cam3D.GetForwardVector() * camera_speed * deltaTime);
	}
	if (keyboard.KeyIsPressed('S'))
	{
		//move backward when pressed S
		gfx.cam3D.AdjustPosition(gfx.cam3D.GetBackwardVector() * camera_speed * deltaTime);
	}
	if (keyboard.KeyIsPressed('A'))
	{
		//move left when pressed A
		gfx.cam3D.AdjustPosition(gfx.cam3D.GetLeftVector() * camera_speed * deltaTime);
	}
	if (keyboard.KeyIsPressed('D'))
	{
		//move right when pressed D
		gfx.cam3D.AdjustPosition(gfx.cam3D.GetRightVector() * camera_speed * deltaTime);
	}
	if (keyboard.KeyIsPressed('Q'))
	{
		//add up position when pressed Q
		gfx.cam3D.AdjustPosition(0.0f, camera_speed * deltaTime, 0.0f);
	}
	if (keyboard.KeyIsPressed('E'))
	{
		//add down position when pressed E
		gfx.cam3D.AdjustPosition(0.0f, -camera_speed * deltaTime, 0.0f);
	}
	if (keyboard.KeyIsPressed('C'))
	{
		//set point light position to character pos when pressed C
		gfx.pPointLight.SetObjectPosition(gfx.cam3D.GetPositionFloat3().x,
			gfx.cam3D.GetPositionFloat3().y, gfx.cam3D.GetPositionFloat3().z);
	}

	//Set sky box position to camera position
	//that is makes spherical sky box spherical sky box :D
	gfx.pSkyBox.SetPos(gfx.cam3D.GetPositionFloat3(), deltaTime);
}

void App::RenderFrame()
{
	//Render Frame/sec (refresh rate)
	//Draw test triangle all shaded
	gfx.BeginFrame();
	//Run FPS Counter func
	FPSCounter();
	if (!gfx.SceneGraph())
	{
		Error::Log("Something happened to run scene graph");
	}
	//Run end frame func
	gfx.EndFrame();
}

bool App::ProcessMessages(HINSTANCE hInstance) noexcept
{
	//Create msg and hwnd
	MSG msg;
	HWND hwnd = wnd.GetHWND();
	ZeroMemory(&msg, sizeof(MSG));
	//Get Message and dispatch theese messages
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
			SaveValues();

			hwnd = nullptr; //Message processing loop takes care of destroying this window
			UnregisterClass("janus", hInstance);	//Unregister class (closing window)
			return false;	//closing program
		}
	}

	return true;
}

void App::SetSavedValues()
{
	//set camera saved values
	cameraSetting.OpenFileRead("camera_settings.txt");
	gfx.cam3D.SetPosition(float(std::atoi(cameraSetting.GetInfo(0).c_str())), float(std::atoi(cameraSetting.GetInfo(1).c_str())),
		float(std::atoi(cameraSetting.GetInfo(2).c_str())));
	gfx.cam3D.SetRotation(float(std::atoi(cameraSetting.GetInfo(3).c_str())), float(std::atoi(cameraSetting.GetInfo(4).c_str())),
		float(std::atoi(cameraSetting.GetInfo(3).c_str())));
	cameraSetting.CloseFile();

	//set dev menu saved values
	devMenuSettings.OpenFileRead("devmenu_settings.txt");
	if (devMenuSettings.GetInfo(0) == "1")
		GameObject::SetDepthBufferEnabled(true);
	else
		GameObject::SetDepthBufferEnabled(false);

	if (devMenuSettings.GetInfo(1) == "1")
		FSQuad::SetBlurEnabled(true);
	else
		FSQuad::SetBlurEnabled(false);

	FSQuad::SetBlurIntensity(float(std::atoi(devMenuSettings.GetInfo(2).c_str())));

	if (devMenuSettings.GetInfo(3) == "1")
		GameObject::SetFogEnabled(true);
	else
		GameObject::SetFogEnabled(false);

	GameObject::SetFogStart(static_cast<float>(std::atoi(devMenuSettings.GetInfo(4).c_str())));
	GameObject::SetFogEnd(static_cast<float>(std::atoi(devMenuSettings.GetInfo(5).c_str())));

	if (devMenuSettings.GetInfo(6) == "1")
		GameObject::SetWireframeEnabled(true);
	else
		GameObject::SetWireframeEnabled(false);

	if (devMenuSettings.GetInfo(7) == "1")
		GridMap::setRender(true);
	else
		GridMap::setRender(false);
	gfx.msaaQuality = static_cast<UINT>(std::atoi(devMenuSettings.GetInfo(8).c_str()));

	if (devMenuSettings.GetInfo(9) == "1")
		gfx.msaaEnabled = true;
	else
		gfx.msaaEnabled = false;

	//close dev menu file
	devMenuSettings.CloseFile();
}

void App::SaveValues()
{
	//open point light txt, stores point light settings
	gfx.pPointLight.pointLightSetting.OpenFileWrite("pointlight_settings.txt");
	gfx.pPointLight.pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetAmbientIntensity()));
	gfx.pPointLight.pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetDiffuseIntensity()));
	gfx.pPointLight.pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetSpecularIntensity()));
	gfx.pPointLight.pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetReflectionIntensity()));
	gfx.pPointLight.pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetObjectPositionX()));
	gfx.pPointLight.pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetObjectPositionY()));
	gfx.pPointLight.pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetObjectPositionZ()));
	gfx.pPointLight.pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetNormalMapEnabled()));
	gfx.pPointLight.pPointLightSavedItems.push_back(std::to_string(gfx.pPointLight.GetReflectionEnabled()));
	gfx.pPointLight.pointLightSetting.AddInfo(gfx.pPointLight.pPointLightSavedItems);
	gfx.pPointLight.pointLightSetting.CloseFile();
	//open camera txt, stores camera position and rotation data
	cameraSetting.OpenFileWrite("camera_settings.txt");
	pCameraSavedItems.push_back(std::to_string(gfx.cam3D.GetPositionFloat3().z));
	pCameraSavedItems.push_back(std::to_string(gfx.cam3D.GetPositionFloat3().y));
	pCameraSavedItems.push_back(std::to_string(gfx.cam3D.GetPositionFloat3().x));
	pCameraSavedItems.push_back(std::to_string(gfx.cam3D.GetRotationFloat3().z));
	pCameraSavedItems.push_back(std::to_string(gfx.cam3D.GetRotationFloat3().y));
	pCameraSavedItems.push_back(std::to_string(gfx.cam3D.GetRotationFloat3().x));
	cameraSetting.AddInfo(pCameraSavedItems);
	cameraSetting.CloseFile();

	//open dev menu txt, stores dev menu settings
	devMenuSettings.OpenFileWrite("devmenu_settings.txt");
	pDevMenuSavedItems.push_back(std::to_string(*GameObject::GetDepthBufferEnabled()));
	pDevMenuSavedItems.push_back(std::to_string(*FSQuad::GetBlurEnabled()));
	pDevMenuSavedItems.push_back(std::to_string(*FSQuad::GetBlurIntensity()));
	pDevMenuSavedItems.push_back(std::to_string(*GameObject::GetFogEnabled()));
	pDevMenuSavedItems.push_back(std::to_string(*GameObject::GetFogStart()));
	pDevMenuSavedItems.push_back(std::to_string(*GameObject::GetFogEnd()));
	pDevMenuSavedItems.push_back(std::to_string(*GameObject::GetWireframeEnabled()));
	pDevMenuSavedItems.push_back(std::to_string(*GridMap::getRender()));
	pDevMenuSavedItems.push_back(std::to_string(gfx.msaaQuality));
	pDevMenuSavedItems.push_back(std::to_string(gfx.msaaEnabled));
	devMenuSettings.AddInfo(pDevMenuSavedItems);
	devMenuSettings.CloseFile();
}

void App::FPSCounter()
{
	//fps counter
	static int fps_counter = 0;
	fps_counter += 1;
	static std::string fps;
	//set fps text value
	if (gfx.timer.GetMilisecondsElapsed() > 1000.0f)
	{
		fps = "FPS: " + std::to_string(fps_counter);
		fps_counter = 0;
		gfx.timer.Restart();
	}
	char temp_string[19];
	char cpu_string[19];
	//Print cpu usage data
	_itoa_s(gfx.pCPU.GetCpuPercentage(), temp_string, 15);
	strcpy_s(cpu_string, "CPU Usage: %%");
	strcat_s(cpu_string, temp_string);
	const std::string cpu_usage_string = cpu_string;
	//print video card name (adapter name)
	WCHAR* adapter_name_wchar = gfx.GetAdapterDesc().Description;
	auto adapter_name = std::wstring(adapter_name_wchar);
	//dev menu creation
	UI::DeveloperUI(std::string(adapter_name.begin(), adapter_name.end()),cpu_usage_string.c_str() ,fps.c_str(), &gfx.cam3D, GameObject::GetWireframeEnabled(),
		GameObject::GetWireColor(), GameObject::GetFogEnabled(), GameObject::GetFogColor(), GameObject::GetFogStart(),
		GameObject::GetFogEnd(), &gfx.vsync, GridMap::getRender(),
		GridMap::getColor(), &gfx, wnd.GetHWND(), this, &gfx.msaaEnabled, FSQuad::GetBlurEnabled(), 
		FSQuad::GetBlurIntensity());
	//toolbar creation
	UI::ToolBar(GridMap::getRender(),
		GameObject::GetWireframeEnabled(),
		GameObject::GetFogEnabled(), 
		GameObject::GetDepthBufferEnabled(),
		FSQuad::GetBlurEnabled(), &gfx.msaaEnabled, this);
}