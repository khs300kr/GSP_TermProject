#pragma once

void error_display(char *msg, int err_no);
enum OPTYPE { OP_SEND, OP_RECV };

struct OverlappedEx
{
	// WSAOVERLAPPED ����ü ���� 2����.
	// 1. �񵿱� ������� ���� ������ �ü���� �����Ѵ�.
	// 2. �ü���� �񵿱� ����� ����� �������α׷��� �˷��� �� ����Ѵ�.
	WSAOVERLAPPED	m_Over;
	// WSABUF (1. ���� 2. ���� �����ּ�) => Scatter/Gather
	WSABUF			m_Wsabuf;
	// IOCP send/recv ����
	unsigned char	m_IOCP_buf[MAX_BUFF_SIZE];
	// Send(?) Recv(?)
	OPTYPE			m_Event_type;

};

struct CLIENT
{
	int				m_iX;
	int				m_iY;
	BYTE			m_Dir;
	bool			m_bConnect;
	SOCKET			m_client_socket;
	OverlappedEx	m_recv_over;
	// Char_info
	BYTE			m_Level;
	WORD			m_Exp;
	WORD			m_HP;
	WORD			m_ATT;
	int				m_Gold;

	// recv�� ���� ����.
	unsigned char	packet_buf[MAX_PACKET_SIZE];
	// ������ ���� ������.
	int prev_packet_data;	// ���� �� ������
	int curr_packet_size;	// ���� ��
	// �þ߱���
	std::unordered_set <int> view_list;
	std::mutex vl_lock;
};

// Server
extern HANDLE g_Hiocp;
extern SOCKET g_ServerSocket;
extern CLIENT g_Clients[MAX_USER];

// DB
extern SQLHENV henv;
extern SQLHDBC hdbc;
extern SQLHSTMT hstmt;
extern SQLRETURN retcode;