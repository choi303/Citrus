#include "App.h"

int WINAPI WinMain(HINSTANCE hInstance,    //Main windows function
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    try
    {
        //Initialize Window//
        App app; //Create a window
        app.Init("Janus 0.1", "janus", hInstance, 1366, 900);
        {
            while (app.ProcessMessages(hInstance))
            {
                app.Update();
                app.RenderFrame();
            }
        }
    }
    catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Unhandled Exception", MB_OK | MB_ICONERROR);
    }
}
