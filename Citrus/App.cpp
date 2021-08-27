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
	if (keyboard.KeyIsPressed('H'))
	{
		//set ui visiblity to false
		UI::SetVisiblity(false);
	}
	if (keyboard.KeyIsPressed('V'))
	{
		//set ui visiblity to true
		UI::SetVisiblity(true);
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
	gfx.Render();
	//Run FPS Counter func
	FPSCounter();
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
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
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
	cameraSetting.OpenFileRead("camera_settings.cfg");
	gfx.cam3D.SetPosition(std::stof(cameraSetting.GetInfo(0).c_str()), std::stof(cameraSetting.GetInfo(1).c_str()),
		std::stof(cameraSetting.GetInfo(2).c_str()));
	gfx.cam3D.SetRotation(std::stof(cameraSetting.GetInfo(3).c_str()), std::stof(cameraSetting.GetInfo(4).c_str()),
		std::stof(cameraSetting.GetInfo(3).c_str()));
	cameraSetting.CloseFile();

	//set dev menu saved values
	devMenuSettings.OpenFileRead("devmenu_settings.cfg");
	if (devMenuSettings.GetInfo(0) == "1")
		GameObject::SetDepthBufferEnabled(true);
	else
		GameObject::SetDepthBufferEnabled(false);

	if (devMenuSettings.GetInfo(1) == "1")
		FSQuad::SetBlurEnabled(true);
	else
		FSQuad::SetBlurEnabled(false);

	FSQuad::SetBlurIntensity(std::stof(devMenuSettings.GetInfo(2).c_str()));

	if (devMenuSettings.GetInfo(3) == "1")
		GameObject::SetFogEnabled(true);
	else
		GameObject::SetFogEnabled(false);

	GameObject::SetFogStart(std::stof(devMenuSettings.GetInfo(4).c_str()));
	GameObject::SetFogEnd(std::stof(devMenuSettings.GetInfo(5).c_str()));

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

	if (devMenuSettings.GetInfo(10) == "1")
		FSQuad::SetFxaaEnabled(true);
	else
		FSQuad::SetFxaaEnabled(false);

	if (devMenuSettings.GetInfo(11) == "1")
		FSQuad::SetSSAOEnabled(TRUE);
	else
		FSQuad::SetSSAOEnabled(FALSE);

	FSQuad::SetArea(std::stof(devMenuSettings.GetInfo(12).c_str()));
	FSQuad::SetBase(std::stof(devMenuSettings.GetInfo(13).c_str()));
	FSQuad::SetRadius(std::stof(devMenuSettings.GetInfo(14).c_str()));
	FSQuad::SetTotalStrength(std::stof(devMenuSettings.GetInfo(15).c_str()));
	FSQuad::SetExposure(std::stof(devMenuSettings.GetInfo(16).c_str()));
	FSQuad::SetGamma(std::stof(devMenuSettings.GetInfo(17).c_str()));
	if (devMenuSettings.GetInfo(18) == "1")
		FSQuad::SetToneMappingEnabled(TRUE);
	else
		FSQuad::SetToneMappingEnabled(FALSE);

	if (devMenuSettings.GetInfo(19) == "1")
		FSQuad::SetBloomEnabled(TRUE);
	else
		FSQuad::SetBloomEnabled(FALSE);
	
	FSQuad::SetBloomIntensity(std::stof(devMenuSettings.GetInfo(20).c_str()));

	//close dev menu file
	devMenuSettings.CloseFile();

	//set fx menu saved values
	fxMenuSettings.OpenFileRead("fx_menu_settings.cfg");
	Particle::SetDeviationX(std::stof(fxMenuSettings.GetInfo(0)));
	Particle::SetDeviationY(std::stof(fxMenuSettings.GetInfo(1)));
	Particle::SetDeviationZ(std::stof(fxMenuSettings.GetInfo(2)));
	Particle::SetParticleVelocity(std::stof(fxMenuSettings.GetInfo(3)));
	Particle::SetParticleVelocityVariation(std::stof(fxMenuSettings.GetInfo(4)));
	Particle::SetParticleSize(std::stof(fxMenuSettings.GetInfo(5)));
	Particle::SetParticleMax(std::atoi(fxMenuSettings.GetInfo(6).c_str()));
	Particle::SetParticlesPerSecond(std::stof(fxMenuSettings.GetInfo(7)));
	Particle::SetLifeTime(std::stof(fxMenuSettings.GetInfo(8)));
	if (fxMenuSettings.GetInfo(9) == "1")
		Particle::SetIsLifeTime(true);
	else
		Particle::SetIsLifeTime(false);
	Particle::SetPosX(std::stof(fxMenuSettings.GetInfo(10)));
	Particle::SetPosY(std::stof(fxMenuSettings.GetInfo(11)));
	Particle::SetPosZ(std::stof(fxMenuSettings.GetInfo(12)));
	fxMenuSettings.CloseFile();

	//set fire menu saved values
	fireMenuSettings.OpenFileRead("fire_menu_settings.cfg");
	Fire::SetPosX(std::stof(fireMenuSettings.GetInfo(0)));
	Fire::SetPosY(std::stof(fireMenuSettings.GetInfo(1)));
	Fire::SetPosZ(std::stof(fireMenuSettings.GetInfo(2)));
	Fire::SetScaleX(std::stof(fireMenuSettings.GetInfo(3)));
	Fire::SetScaleY(std::stof(fireMenuSettings.GetInfo(4)));
	Fire::SetScaleZ(std::stof(fireMenuSettings.GetInfo(5)));
	Fire::SetDistortion1X(std::stof(fireMenuSettings.GetInfo(6)));
	Fire::SetDistortion1Y(std::stof(fireMenuSettings.GetInfo(7)));
	Fire::SetDistortion2X(std::stof(fireMenuSettings.GetInfo(8)));
	Fire::SetDistortion2Y(std::stof(fireMenuSettings.GetInfo(9)));
	Fire::SetDistortion3X(std::stof(fireMenuSettings.GetInfo(10)));
	Fire::SetDistortion3Y(std::stof(fireMenuSettings.GetInfo(11)));
	Fire::SetScalesX(std::stof(fireMenuSettings.GetInfo(12)));
	Fire::SetScalesY(std::stof(fireMenuSettings.GetInfo(13)));
	Fire::SetScalesZ(std::stof(fireMenuSettings.GetInfo(14)));
	Fire::SetSSpeedX(std::stof(fireMenuSettings.GetInfo(15)));
	Fire::SetSSpeedY(std::stof(fireMenuSettings.GetInfo(16)));
	Fire::SetSSpeedZ(std::stof(fireMenuSettings.GetInfo(17)));
	Fire::SetDistortionScale(std::stof(fireMenuSettings.GetInfo(18)));
	Fire::SetDistortionBias(std::stof(fireMenuSettings.GetInfo(19)));
	fireMenuSettings.CloseFile();
}

void App::SaveValues()
{
	//Directional Light
	gfx.pDirectLight->directLightSettings.OpenFileWrite("directional_light_settings.cfg");
	gfx.pDirectLight->pDirectLightSavedItems.push_back("[Light Direction Z]:" + std::to_string(
		gfx.pDirectLight->GetDirection().z));
	gfx.pDirectLight->pDirectLightSavedItems.push_back("[Light Direction Y]:" + std::to_string(
		gfx.pDirectLight->GetDirection().y));
	gfx.pDirectLight->pDirectLightSavedItems.push_back("[Light Direction X]:" + std::to_string(
		gfx.pDirectLight->GetDirection().x));
	gfx.pDirectLight->pDirectLightSavedItems.push_back("[Ambient Intensity]:" + std::to_string(
		gfx.pDirectLight->GetAmbientIntensity()));
	gfx.pDirectLight->pDirectLightSavedItems.push_back("[Diffuse Intensity]:" + std::to_string(
		gfx.pDirectLight->GetDiffuseIntensity()));
	gfx.pDirectLight->pDirectLightSavedItems.push_back("[Reflection Intensity]:" + std::to_string(
		gfx.pDirectLight->GetReflectionIntensity()));
	gfx.pDirectLight->pDirectLightSavedItems.push_back("[Specular Intensity]:" + std::to_string(
		gfx.pDirectLight->GetSpecularIntensity()));
	gfx.pDirectLight->pDirectLightSavedItems.push_back("[Shadow Bias]:" + std::to_string(
		gfx.pDirectLight->GetBias()));
	gfx.pDirectLight->pDirectLightSavedItems.push_back("[Normal Map Enabled]:" + std::to_string(
		gfx.pDirectLight->GetNormalMapEnabled()));
	gfx.pDirectLight->pDirectLightSavedItems.push_back("[Reflection Enabled]:" + std::to_string(
		gfx.pDirectLight->GetReflectionEnabled()));
	gfx.pDirectLight->pDirectLightSavedItems.push_back("[PCF Filter Enabled]:" + std::to_string(
		gfx.pDirectLight->GetPCFEnabled()));
	gfx.pDirectLight->pDirectLightSavedItems.push_back("[Alpha Clip Enabled]:" + std::to_string(
		*gfx.pDirectLight->GetAlphaCEnabled()));
	gfx.pDirectLight->pDirectLightSavedItems.push_back("[Emessive Map Enabled]:" + std::to_string(
		*gfx.pDirectLight->GetEmessiveEnabled()));
	gfx.pDirectLight->pDirectLightSavedItems.push_back("[Emessive Intensity]:" + std::to_string(
		gfx.pDirectLight->GetEmessiveIntensity()));
	gfx.pDirectLight->directLightSettings.AddInfo(
		gfx.pDirectLight->pDirectLightSavedItems);
	gfx.pDirectLight->directLightSettings.CloseFile();

	//open camera txt, stores camera position and rotation data
	cameraSetting.OpenFileWrite("camera_settings.cfg");
	pCameraSavedItems.push_back("[Camera Position Z]:" + std::to_string(
		gfx.cam3D.GetPositionFloat3().z));
	pCameraSavedItems.push_back("[Camera Position Y]:" + std::to_string(
		gfx.cam3D.GetPositionFloat3().y));
	pCameraSavedItems.push_back("[Camera Position X]:" + std::to_string(
		gfx.cam3D.GetPositionFloat3().x));
	pCameraSavedItems.push_back("[Camera Rotation Z]:" + std::to_string(
		gfx.cam3D.GetRotationFloat3().z));
	pCameraSavedItems.push_back("[Camera Rotation Y]:" + std::to_string(
		gfx.cam3D.GetRotationFloat3().y));
	pCameraSavedItems.push_back("[Camera Rotation X]:" + std::to_string(
		gfx.cam3D.GetRotationFloat3().x));
	cameraSetting.AddInfo(pCameraSavedItems);
	cameraSetting.CloseFile();

	//open dev menu txt, stores dev menu settings
	devMenuSettings.OpenFileWrite("devmenu_settings.cfg");
	pDevMenuSavedItems.push_back("[Depth Buffer Enabled]:" + std::to_string(
		*GameObject::GetDepthBufferEnabled()));
	pDevMenuSavedItems.push_back("[Blur Enabled]:" + std::to_string(
		*FSQuad::GetBlurEnabled()));
	pDevMenuSavedItems.push_back("[Blur Intensity]:" + std::to_string(
		*FSQuad::GetBlurIntensity()));
	pDevMenuSavedItems.push_back("[Fog Enabled]:" + std::to_string(
		*GameObject::GetFogEnabled()));
	pDevMenuSavedItems.push_back("[Fog Start]:" + std::to_string(
		*GameObject::GetFogStart()));
	pDevMenuSavedItems.push_back("[Fog End]:" + std::to_string(
		*GameObject::GetFogEnd()));
	pDevMenuSavedItems.push_back("[Wireframe Enabled]:" + std::to_string(
		*GameObject::GetWireframeEnabled()));
	pDevMenuSavedItems.push_back("[Grid Map Enabled]:" + std::to_string(
		*GridMap::getRender()));
	pDevMenuSavedItems.push_back("[MSAA Quality]:" + std::to_string(
		gfx.msaaQuality));
	pDevMenuSavedItems.push_back("[MSAA Enabled]:" + std::to_string(
		gfx.msaaEnabled));
	pDevMenuSavedItems.push_back("[FXAA Enabled]:" + std::to_string(
		*FSQuad::GetFxaaEnabled()));
	pDevMenuSavedItems.push_back("[SSAO Enabled]:" + std::to_string(
		*FSQuad::GetSSAOEnabled()));
	pDevMenuSavedItems.push_back("[SSAO Area]:" + std::to_string(
		*FSQuad::GetArea()));
	pDevMenuSavedItems.push_back("[SSAO Base]:" + std::to_string(
		*FSQuad::GetBase()));
	pDevMenuSavedItems.push_back("[SSAO Radius]:" + std::to_string(
		*FSQuad::GetRadius()));
	pDevMenuSavedItems.push_back("[SSAO Total Strength]:" + std::to_string(
		*FSQuad::GetTotalStrength()));
	pDevMenuSavedItems.push_back("[Exposure]:" + std::to_string(
		*FSQuad::GetExposure()));
	pDevMenuSavedItems.push_back("[Gamma]:" + std::to_string(
		*FSQuad::GetGamma()));
	pDevMenuSavedItems.push_back("[Tone Mapping Enabled]:" + std::to_string(
		*FSQuad::GetToneMappingEnabled()));
	pDevMenuSavedItems.push_back("[Bloom Enabled]:" + std::to_string(
		*FSQuad::GetBloomEnabled()));
	pDevMenuSavedItems.push_back("[Bloom Intensity]:" + std::to_string(
		*FSQuad::GetBloomIntensity()));
	devMenuSettings.AddInfo(pDevMenuSavedItems);
	devMenuSettings.CloseFile();

	//open fx menu txt, stores fx menu settings
	fxMenuSettings.OpenFileWrite("fx_menu_settings.cfg");
	pFxMenuSavedItems.push_back("[Particle Deviation X]:" + std::to_string(*Particle::GetDeviationX()));
	pFxMenuSavedItems.push_back("[Particle Deviation Y]:" + std::to_string(*Particle::GetDeviationY()));
	pFxMenuSavedItems.push_back("[Particle Deviation Z]:" + std::to_string(*Particle::GetDeviationZ()));
	pFxMenuSavedItems.push_back("[Particle Velocity]:" + std::to_string(*Particle::GetParticleVelocity()));
	pFxMenuSavedItems.push_back("[Particle Velocity Variation]:" + std::to_string(*Particle::GetParticleVelocityVariation()));
	pFxMenuSavedItems.push_back("[Particle Size]:" + std::to_string(*Particle::GetParticleSize()));
	pFxMenuSavedItems.push_back("[Particle Max]:" + std::to_string(*Particle::GetParticleMax()));
	pFxMenuSavedItems.push_back("[Particle Per Second]:" + std::to_string(*Particle::GetParticlesPerSecond()));
	pFxMenuSavedItems.push_back("[Particle Life Time]:" + std::to_string(*Particle::GetLifeTime()));
	pFxMenuSavedItems.push_back("[Particle Life Time Enabled]:" + std::to_string(*Particle::GetIsLifeTime()));
	pFxMenuSavedItems.push_back("[Particle Position X]:" + std::to_string(*Particle::GetPosX()));
	pFxMenuSavedItems.push_back("[Particle Position Y]:" + std::to_string(*Particle::GetPosY()));
	pFxMenuSavedItems.push_back("[Particle Position Z]:" + std::to_string(*Particle::GetPosZ()));
	fxMenuSettings.AddInfo(pFxMenuSavedItems);
	fxMenuSettings.CloseFile();

	//open fire menu txt, stores fire menu settings
	fireMenuSettings.OpenFileWrite("fire_menu_settings.cfg");
	pFireMenuSavedItems.push_back("[Fire Position X]:" + std::to_string(*Fire::GetPosX()));
	pFireMenuSavedItems.push_back("[Fire Position Y]:" + std::to_string(*Fire::GetPosY()));
	pFireMenuSavedItems.push_back("[Fire Position Z]:" + std::to_string(*Fire::GetPosZ()));
	pFireMenuSavedItems.push_back("[Fire Scale X]:" + std::to_string(*Fire::GetScaleX()));
	pFireMenuSavedItems.push_back("[Fire Scale Y]:" + std::to_string(*Fire::GetScaleY()));
	pFireMenuSavedItems.push_back("[Fire Scale Z]:" + std::to_string(*Fire::GetScaleZ()));
	pFireMenuSavedItems.push_back("[Fire Distortion 1 X]:" + std::to_string(*Fire::GetDistortion1X()));
	pFireMenuSavedItems.push_back("[Fire Distortion 1 Y]:" + std::to_string(*Fire::GetDistortion1Y()));
	pFireMenuSavedItems.push_back("[Fire Distortion 2 X]:" + std::to_string(*Fire::GetDistortion2X()));
	pFireMenuSavedItems.push_back("[Fire Distortion 2 Y]:" + std::to_string(*Fire::GetDistortion2Y()));
	pFireMenuSavedItems.push_back("[Fire Distortion 3 X]:" + std::to_string(*Fire::GetDistortion3X()));
	pFireMenuSavedItems.push_back("[Fire Distortion 3 Y]:" + std::to_string(*Fire::GetDistortion3Y()));
	pFireMenuSavedItems.push_back("[Fire Scales X]:" + std::to_string(*Fire::GetScalesX()));
	pFireMenuSavedItems.push_back("[Fire Scales Y]:" + std::to_string(*Fire::GetScalesY()));
	pFireMenuSavedItems.push_back("[Fire Scales Z]:" + std::to_string(*Fire::GetScalesZ()));
	pFireMenuSavedItems.push_back("[Fire Speed X]:" + std::to_string(*Fire::GetSSpeedX()));
	pFireMenuSavedItems.push_back("[Fire Speed Y]:" + std::to_string(*Fire::GetSSpeedY()));
	pFireMenuSavedItems.push_back("[Fire Speed Z]:" + std::to_string(*Fire::GetSSpeedZ()));
	pFireMenuSavedItems.push_back("[Fire Distortion Scale]:" + std::to_string(*Fire::GetDistortionScale()));
	pFireMenuSavedItems.push_back("[Fire Distortion Bias]:" + std::to_string(*Fire::GetDistortionBias()));
	fireMenuSettings.AddInfo(pFireMenuSavedItems);
	fireMenuSettings.CloseFile();
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
	UI::SetCanRendered(true);
	UI::DeveloperUI(std::string(adapter_name.begin(), adapter_name.end()),cpu_usage_string.c_str() ,fps.c_str(), &gfx.cam3D, GameObject::GetWireframeEnabled(),
		GameObject::GetWireColor(), GameObject::GetFogEnabled(), GameObject::GetFogColor(), GameObject::GetFogStart(),
		GameObject::GetFogEnd(), &gfx.vsync, GridMap::getRender(),
		GridMap::getColor(), &gfx, wnd.GetHWND(), this, &gfx.msaaEnabled, FSQuad::GetBlurEnabled(), 
		FSQuad::GetBlurIntensity(), FSQuad::GetSSAOEnabled(), FSQuad::GetTotalStrength(), FSQuad::GetBase(),
		FSQuad::GetArea(), FSQuad::GetFallOff(), FSQuad::GetRadius(), FSQuad::GetExposure(), FSQuad::GetGamma(),
		FSQuad::GetToneMappingEnabled(), FSQuad::GetBloomIntensity(), FSQuad::GetBloomEnabled());
	//toolbar creation
	UI::ToolBar(GridMap::getRender(),
		GameObject::GetWireframeEnabled(),
		GameObject::GetFogEnabled(), 
		GameObject::GetDepthBufferEnabled(),
		FSQuad::GetBlurEnabled(), &gfx.msaaEnabled, this, FSQuad::GetFxaaEnabled(), GameObject::GetBackCulling(),
		GameObject::GetFrontCulling(), this, DirectionalLight::GetAlphaCEnabled(), FSQuad::GetSSAOEnabled(),
		FSQuad::GetToneMappingEnabled(), FSQuad::GetBloomEnabled(), gfx.pGameObjects, gfx.pDevice.Get(),
		gfx.pContext.Get(), width, height);
	UI::SetCanRendered(false);
}