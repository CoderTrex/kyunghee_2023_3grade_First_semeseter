#include "Game/Game.h"

//namespace library
//{
//    /*--------------------------------------------------------------------
//      Function definitions
//    --------------------------------------------------------------------*/
//    void PrintHi()
//    {
//      OutputDebugString(L"hi\n");
//      MessageBox(nullptr, L"hi", L"Game Graphics Programming", MB_OK);
//    }
//}

HWND g_hWnd = nullptr;
HINSTANCE g_hInstance = nullptr;
LPCWSTR g_pszWindowName = L"GGP02: Direct3D 11 Basics";
LPCWSTR g_pszWindowClassName = L"GGPWindowClass";


LRESULT CALLBACK WindowProc (_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

HRESULT InitWindow(_In_ HINSTANCE hInstance, _In_ INT nCmdShow)
{
    WNDCLASSEX wcex =
    {
    .cbSize = sizeof(WNDCLASSEX),
    .style = CS_HREDRAW | CS_VREDRAW,
    .lpfnWndProc = WindowProc,
    .cbClsExtra = 0,
    .cbWndExtra = 0,
    .hInstance = hInstance,
    .hIcon = LoadIcon(hInstance,IDI_APPLICATION),
    .hCursor = LoadCursor(nullptr, IDC_ARROW),
    .hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
    .lpszMenuName = nullptr,
    .lpszClassName = g_pszWindowClassName,
    .hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION),
    };

    if (!RegisterClassEx(&wcex))
    {
        DWORD dwError = GetLastError();
        MessageBox(
            nullptr,
            L"Call to RegisterClassEx failed!",
            L"Game Graphics Programming",
            NULL
        );
        if (dwError != ERROR_CLASS_ALREADY_EXISTS)
        {
            return HRESULT_FROM_WIN32(dwError);
        }
        return E_FAIL;
    }
    g_hInstance = hInstance;
    RECT rc = { 0, 0, 800, 600 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hWnd = CreateWindow(
        g_pszWindowClassName,
        g_pszWindowName,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!g_hWnd)
    {
        DWORD dwError = GetLastError();
        MessageBox(
            nullptr,
            L"Call to CreateWindow failed!",
            L"Game Graphics Programming",
            NULL
        );
        if (dwError != ERROR_CLASS_ALREADY_EXISTS)
        {
            return HRESULT_FROM_WIN32(dwError);
        }
        return E_FAIL;
    }

    ShowWindow(g_hWnd, nCmdShow);
    return S_OK;
}


LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE: // window resizing
        break;
    case WM_CLOSE:
        if (MessageBox(hWnd,
            L"진짜 종료하시겠습니까?",
            L"Game Graphics Programming",
            MB_OKCANCEL) == IDOK)
        {
            DestroyWindow(hWnd);
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

}