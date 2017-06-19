#pragma once

void error_display(char *msg, int err_no);
void CreateMapFile();

enum OPTYPE { OP_SEND, OP_RECV, OP_DOAI, OP_PLAYER_HEAL, OP_PLAYER_HIT};

struct OverlappedEx
{
	// WSAOVERLAPPED 구조체 역할 2가지.
	// 1. 비동기 입출력을 위한 정보를 운영체제에 전달한다.
	// 2. 운영체제가 비동기 입출력 결과를 응용프로그램에 알려줄 떄 사용한다.
	WSAOVERLAPPED	m_Over;
	// WSABUF (1. 길이 2. 버퍼 시작주소) => Scatter/Gather
	WSABUF			m_Wsabuf;
	// IOCP send/recv 버퍼
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
	// recv의 조립 버퍼.
	unsigned char	packet_buf[MAX_PACKET_SIZE];
	// 조립을 위한 데이터.
	int prev_packet_data;	// 조립 중 데이터
	int curr_packet_size;	// 받은 양
	// 시야구현
	std::unordered_set <int> view_list;
	std::mutex vl_lock;
	// Astar
	AStar::CoordinateList Coor_list;
	vector<AStar::Vec2i>::reverse_iterator iter;
	int	m_iTarget;
};

enum Event_Type { E_MOVE, P_HEAL, P_HIT};

struct Timer_Event {
	int object_id;											// 어떤 아이디냐?
	high_resolution_clock::time_point exec_time;			// 언제 실행할꺼냐?
	Event_Type event;										// 어떤 이벤트인가??
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