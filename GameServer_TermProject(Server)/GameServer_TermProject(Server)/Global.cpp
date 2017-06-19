#include "define.h"
#include "Global.h"

void error_display(char *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"에러" << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
	while (true);
}

priority_queue<Timer_Event, vector<Timer_Event>, comparison> timer_queue{};
std::mutex timerqueue_lock{};

// Server
HANDLE g_Hiocp{};
SOCKET g_ServerSocket{};
CLIENT g_Clients[NUM_OF_NPC]{};

// DB
SQLHENV henv{};
SQLHDBC hdbc{};
SQLHSTMT hstmt{};
SQLRETURN retcode{};;

// Map
int map[300][300]{};
AStar::Generator generator{};


void CreateMapFile()
{
	int width = 0;
	int height = 0;
	int count = 1;//10~100자리
	FILE* fp = fopen("map.txt", "r");

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

