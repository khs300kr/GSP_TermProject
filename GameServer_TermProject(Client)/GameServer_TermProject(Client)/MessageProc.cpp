#include "Global.h"
#include "MessageProc.h"

HBITMAP hBackGround{};
HBITMAP hPlayer{};
HBITMAP hOther{};
HBITMAP hMainMenu{};
HBITMAP hTile{};
wstring chat_maker;
int iLine;
int iFrontRange;

void OnCreate(HINSTANCE g_hInst, HWND hWnd)
{
	hBackGround = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	hPlayer = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
	hOther = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));
	hMainMenu = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));
	hTile = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP6));
}


void OnRender(HWND hWnd, HDC hDC, HDC memdc)
{
	HFONT hFont{}, hOldFont{};
	hFont = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("휴먼편지체"));
	hOldFont = (HFONT)SelectObject(hDC, hFont);

	if (g_GameScene == MAINMENU)
	{
		SelectObject(memdc, hMainMenu);
		BitBlt(hDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, memdc, 0, 0, SRCCOPY);

		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, RGB(255, 255, 255));
		TextOut(hDC, 430, 455, str, int(wcslen(str)));
	}
	if (g_GameScene == INGAME)
	{
		// Temporary Map
		SelectObject(memdc, hBackGround);
		BitBlt(hDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, memdc, 0, 0, SRCCOPY);
		
		// Map
		SelectObject(memdc,hTile);
		for (int i = 0; i < 20; ++i){
			for (int j = 0; j < 20; ++j)
			{
				int tile_x = i + g_LeftX;
				int tile_y = j + g_TopY;
				if ((tile_x < 0) || (tile_y < 0) || (tile_x >= 300) || (tile_y >= 300)) continue;
				if((map[tile_y][tile_x] < 17))
					StretchBlt(hDC, TILEWIDTH * i , TILEWIDTH * j, TILEWIDTH, TILEWIDTH,
						memdc, (int(map[tile_y][tile_x] -1) % 16) * TILEWIDTH, (int(map[tile_y][tile_x] / 17)) * TILEWIDTH,
						TILEWIDTH, TILEWIDTH, SRCCOPY);
				else if ((map[tile_y][tile_x] < 49))
					StretchBlt(hDC, TILEWIDTH * i, TILEWIDTH * j, TILEWIDTH, TILEWIDTH,
						memdc, (int(map[tile_y][tile_x] - 1) % 16) * TILEWIDTH, (int(map[tile_y][tile_x] + 1) / 17) * TILEWIDTH,
						TILEWIDTH, TILEWIDTH, SRCCOPY);
				else if ((map[tile_y][tile_x] < 65))
					StretchBlt(hDC, TILEWIDTH * i, TILEWIDTH * j, TILEWIDTH, TILEWIDTH,
						memdc, (int(map[tile_y][tile_x] - 1) % 16) * TILEWIDTH, (int(map[tile_y][tile_x]+2) / 17) * TILEWIDTH,
						TILEWIDTH, TILEWIDTH, SRCCOPY);
				else if ((map[tile_y][tile_x] < 81))
					StretchBlt(hDC, TILEWIDTH * i, TILEWIDTH * j, TILEWIDTH, TILEWIDTH,
						memdc, (int(map[tile_y][tile_x] - 1) % 16) * TILEWIDTH, (int(map[tile_y][tile_x] +3) / 17) * TILEWIDTH,
						TILEWIDTH, TILEWIDTH, SRCCOPY);
				else if ((map[tile_y][tile_x] < 97))
					StretchBlt(hDC, TILEWIDTH * i, TILEWIDTH * j, TILEWIDTH, TILEWIDTH,
						memdc, (int(map[tile_y][tile_x] - 1) % 16) * TILEWIDTH, (int(map[tile_y][tile_x]+ 4) / 17) * TILEWIDTH,
						TILEWIDTH, TILEWIDTH, SRCCOPY);
				else if ((map[tile_y][tile_x] < 113))
					StretchBlt(hDC, TILEWIDTH * i, TILEWIDTH * j, TILEWIDTH, TILEWIDTH,
						memdc, (int(map[tile_y][tile_x] - 1) % 16) * TILEWIDTH, (int(map[tile_y][tile_x] +5)/ 17) * TILEWIDTH,
						TILEWIDTH, TILEWIDTH, SRCCOPY);
				else if ((map[tile_y][tile_x] < 129))
					StretchBlt(hDC, TILEWIDTH * i, TILEWIDTH * j, TILEWIDTH, TILEWIDTH,
						memdc, (int(map[tile_y][tile_x] - 1) % 16) * TILEWIDTH, (int(map[tile_y][tile_x] + 6) / 17) * TILEWIDTH,
						TILEWIDTH, TILEWIDTH, SRCCOPY);
				else if ((map[tile_y][tile_x] < 145))
					StretchBlt(hDC, TILEWIDTH * i, TILEWIDTH * j, TILEWIDTH, TILEWIDTH,
						memdc, (int(map[tile_y][tile_x] - 1) % 16) * TILEWIDTH, (int(map[tile_y][tile_x] + 7) / 17) * TILEWIDTH,
						TILEWIDTH, TILEWIDTH, SRCCOPY);
				else if ((map[tile_y][tile_x] < 161))
					StretchBlt(hDC, TILEWIDTH * i, TILEWIDTH * j, TILEWIDTH, TILEWIDTH,
						memdc, (int(map[tile_y][tile_x] - 1) % 16) * TILEWIDTH, (int(map[tile_y][tile_x] + 8) / 17) * TILEWIDTH,
						TILEWIDTH, TILEWIDTH, SRCCOPY);
				else if ((map[tile_y][tile_x] < 177))
					StretchBlt(hDC, TILEWIDTH * i, TILEWIDTH * j, TILEWIDTH, TILEWIDTH,
						memdc, (int(map[tile_y][tile_x] - 1) % 16) * TILEWIDTH, (int(map[tile_y][tile_x] + 9) / 17) * TILEWIDTH,
						TILEWIDTH, TILEWIDTH, SRCCOPY);
				else if ((map[tile_y][tile_x] < 193))
					StretchBlt(hDC, TILEWIDTH * i, TILEWIDTH * j, TILEWIDTH, TILEWIDTH,
						memdc, (int(map[tile_y][tile_x] - 1) % 16) * TILEWIDTH, (int(map[tile_y][tile_x] + 10) / 17) * TILEWIDTH,
						TILEWIDTH, TILEWIDTH, SRCCOPY);
			}
		}

		// UI Draw
		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, RGB(255, 255, 255));
		TextOut(hDC, 700, 550, g_Player.ID, int(wcslen(g_Player.ID)));
		TextOut(hDC, 745, 574, to_wstring(g_Player.m_Level).c_str(), int(to_wstring(g_Player.m_Level).size()));
		TextOut(hDC, 700, 596, to_wstring(g_Player.m_HP).c_str(), int(to_wstring(g_Player.m_HP).size()));
		TextOut(hDC, 823, 596, to_wstring(g_Player.m_ATT).c_str(), int(to_wstring(g_Player.m_ATT).size()));
		TextOut(hDC, 715, 618, to_wstring(g_Player.m_Exp).c_str(), int(to_wstring(g_Player.m_Exp).size()));
		TextOut(hDC, 840, 618, to_wstring(g_Player.m_Gold).c_str(), int(to_wstring(g_Player.m_Gold).size()));
		TextOut(hDC, 680, 642, to_wstring(g_Player.m_iX).c_str(), int(to_wstring(g_Player.m_iX).size()));
		TextOut(hDC, 780, 642, to_wstring(g_Player.m_iY).c_str(), int(to_wstring(g_Player.m_iY).size()));
		// UI Draw(Chat)
		vector<wstring>::iterator iter = vOutput.begin() + iFrontRange;
		for (int i = 0; iter != vOutput.end() ; ++iter, ++i)
		{
			TextOut(hDC, 643, 400 + (i * 20), iter->c_str(), int(wcslen(iter->c_str())));
		}


		SetTextColor(hDC, RGB(0, 0, 0));
		TextOut(hDC, 5, 645, str, int(wcslen(str)));


		// Grid Draw
		for (int i = 0; i < 20; ++i)
		{
			// 가로줄
			MoveToEx(hDC, 0, 640 * i / 20, NULL);
			LineTo(hDC, 640, 640 * i / 20);
			// 세로줄
			MoveToEx(hDC, 640 * i / 20, 0, NULL);
			LineTo(hDC, 640 * i / 20, 640);
		}

		// Object Draw
		SelectObject(memdc, hPlayer);
		if (g_Player.m_bConnected == true)
			TransparentBlt(hDC, 314, 294 , 46, 51, memdc, (g_Player.m_iFrameX * 46),
			(g_Player.m_iFrameY * 51), 46, 51, RGB(255, 0, 255));

		SelectObject(memdc, hOther);
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (g_OtherPlayer[i].m_bConnected == true)
				TransparentBlt(hDC, ((g_OtherPlayer[i].m_iX - g_LeftX) * 32) - 6, ((g_OtherPlayer[i].m_iY - g_TopY) * 32) - 22, 46, 51, memdc, (g_OtherPlayer[i].m_iFrameX * 48),
				(g_OtherPlayer[i].m_iFrameY * 50), 48, 50, RGB(255, 0, 255));
		}
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
	DeleteObject(hMainMenu); DeleteObject(hBackGround);
	DeleteObject(hPlayer); 	DeleteObject(hOther);
	DeleteObject(hTile);
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
		cout << "접속중입니다 다른 아이디로 로그인 해주세요. \n";
		break;
	}
	case SC_CHAR_DBINFO:
	{
		sc_packet_char_dbinfo *my_packet = reinterpret_cast<sc_packet_char_dbinfo *>(ptr);
		g_Player.m_Level = my_packet->Level;
		g_Player.m_Exp = my_packet->Exp;
		g_Player.m_HP = my_packet->HP;
		g_Player.m_ATT = my_packet->ATT;
		g_Player.m_Gold = my_packet->Gold;
		break;
	}
	case SC_PUT_PLAYER:
	{
		// 로그인 성공 처리.
		cout << "로그인 성공.\n";
		g_GameScene = INGAME;

		sc_packet_put_player *my_packet = reinterpret_cast<sc_packet_put_player *>(ptr);
		int id = my_packet->id;
		if (first_time) {
			g_login = true;
			first_time = false;
			g_myid = id;
			g_LeftX = my_packet->x - 10;
			g_TopY = my_packet->y - 10;
		}
		if (id == g_myid) {
			g_Player.m_bConnected = true;
			g_Player.m_iX = my_packet->x;
			g_Player.m_iY = my_packet->y;
			g_Player.m_iFrameY = my_packet->dir;
		}	
		else if (id < NPC_START) {
			g_OtherPlayer[id].m_bConnected = true;
			g_OtherPlayer[id].m_iX = my_packet->x;
			g_OtherPlayer[id].m_iY = my_packet->y;
			g_OtherPlayer[id].m_iFrameY = my_packet->dir;
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
	case SC_CHAT:
	{
		sc_packet_chat *my_packet = reinterpret_cast<sc_packet_chat *>(ptr);
		chat_maker = my_packet->char_id;
		chat_maker += L" : ";
		chat_maker += my_packet->message;
		vOutput.push_back(chat_maker);
		chat_maker.clear();

		if (iLine >= 7)
			++iFrontRange;
		else ++iLine;

		break;
	}
	case SC_ATTACK:
	{
		sc_packet_attack *my_packet = reinterpret_cast<sc_packet_attack *>(ptr);
		cout << "ATT" << endl;
		int id = my_packet->id;
		if (id == g_myid)
		{
			g_Player.m_bAttack = true;
		}
		else if (id != g_myid)
		{
			g_OtherPlayer[id].m_bAttack = true;
		}
		break;
	}
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
		g_Player.m_iFrameY = dir;
		g_Player.m_iX = my_packet->x;
		g_Player.m_iY = my_packet->y;
		g_LeftX = my_packet->x - 10;
		g_TopY = my_packet->y - 10;
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
