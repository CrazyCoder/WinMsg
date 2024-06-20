#include <windows.h>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <chrono>

#include "resource.h"

static constexpr const char *const CLASS_NAME = "WinMsgWindowClass";
static constexpr const char *const WINDOW_NAME = "WinMsg";

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_PAINT: {
            // hide mouse cursor
            SetCursor(NULL);

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);
            int fontSize = std::min(rect.right, rect.bottom) / 8; // adjust this for relative font size

            HFONT hfont, hOldFont;
            hfont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
            hOldFont = (HFONT) SelectObject(hdc, hfont);

            // Set the text color to white
            SetTextColor(hdc, RGB(255, 255, 255));
            // Set the background color to black
            SetBkColor(hdc, RGB(0, 0, 0));
            SetBkMode(hdc, OPAQUE);

            std::string text = (__argc > 1) ? __argv[1] : "";
            DrawText(hdc, text.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            SelectObject(hdc, hOldFont);
            DeleteObject(hfont);

            EndPaint(hwnd, &ps);
        }
            break;
        default:
            return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProcedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = static_cast<HICON>(::LoadImage(hInstance,
                                              MAKEINTRESOURCE(IDI_MAIN),
                                              IMAGE_ICON,
                                              256, 256,
                                              LR_DEFAULTCOLOR));
    wc.hCursor = NULL; // hide mouse cursor
    wc.hbrBackground = (HBRUSH) (HBRUSH) CreateSolidBrush(RGB(0, 0, 0));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = CLASS_NAME;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        return 0;
    }

    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    hwnd = CreateWindowEx(0, CLASS_NAME, WINDOW_NAME, WS_POPUP, 0, 0, nScreenWidth, nScreenHeight, NULL, NULL,
                          hInstance, NULL);

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG Msg;
    int ReturnCode;
    int timeout = (__argc > 2) ? std::stoi(__argv[2]) : 5000;
    auto start = std::chrono::high_resolution_clock::now();

    while (true) {
        while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
            if (Msg.message == WM_QUIT) {
                ReturnCode = 0;
                goto End;
            }
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        if (elapsed > timeout) {
            ReturnCode = 0;
            break;
        }
    }

    End:
    return ReturnCode;
}
