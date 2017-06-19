#pragma once

void error_display(char *msg, int err_no);
void CreateMapFile();

enum OPTYPE { OP_SEND, OP_RECV, OP_DOAI, OP_PLAYER_HEAL, OP_PLAYER_HIT};

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
	int event_target;
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
	short			m_HP;
	WORD			m_ATT;
	int				m_Gold;
	// Mon_info
	bool is_active; // NPC status
	chrono::high_resolution_clock::time_point last_move_time;
	lua_State* L;
	BYTE			m_MonType;
	// recv�� ���� ����.
	unsigned char	packet_buf[MAX_PACKET_SIZE];
	// ������ ���� ������.
	int prev_packet_data;	// ���� �� ������
	int curr_packet_size;	// ���� ��
	// �þ߱���
	std::unordered_set <int> view_list;
	std::mutex vl_lock;
	// Astar
	AStar::CoordinateList Coor_list;
	vector<AStar::Vec2i>::reverse_iterator iter;
	int	m_iTarget;
};

enum Event_Type { E_MOVE, P_HEAL, P_HIT};

struct Timer_Event {
	int object_id;											// � ���̵��?
	high_resolution_clock::time_point exec_time;			// ���� �����Ҳ���?
	Event_Type event;										// � �̺�Ʈ�ΰ�??
};

class comparison {
	bool reverse;
public:
	comparison() {}
	bool operator()(const Timer_Event first, const Timer_Event second) const {
		return first.exec_time > second.exec_time;
	}
};

extern priority_queue<Timer_Event, vector<Timer_Event>, comparison> timer_queue;
extern mutex timerqueue_lock;


// Server
extern HANDLE g_Hiocp;
extern SOCKET g_ServerSocket;
extern CLIENT g_Clients[NUM_OF_NPC];

// DB
extern SQLHENV henv;
extern SQLHDBC hdbc;
extern SQLHSTMT hstmt;
extern SQLRETURN retcode;

// Map
extern int map[300][300];
extern AStar::Generator generator;