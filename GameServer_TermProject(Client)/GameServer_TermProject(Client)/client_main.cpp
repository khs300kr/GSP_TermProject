#include "Global.h"

// Global_Variables
HINSTANCE g_hInst;

// Window_Variables
void CALLBACK OnTimer(HWND hWnd, UINT uMsg, UINT_PTR uIDEvent, DWORD dwTime);
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdshow)
{
	//DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);

	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = WINDOW_CLASS_NAME;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(WINDOW_CLASS_NAME, L"TermProject_Client",
		WS_OVERLAPPEDWINDOW, 300, 0
		, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdshow);
	UpdateWindow(hWnd);

	//
	WSADATA wsaData{};
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	g_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int Result = WSAConnect(g_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	WSAAsyncSelect(g_mysocket, hWnd, WM_SOCKET, FD_CLOSE | FD_READ);

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;
	//
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	// 윈속종료
	closesocket(g_mysocket);
	WSACleanup();

	return (int)Message.wParam;
}
// DIALOG /////////////////////////////////////////////////////
//BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
//{
//	static char ipaddr[20];
//
//	switch (iMsg)
//	{
//	case WM_INITDIALOG:
//		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);;
//		return TRUE;
//
//	case WM_COMMAND:
//		switch (LOWORD(wParam)) {
//		case IDOK:
//			GetDlgItemText(hDlg, IDC_EDIT1, ipaddr, 20);
//			strIP = ipaddr;
//			EndDialog(hDlg, IDOK);
//
//			return true;
//		case IDCANCEL:
//			EndDialog(hDlg, IDCANCEL);
//			exit(1);
//			return TRUE;
//		}
//		break;
//	}
//	return FALSE;
//}


void CALLBACK OnTimer(HWND hWnd, UINT uMsg, UINT_PTR uIDEvent, DWORD dwTime)
{

	InvalidateRect(hWnd, NULL, FALSE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	GetClientRect(hWnd, &g_windowrect);
	static HDC hdc, memdc, mem1dc;	 //Double Buffering
	static HBITMAP OffBit, hBit;     //Double Buffering
	static BITMAP bmp;
	PAINTSTRUCT ps;

	//메시지 처리하기
	switch (uMsg) {
	case WM_CREATE:
		SetTimer(hWnd, 0/*uIDEvent*/, 20/*uElapse*/, OnTimer);

#ifdef _DEBUG
		// Console Print
		AllocConsole();
		freopen("CONOUT$", "wt", stdout);
		SetConsoleTitleA("Debug");
#endif // DEBUG

		// Init()
		OnCreate(g_hInst, hWnd);
		break;

	case WM_PAINT:
		// Double Buffering
		hdc = BeginPaint(hWnd, &ps);
		OffBit = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
		memdc = CreateCompatibleDC(hdc);
		mem1dc = CreateCompatibleDC(hdc);

		SelectObject(memdc, OffBit);
		SelectObject(mem1dc, hBit);
		BitBlt(memdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, mem1dc, 0, 0, SRCCOPY);

		// Render
		OnRender(hWnd, memdc, mem1dc);

#ifdef _DEBUG
		// FPS
		//if (m_dwTime + 1000 < GetTickCount())
		//{
		//	m_dwTime = GetTickCount();
		//	wsprintf(m_szFps, "FPS : %d", m_iFps);
		//	SetWindowText(hWnd, m_szFps);
		//	m_iFps = 0;
		//}
		//++m_iFps;
#endif // DEBUG


		// Double Buffering
		BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, memdc, 0, 0, SRCCOPY);
		DeleteObject(OffBit);
		DeleteObject(hBit);
		DeleteDC(memdc);
		DeleteDC(mem1dc);
		EndPaint(hWnd, &ps);

		break;
	case WM_KEYDOWN: {
		int x = 0, y = 0;
		if (wParam == VK_RIGHT)	x += 1;
		if (wParam == VK_LEFT)	x -= 1;
		if (wParam == VK_UP)	y -= 1;
		if (wParam == VK_DOWN)	y += 1;
		cs_packet_up *my_packet = reinterpret_cast<cs_packet_up *>(send_buffer);
		my_packet->size = sizeof(my_packet);
		send_wsabuf.len = sizeof(my_packet);
		DWORD iobyte;
		if (x != 0) {
			if (x == 1) my_packet->type = CS_RIGHT;
			else my_packet->type = CS_LEFT;
			int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
			if (ret) {
				int error_code = WSAGetLastError();
				printf("Error while sending packet [%d]", error_code);
			}
		}
		if (y != 0) {
			if (y == 1) my_packet->type = CS_DOWN;
			else my_packet->type = CS_UP;
			int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
			if (ret) {
				int error_code = WSAGetLastError();
				printf("Error while sending packet [%d]", error_code);
			}
		}
	}
		break;
	case WM_SOCKET:
		OnPacket(hWnd, uMsg, wParam, lParam);
		break;
	case WM_DESTROY:
		OnDestory(hWnd);
		FreeConsole();
		PostQuitMessage(0);
		break;
	} // switch
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

