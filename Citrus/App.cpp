#include "App.h"

void App::Init(std::string wndName, std::string className, HINSTANCE hInstance, const int width, const int height)
{
	timer.Start();
	//Overloaded initialize window
	if (!wnd.InitializeWindow(wndName.c_str(), className.c_str(), hInstance, width, height))
	{
		Error::Log("Something happon when initialize the window (overload)");
	}

	if (!gfx.InitializeGraphics(wnd.GetHWND(), width, height))
	{
		Error::Log("Failed to initialize graphics");
	}
}

void App::Update() noexcept
{
	float deltaTime = (float)timer.GetMilisecondsElapsed();
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

		if (keycode == 'I') {
			MessageBoxA(NULL, "Janus 0.1 - Janus Renderer", "Janus Info", MB_OK | MB_ICONINFORMATION);
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
				gfx.cam3D.AdjustRotation((float)me.GetPosY() * 0.009f, (float)me.GetPosX() * 0.009f, 0);
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
}

void App::RenderFrame()
{
	//Render Frame/sec (75)
	//Draw test triangle all shaded
	gfx.BeginFrame();
	if (!gfx.SceneGraph())
	{
		Error::Log("Something happon to run the test triangle");
	}
	gfx.EndFrame();
}

bool App::ProcessMessages(HINSTANCE hInstance) noexcept
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
			hwnd = NULL; //Message processing loop takes care of destroying this window
			UnregisterClass("janus", hInstance);
			return false;
		}
	}

	return true;
}
