//#include <windows.h>
//
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR ipCmdline, int CmdShow)
//{	
//
//	/*�Լ��� ��� ���� �� Ȱ��*/
//	//MessageBox(NULL, ipCmdline, "Title", MB_OKCANCEL | MB_ICONEXCLAMATION);
//
//	/*�Լ��� �������� ���� ���� ���� �ڵ�*/
//	if (MessageBox(NULL, ipCmdline, "Title", MB_OKCANCEL) == IDOK) {
//		MessageBox(NULL, ipCmdline, "Title", MB_ICONEXCLAMATION);
//	}
//	return 0;
//}

///**/
//LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) // ��������
//{
//	��
//		switch (message) { // ����
//		case WM_CREATE: break; // ������޼���_���� -> ���� ����ġ������ ������
//			��
//		}
//	��
//}
//
//int APIENTRY WinMain(HINSTANCE hInstance,
//	HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//{
//	WNDCLASSEX windowClass;
//	��
//		windowClass.lpfnWndProc = WndProc; // cal��back �Լ��� ������ ����ü���� �ش� ������ ������.
//	while (1) {
//		// �޼����� ����ؼ� ����
//		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
//			��
//		}
//	}

//#include <windows.h>
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine,
//	int nCmdShow) {
//	WNDCLASSEX windowClass;
//	windowClass.cbSize = sizeof(WNDCLASSEX);
//	windowClass.style = CS_HREDRAW | CS_VREDRAW;
//	windowClass.lpfnWndProc = WindowProc;
//	windowClass.cbClsExtra = 0;
//	windowClass.cbWndExtra = 0;
//	windowClass.hInstance = hInstance;
//	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
//	windowClass.hbrBackground = NULL;
//	windowClass.lpszMenuName = NULL;
//	windowClass.lpszClassName = "WinApp Class";
//	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
//	if (!RegisterClassEx(&windowClass)) return 0;
//	HWND hwnd = CreateWindowEx(
//		NULL,
//		"WinApp Class",
//		"Title of Program",
//		WS_OVERLAPPEDWINDOW,
//		// Size and position
//		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
//		NULL, // Parent window
//		NULL, // Menu
//		hInstance, // Instance handle
//		NULL // Additional application data
//	);
//	if (hwnd == NULL) return 0;
//	ShowWindow(hwnd, SW_SHOW);// nCmdShow);
//	MSG msg;
//	while (GetMessage(&msg, NULL, 0, 0) > 0) {
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
//	return 0;
//}

