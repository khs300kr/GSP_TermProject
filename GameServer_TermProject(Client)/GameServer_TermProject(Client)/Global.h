// Includes
#pragma once
#pragma warning(disable:4996)
#pragma comment(lib,"ws2_32")
#pragma comment(lib,"msimg32.lib")
#include <WinSock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include "MessageProc.h"
#include "resource.h"
#include "../../GameServer_TermProject(Server)/GameServer_TermProject(Server)/protocol.h"

using namespace std;

// Defines
#define WINDOW_CLASS_NAME L"WINXCLASS"  // class name

#define WINDOW_WIDTH    900   // size of window
#define WINDOW_HEIGHT   710
#define CHAT_LENGTH 50

#define	BUF_SIZE				1024
#define	WM_SOCKET				WM_USER + 1
// Key Input
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define MAINMENU	0
#define INGAME		1

// SERVER GLOBAL
extern SOCKET	g_mysocket;
extern WSABUF	send_wsabuf;
extern char 	send_buffer[BUF_SIZE];
extern WSABUF	recv_wsabuf;
extern char	recv_buffer[BUF_SIZE];
extern char	packet_buffer[BUF_SIZE];
extern DWORD	in_packet_size;
extern int		saved_packet_size;
extern int		g_myid;

// GAME GLOBAL
struct Player {
	int m_iX;
	int	m_iY;
	int	m_iFrameX;
	int	m_iFrameY;
	TCHAR ID[MAX_ID_LEN];
	bool m_bConnected;	// Hide Or Not
	// Char_info
	BYTE			m_Level;
	WORD			m_Exp;
	WORD			m_HP;
	WORD			m_ATT;
	int				m_Gold;
	// Attack
	bool			m_bAttack;
};

extern RECT	  g_windowrect;
extern Player g_Player;
extern Player g_OtherPlayer[MAX_USER];

extern int	  g_GameScene;
extern bool	  g_login;

extern int	  g_LeftX;
extern int	  g_TopY;

// CHAT
extern TCHAR str[CHAT_LENGTH];
extern SIZE lensize;
extern vector<wstring> vOutput;
