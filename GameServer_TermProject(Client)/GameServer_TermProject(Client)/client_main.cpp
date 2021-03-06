#include "Global.h"

//#define USE_CONSOLE

// Global_Variables
HINSTANCE g_hInst;
HWND g_hWnd;
HWND hChat{};

// Chat_Variables
#define ID_EDIT 100
WNDPROC wpOldEditProc;

// Window_Variables
void CALLBACK OnTimer(HWND hWnd, UINT uMsg, UINT_PTR uIDEvent, DWORD dwTime);
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK EditProc(HWND, UINT, WPARAM, LPARAM);
void CreateMapFile();

void CreateMapFile()
{
	int width = 0;
	int height = 0;
	int count = 1;//10~100자리
	FILE* fp = fopen("Resources\\map.txt", "r");

	if (fp != NULL) {
		int c;
		do {
			c = getc(fp);
			if (c == ',' || c == EOF) {
				continue;
			}
			else if (c == ' ') {
				width++;
				if (width == 300) { width = 0; height++; }
				count = 1;
			}
			else {
				map[height][width] = map[height][width] * count;
				map[height][width] += (c - 48);
				count = 10;
			}
		} while (c != EOF);
	}
	fclose(fp);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdshow)
{
	wcout.imbue(locale("korean"));
	wcin.imbue(locale("korean"));

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

	g_hWnd = CreateWindow(WINDOW_CLASS_NAME, L"TermProject_Client",
		WS_OVERLAPPEDWINDOW, 300, 0
		, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(g_hWnd, nCmdshow);
	UpdateWindow(g_hWnd);

	//
	WSADATA wsaData{};
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	g_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);


#ifdef USE_CONSOLE
	char ipAddr[20];
	cout << "접속할 서버의 IP주소를 입력하세요 : ";
	cin >> ipAddr;
#endif
	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family		= AF_INET;
	ServerAddr.sin_port			= htons(MY_SERVER_PORT);
#ifdef USE_CONSOLE
	ServerAddr.sin_addr.s_addr = inet_addr(ipAddr);
#else
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif

	int Result = WSAConnect(g_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	WSAAsyncSelect(g_mysocket, g_hWnd, WM_SOCKET, FD_CLOSE | FD_READ);

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	// 윈속종료
	closesocket(g_mysocket);
	WSACleanup();

	return (int)Message.wParam;
}

LRESULT CALLBACK EditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
 {
	switch (msg)
	{
	case WM_CHAR:
		{
			if (g_GameScene == MAINMENU && wParam == VK_RETURN)
			{
				if (wcslen(str) < 10)	// 글자수 제한 (10글자)
				{
					cs_packet_login *my_packet = reinterpret_cast<cs_packet_login*>(send_buffer);
					my_packet->size = sizeof(cs_packet_login);
					send_wsabuf.len = sizeof(cs_packet_login);
					DWORD iobyte;
					my_packet->type = CS_LOGIN;
					wcsncpy_s(my_packet->GAME_ID, str, MAX_ID_LEN);
					WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
					wcsncpy_s(g_Player.ID, str, MAX_ID_LEN); // 내 ID 저장.
				}
				SetWindowTextW(hWnd, '\0');
				memset(str, '\0', sizeof(str));
				SetFocus(g_hWnd);
				return 0;
			}
			if (g_GameScene == INGAME &&wParam == VK_RETURN)
			{
				if (wcslen(str) < MAX_STR_SIZE)
				{
					cs_packet_chat *my_packet = reinterpret_cast<cs_packet_chat*>(send_buffer);
					my_packet->size = sizeof(cs_packet_chat);
					send_wsabuf.len = sizeof(cs_packet_chat);
					DWORD iobyte;
					my_packet->type = CS_CHAT;
					wcsncpy_s(my_packet->message, str, MAX_STR_SIZE);
					wcsncpy_s(my_packet->char_id, g_Player.ID, MAX_ID_LEN);
					WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				}
				SetWindowTextW(hWnd, '\0');
				memset(str, '\0', sizeof(str));
				SetFocus(g_hWnd);
				return 0;
			}
		} // WM_KEYDOWN
	} // switch msg
	// Pass the messages we don't process here on to the
	// original window procedure for default handling.
	return CallWindowProc(wpOldEditProc, hWnd, msg, wParam, lParam);
}

void CALLBACK OnTimer(HWND hWnd, UINT uMsg, UINT_PTR uIDEvent, DWORD dwTime)
{
	if (g_Player.m_bAttack) {
		++g_Player.m_iFrameX;
		if (g_Player.m_iFrameX == 4)
		{
			g_Player.m_iFrameX = 0;
			g_Player.m_bAttack = false;
		}
	}
	
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (g_OtherPlayer[i].m_bAttack) {
			++g_OtherPlayer[i].m_iFrameX;
			if (g_OtherPlayer[i].m_iFrameX == 4)
			{
				g_OtherPlayer[i].m_iFrameX = 0;
				g_OtherPlayer[i].m_bAttack = false;
			}

		}
	}
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
		// Map Init
		CreateMapFile();


		SetTimer(hWnd, 0/*uIDEvent*/, 50/*uElapse*/, OnTimer);

#ifdef _DEBUG
		// Console Print
		AllocConsole();
		freopen("CONOUT$", "wt", stdout);
		freopen("CONIN$", "rt", stdin);
#endif // DEBUG

		// Chatting
		hChat = CreateWindow(L"edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER |
			ES_AUTOHSCROLL, 2000, 2000, 200, 25, hWnd, (HMENU)ID_EDIT, g_hInst, NULL);
		PostMessage(hChat, EM_LIMITTEXT, (WPARAM)49, 0);
		wpOldEditProc = (WNDPROC)SetWindowLongPtr(hChat,
			GWLP_WNDPROC,
			(LONG_PTR)EditProc);
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
	case WM_KEYDOWN: 
	{
		if (wParam == VK_RIGHT || wParam == VK_LEFT || wParam == VK_UP || wParam == VK_DOWN)
		{
			int x = 0, y = 0;
			switch (wParam)
			{
				case VK_RIGHT: x += 1; break;
				case VK_LEFT: x -= 1; break;
				case VK_UP: y -= 1; break;
				case VK_DOWN : y += 1; break;
			}
			cs_packet_up *my_packet = reinterpret_cast<cs_packet_up *>(send_buffer);
			my_packet->size = sizeof(my_packet);
			send_wsabuf.len = sizeof(my_packet);
			DWORD iobyte;
			if (x != 0)
			{
				if (x == 1) my_packet->type = CS_RIGHT;
				else my_packet->type = CS_LEFT;
				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}
			if (y != 0)
			{
				if (y == 1) my_packet->type = CS_DOWN;
				else my_packet->type = CS_UP;
				int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
				if (ret) {
					int error_code = WSAGetLastError();
					printf("Error while sending packet [%d]", error_code);
				}
			}
		}
		if (wParam == VK_RETURN)
		{
			SetFocus(hChat);
			InvalidateRect(g_hWnd, NULL, false);
		}
		if (g_GameScene == INGAME && (wParam == 'a' || wParam == 'A' ))
		{
			cs_packet_attack *my_packet = reinterpret_cast<cs_packet_attack*>(send_buffer);
			my_packet->size = sizeof(cs_packet_attack);
			send_wsabuf.len = sizeof(cs_packet_attack);
			DWORD iobyte;
			my_packet->type = CS_ATTACK;

			WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		}
	}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_EDIT:
			switch (HIWORD(wParam))
			{
			case EN_CHANGE:

				GetWindowText(hChat, str, sizeof(str));
				
				break;
			}
		}
		InvalidateRect(hWnd, NULL, false);
		break;
	case WM_SOCKET:
		OnPacket(hWnd, uMsg, wParam, lParam);
		break;
	case WM_CLOSE:
	{
		if (g_Player.ID[0] != '\0' && g_Player.m_Level != 0)
		{
			cs_packet_logout *my_packet = reinterpret_cast<cs_packet_logout*>(send_buffer);
			my_packet->size = sizeof(cs_packet_logout);
			send_wsabuf.len = sizeof(cs_packet_logout);
			DWORD iobyte;
			my_packet->type = CS_LOGOUT;
			wcsncpy_s(my_packet->GAME_ID, g_Player.ID, MAX_ID_LEN);
			my_packet->x = g_Player.m_iX;
			my_packet->y = g_Player.m_iY;
			my_packet->Level = g_Player.m_Level;
			my_packet->Exp = g_Player.m_Exp;
			my_packet->HP = g_Player.m_HP;
			my_packet->ATT = g_Player.m_ATT;
			my_packet->Gold = g_Player.m_Gold;

			WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		}
		break;
	}

	case WM_DESTROY:
		SetWindowLongPtr(hChat, GWLP_WNDPROC, (LONG_PTR)wpOldEditProc);
		OnDestory(hWnd);
		FreeConsole();
		PostQuitMessage(0);
		break;
	} // switch
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

