#include "ChoiWin.h"
#include "resource.h"
#include "WndProc.h"

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

        // All other messages
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_KILLFOCUS:
        
        break;
    case WM_SETFOCUS:
        SetFocus(hwnd);
        break;
    case WM_ACTIVATE:
        SetFocus(hwnd);
        break;

    default:
    {
        // retrieve ptr to window class
        WndProc* const pWindow = reinterpret_cast<WndProc*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        // forward message to window class handler
        return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_NCCREATE:
    {
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        WndProc* pWindow = reinterpret_cast<WndProc*>(pCreate->lpCreateParams);
        if (pWindow == nullptr) //Sanity check
        {
            Error::Log("Critical Error: Pointer to window container is null during WM_NCCREATE.");
        }
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
        return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

bool ChoiWin::InitializeWindow(std::string wndName, std::string className, HINSTANCE hInstance, const int width, const int height) noexcept
{
    try
    {
        this->hInstance = hInstance;
        /*Create window */

        WNDCLASSEX wc{};    //Create a new wndclassex
        wc.cbSize = sizeof(wc);    //set size of WNDCLASSEX type
        wc.style = CS_HREDRAW | CS_VREDRAW;    //class styles
        wc.lpfnWndProc = HandleMessageSetup;    //Set our wndproc for proccess messages
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;    //Set current instance of winMain 
        wc.hIcon = static_cast<HICON>(LoadImage(hInstance,
            MAKEINTRESOURCE(IDI_ICON1),
            IMAGE_ICON, 256, 256, 0));    //Window icon
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);    //Mouse icon
        wc.hbrBackground = nullptr;    //window background color
        wc.lpszMenuName = nullptr;    //Name of the menu attached to our window
        wc.lpszClassName = className.c_str();    //Name of our windows class
        wc.hIconSm = static_cast<HICON>(LoadImage(hInstance,
            MAKEINTRESOURCE(IDI_ICON1),
            IMAGE_ICON, 128, 128, 0)); //icon in the taskbar 

        if (!RegisterClassEx(&wc)) //register wndclassex I created a moment ago
        {
            Error::Log("Failed to register class ex");
            return false;
        }

        // calculate window size based on desired client region size
        RECT wr;
        wr.left = 100;
        wr.right = width + wr.left;
        wr.top = 100;
        wr.bottom = height + wr.top;
        AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

        hwnd = CreateWindowEx(
            WS_EX_APPWINDOW,
            className.c_str(),
            wndName.c_str(),
            WS_OVERLAPPEDWINDOW | CW_USEDEFAULT | CW_USEDEFAULT,
            wr.left,
            wr.top,
            wr.right - wr.left,
            wr.bottom - wr.top,
            nullptr,
            nullptr,
            hInstance,
            this
        );
        SetWindowLong(hwnd, GWL_STYLE,
            GetWindowLong(hwnd, GWL_STYLE) & ~WS_MINIMIZEBOX);

        if (!hwnd)
        {
            Error::Log("Failed to create window ex");
            return false;
        }
        gfx = std::make_unique<Graphics>();
        ShowWindow(hwnd, SW_SHOW);    //show the  window
        UpdateWindow(hwnd);
    }
    catch (std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "Unhandled Exception", MB_OK | MB_ICONERROR);
    }

    return true;    //if there is no errors just return true
}

bool ChoiWin::InitializeGraphics(const int width, const int height)
{
    if(!gfx->InitializeGraphics(hwnd, width, height))
    {Error::Log("Failed to initialize graphics"); return false; }

    return true;
}

HWND ChoiWin::GetHWND() const noexcept
{
    return hwnd;
}