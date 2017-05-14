#define MAX_BUFF_SIZE   4000
#define MAX_PACKET_SIZE  255

#define BOARD_WIDTH   300
#define BOARD_HEIGHT  300

// 시야간격 3
#define VIEW_RADIUS   3

#define MAX_USER 10

#define NPC_START  1000
#define NUM_OF_NPC  10000

#define MY_SERVER_PORT  7000

#define MAX_STR_SIZE  100

#define CS_DOWN		0
#define CS_LEFT		1
#define CS_RIGHT	2
#define CS_UP		3
#define CS_CHAT		5

#define SC_PUT_PLAYER    1
#define SC_REMOVE_PLAYER 2
#define SC_CHAT			 3
#define SC_DOWN			 4
#define SC_LEFT			 5
#define SC_RIGHT		 6
#define SC_UP			 7



#pragma pack (push, 1)

// Client -> Server
struct cs_packet_up {
	BYTE size;
	BYTE type;
};

struct cs_packet_chat {
	BYTE size;
	BYTE type;
	WCHAR message[MAX_STR_SIZE];
};

// Server -> Client
struct sc_packet_pos {
	BYTE size;
	BYTE type;
	WORD id;
	WORD x;
	WORD y;
	WORD dir;
};

struct sc_packet_put_player {
	BYTE size;
	BYTE type;
	WORD id;
	WORD x;
	WORD y;
	BYTE dir;
};
struct sc_packet_remove_player {
	BYTE size;
	BYTE type;
	WORD id;
};

struct sc_packet_chat {
	BYTE size;
	BYTE type;
	WORD id;
	WCHAR message[MAX_STR_SIZE];
};

#pragma pack (pop)