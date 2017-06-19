#pragma once
void OnCreate(HINSTANCE g_hInst, HWND hWnd);
void OnRender(HWND hWnd, HDC hDC, HDC memdc);
void OnPacket(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnDestory(HWND hWnd);

// Server
void ProcessPacket(char *ptr);
void ReadPacket(SOCKET sock);
void SetPlayerPosition(int dir,char *ptr);