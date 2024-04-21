#include <stdbool.h>
#include <windows.h>
#include "view.h"

int WinMain(HINSTANCE hi, HINSTANCE unused1, PSTR unused2, int unused3) {
	(void)unused1;
	(void)unused2;
	(void)unused3;

	WNDCLASSA wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = wndproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hi;
	wc.hIcon = LoadIcon(NULL , IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL , IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = "GAME_OF_LIFE";
	wc.lpszClassName = "GAME_OF_LIFE";
	if (!RegisterClassA(&wc))
		return 0;

	RECT rect = {
		.right = window_width,
		.bottom = window_height,
	};
	static const DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | 
	                           WS_VISIBLE;
	AdjustWindowRect(&rect, style, false);

	HWND hwnd = CreateWindowA("GAME_OF_LIFE", "Game of Life", style, 100, 100,
	                          rect.right - rect.left, rect.bottom - rect.top,
                            NULL, NULL, hi, NULL);
	if (hwnd == NULL)
		return 0;

	for (MSG msg; GetMessageA(&msg, NULL, 0, 0);) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return 0;
}
