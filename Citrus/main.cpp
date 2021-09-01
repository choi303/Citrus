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
        double version = 0.2; //set version
        std::string versionStr = std::to_string(version);
        versionStr = versionStr.erase(versionStr.find_last_not_of('0') + 1, std::string::npos);
        app.Init("Citrus " + versionStr + " <DX11>", "janus", hInstance, 1600, 900, version);
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
