#include <Common.h>
#include <Game/Game.h>

    INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow)
    {
        if (FAILED(InitWindow(hInstance, nCmdShow)))
            return 0;

        if (FAILED(InitDevice()))
        {
            CleanupDevice();
            return 0;
        }

        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
        TODO: Elapse Time 변수 선언
        -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        

        // Main message loop
        MSG msg = { 0 };
        while (WM_QUIT != msg.message)
        {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
                TODO: 매 프레임마다 Elapse Time 계산
                -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
                

                // HandleInput(deltaTime);
                // Update(deltaTime);
                
                Render();
            }
        }

        CleanupDevice();

        return (int)msg.wParam;
    }