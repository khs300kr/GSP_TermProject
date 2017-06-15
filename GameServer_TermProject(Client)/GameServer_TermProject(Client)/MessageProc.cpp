#include "MessageProc.h"

HBITMAP hBackGround{};
HBITMAP hPlayer{};
HBITMAP hOther{};

void OnCreate(HINSTANCE g_hInst, HWND hWnd)
{
	//for (int i = 0; i <BOARD_HEIGHT; i++) //열 갯수
	//{
	//	for (int j = 0; j < BOARD_WIDTH; j++) //행갯수
	//	{
	//		sector[i][j].m_iX = iWidth;
	//		sector[i][j].m_iY = iHeight;
	//		if (iWidth)
	//			iWidth += 50;

	//	}
	//}
	hBackGround = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
	hPlayer = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
	hOther = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));
}


void OnRender(HWND hWnd, HDC hDC, HDC memdc)
{
	SelectObject(memdc, hBackGround);
	StretchBlt(hDC, g_LeftX, g_TopY, g_LeftX + 640, g_TopY + 640, memdc, 0, 0,1008,689, SRCCOPY);

	// Draw Grid
	for (int i = 0; i < 20; ++i)
	{
		// 가로줄
		MoveToEx(hDC, 0, 640 * i / 20, NULL);
		LineTo(hDC, 640, 640 * i / 20);
		// 세로줄
		MoveToEx(hDC, 640 * i / 20, 0, NULL);
		LineTo(hDC, 640 * i / 20, 640);
	}

	SelectObject(memdc, hPlayer);
	if(g_Player.m_bConnected == true)
		TransparentBlt(hDC, (g_Player.m_iX * 32) - 7, (g_Player.m_iY * 32) - 21, 46, 51, memdc, (g_Player.m_iFrameX * 46),
			(g_Player.m_iFrameY * 51), 46, 51, RGB(255, 0, 255));

	SelectObject(memdc, hOther);
	for (int i = 0; i < MAX_USER; ++i)
	{
		if(g_OtherPlayer[i].m_bConnected == true)
			TransparentBlt(hDC, (g_OtherPlayer[i].m_iX * 32) - 7, (g_OtherPlayer[i].m_iY * 32) - 21, 46, 51, memdc, (g_OtherPlayer[i].m_iFrameX * 48),
				(g_OtherPlayer[i].m_iFrameY * 50), 48, 50, RGB(255, 0, 255));

	}


}

void OnPacket(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
		closesocket((SOCKET)wParam);
		exit(-1);
	}
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
		ReadPacket((SOCKET)wParam);
		break;
	case FD_CLOSE:
		closesocket((SOCKET)wParam);
		exit(-1);
	}
}

void OnDestory(HWND hWnd)
{
	DeleteObject(hBackGround);
	DeleteObject(hPlayer); 	DeleteObject(hOther);

	KillTimer(hWnd, 0/*uIDEvent*/);
}

// FUNCTIONS //////////////////////////////////////////////
void ProcessPacket(char *ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_LOGIN_FAIL:
	{
		cout << "로그인 실패 다시 입력해주세요. \n";
		break;
	}
	case SC_PUT_PLAYER:
	{
		cout << "로그인 성공.\n";
		sc_packet_put_player *my_packet = reinterpret_cast<sc_packet_put_player *>(ptr);
		int id = my_packet->id;
		if (first_time) {
			g_login = true;
			first_time = false;
			g_myid = id;
		}
		if (id == g_myid) {
			g_Player.m_bConnected = true;
			g_Player.m_iX = my_packet->x;
			g_Player.m_iY = my_packet->y;
			g_Player.m_iFrameY = my_packet->dir;
			//g_Player.m_iFrameY = my_packet->dir;
			//player.attr |= BOB_ATTR_VISIBLE;
		}	
		else if (id < NPC_START) {
			g_OtherPlayer[id].m_bConnected = true;
			g_OtherPlayer[id].m_iX = my_packet->x;
			g_OtherPlayer[id].m_iY = my_packet->y;
			g_OtherPlayer[id].m_iFrameY = my_packet->dir;

			//skelaton[id].attr |= BOB_ATTR_VISIBLE;
		}
		//else {
		//	npc[id - NPC_START].x = my_packet->x;
		//	npc[id - NPC_START].y = my_packet->y;
		//	npc[id - NPC_START].attr |= BOB_ATTR_VISIBLE;
		//}
		break;
	}
	//case SC_POS:
	case SC_DOWN: SetPlayerPosition(0, ptr); break;
	case SC_LEFT: SetPlayerPosition(1, ptr); break;
	case SC_RIGHT:SetPlayerPosition(2, ptr); break;
	case SC_UP:   SetPlayerPosition(3, ptr); break;

	case SC_REMOVE_OBJECT:
	{
		sc_packet_remove_player *my_packet = reinterpret_cast<sc_packet_remove_player *>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			g_Player.m_bConnected = false;
		}
		else if (other_id < NPC_START) {
			g_OtherPlayer[other_id].m_bConnected = false;

		}
		//else {
		//	npc[other_id - NPC_START].attr &= ~BOB_ATTR_VISIBLE;
		//}
		break;
	}
	//case SC_CHAT:
	//{
	//	sc_packet_chat *my_packet = reinterpret_cast<sc_packet_chat *>(ptr);
	//	int other_id = my_packet->id;
	//	if (other_id == g_myid) {
	//		wcsncpy_s(player.message, my_packet->message, 256);
	//		player.message_time = GetTickCount();
	//	}
	//	else if (other_id < NPC_START) {
	//		wcsncpy_s(skelaton[other_id].message, my_packet->message, 256);
	//		skelaton[other_id].message_time = GetTickCount();
	//	}
	//	else {
	//		wcsncpy_s(npc[other_id - NPC_START].message, my_packet->message, 256);
	//		npc[other_id - NPC_START].message_time = GetTickCount();
	//	}
	//	break;

	//}
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}

void ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;

	int ret = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}

void SetPlayerPosition(int dir, char *ptr)
{
	sc_packet_pos *my_packet = reinterpret_cast<sc_packet_pos *>(ptr);
	int other_id = my_packet->id;
	if (other_id == g_myid) {
		cout << "MyPOS" << endl;
		cout << (int)my_packet->x << endl;
		cout << (int)my_packet->y << endl;
		cout << g_Player.m_iFrameY << endl;
		
		g_Player.m_iFrameY = dir;
		g_Player.m_iX = my_packet->x;
		g_Player.m_iY = my_packet->y;
	}
	else if (other_id < NPC_START) {
		g_OtherPlayer[other_id].m_iFrameY = dir;
		g_OtherPlayer[other_id].m_iX = my_packet->x;
		g_OtherPlayer[other_id].m_iY = my_packet->y;
	}
	//else {
	//	npc[other_id - NPC_START].x = my_packet->x;
	//	npc[other_id - NPC_START].y = my_packet->y;
	//}
	//break;
}
