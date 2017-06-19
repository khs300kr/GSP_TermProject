#include "Global.h"

// Server Global
SOCKET	g_mysocket{};
WSABUF	send_wsabuf{};
char 	send_buffer[BUF_SIZE]{};
WSABUF	recv_wsabuf{};
char	recv_buffer[BUF_SIZE]{};
char	packet_buffer[BUF_SIZE]{};
DWORD	in_packet_size = 0;
int		saved_packet_size = 0;
int		g_myid{};

// Game Global
RECT   g_windowrect{};
Player g_Player{};
Player g_OtherPlayer[MAX_USER]{};

bool g_login{};

int	   g_LeftX{};
int	   g_TopY{};
int	   g_GameScene{};

// CHAT
TCHAR str[CHAT_LENGTH]{};
SIZE  lensize{};
vector<wstring> vOutput{};