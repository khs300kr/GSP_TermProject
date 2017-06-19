#include "define.h"
#include "Global.h"
#include "PacketFunc.h"
#include "DBFunc.h"

void Initialize_NPC()
{
	for (int i = NPC_START; i < NUM_OF_NPC; ++i) {
		lua_State *L = lua_open(); //��Ƹ�����.
		luaopen_math(L);
		luaopen_os(L);
		lua_tinker::dofile(L, "monster.lua");
		if (i < NPC_START + lua_tinker::get<int>(L,"peace_num"))
		{
			g_Clients[i].m_iX = lua_tinker::call<int>(L, "return_x", MONSTER_PEACE);
			g_Clients[i].m_iY = lua_tinker::call<int>(L, "return_y", MONSTER_PEACE);
			g_Clients[i].m_Level= lua_tinker::call<int>(L, "return_level", MONSTER_PEACE);
			g_Clients[i].m_Exp= lua_tinker::call<int>(L, "return_hp", MONSTER_PEACE, g_Clients[i].m_Level);
			g_Clients[i].m_HP= lua_tinker::call<int>(L, "return_exp", MONSTER_PEACE, g_Clients[i].m_Level);
			g_Clients[i].m_ATT= lua_tinker::call<int>(L, "return_att", MONSTER_PEACE, g_Clients[i].m_Level);
			g_Clients[i].m_MonType = MONSTER_PEACE;
		}
		g_Clients[i].m_bConnect = false;
		g_Clients[i].last_move_time = chrono::high_resolution_clock::now();
		g_Clients[i].is_active = false;


		g_Clients[i].L = L;
	}
}



void Init_Server()
{
	Initialize_NPC();

	// �����ʱ�ȭ
	wcout.imbue(locale("korean"));
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	// IOCP ����
	g_Hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	// socket()
	g_ServerSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	// bind()
	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = INADDR_ANY;
	::bind(g_ServerSocket, reinterpret_cast<sockaddr *>(&ServerAddr), sizeof(ServerAddr));

	listen(g_ServerSocket, 5);
	for (int i = 0; i < MAX_USER; ++i)
	{
		g_Clients[i].m_bConnect = false;
		g_Clients[i].L = nullptr;
	}
}

void PLAYER_HEAL(int id)
{
	if (g_Clients[id].m_HP < g_Clients[id].m_Level * 100)
	{
		g_Clients[id].m_HP += g_Clients[id].m_Level * 10;
		SendCharDBinfo(id, id);
	}
}

void PLAYER_HIT(int id)
{
	int hitmon{ -1 };
	for (auto vl : g_Clients[id].view_list)		// �� �þ� ���� �ȿ��ִ³�� �߿���
	{
		if (vl < NPC_START) // �ٸ� ������
		{
		}
		else // ���͸�
		{
			g_Clients[id].vl_lock.lock();
			if (Is_Hit(id, vl) == true)
			{
				hitmon = vl;
				WakeUpNPC(vl);
				g_Clients[vl].m_HP -= g_Clients[id].m_ATT;	// HIT
			}
			g_Clients[id].vl_lock.unlock();
			if (g_Clients[vl].m_HP <= 0)		// ������.
			{
				g_Clients[vl].m_iX = -200;
				g_Clients[vl].m_iY = -200;

				g_Clients[id].m_Exp += g_Clients[vl].m_Level * 5;	// ����ġ�� ���
				//SendCharDBinfo(id, id);	// ������.
			}
		}
	}

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (g_Clients[i].m_bConnect == true && Is_Close(id, i))
		{
			SendMoninfo(i, hitmon);
			if (g_Clients[hitmon].m_HP <= 0)
			{
				SendRemovePlayerPacket(i, hitmon);
				g_Clients[i].vl_lock.lock();
				if (g_Clients[i].view_list.count(hitmon) != 0)
					g_Clients[i].view_list.erase(hitmon);
				g_Clients[i].vl_lock.unlock();
			}
			SendAttackPacket(i, id);
		}
	}
}
void NPC_ASTAR_MOVE(int id, int ci)
{
	bool _goto = false, _goto2 = false;
	//g_Clients[id].vl_lock.lock();

	g_Clients[id].Coor_list.clear();

	auto path = generator.findPath({ g_Clients[id].m_iX, g_Clients[id].m_iY },
	{ g_Clients[ci].m_iX, g_Clients[ci].m_iY });
	if (path.capacity() <= 1) {
		_goto = true;
	};
	if (!_goto)
	{
		g_Clients[id].Coor_list = path;

		//g_Clients[ci].vl_lock.unlock();

		g_Clients[id].iter = g_Clients[id].Coor_list.rbegin();

		//g_Clients[id].vl_lock.lock();
		if (g_Clients[id].Coor_list.capacity() <= 1) {
			_goto2 = true;
		}

		if (!_goto2) {
			++g_Clients[id].iter;

			if ((*g_Clients[id].iter).x != NULL && (*g_Clients[id].iter).y != NULL)
			{
				g_Clients[id].m_iX += (*g_Clients[id].iter).x - g_Clients[id].m_iX;
				g_Clients[id].m_iY += (*g_Clients[id].iter).y - g_Clients[id].m_iY;
			}
		}
	}

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (g_Clients[i].m_bConnect == true)
		{
			g_Clients[i].vl_lock.lock();
			if (g_Clients[i].view_list.count(id) != 0) {
				if (true == Is_Close(i, id)) {
					g_Clients[i].vl_lock.unlock();
					SendPositionPacket(i, id);
				}
				else {
					g_Clients[i].view_list.erase(id);
					g_Clients[i].vl_lock.unlock();
					SendRemovePlayerPacket(i, id);
				}
			}
			// �丮��Ʈ�� ����
			else {
				if (true == Is_Close(i, id)) {
					g_Clients[i].view_list.insert(id);
					g_Clients[i].vl_lock.unlock();
					SendPutPlayerPacket(i, id);
				}
				else {
					g_Clients[i].vl_lock.unlock();
				}
			}
		}
	}

	// ������ �÷��̾ ������ ������Ѵ�.
	for (int i = 0; i < MAX_USER; ++i) {
		if ((g_Clients[i].m_bConnect == true) && (Is_Close(i, id))) {

			Timer_Event t = { id,high_resolution_clock::now() + 1s, E_MOVE };
			timerqueue_lock.lock();
			timer_queue.push(t);
			timerqueue_lock.unlock();
			return;
		}
	}
	// ������� �����Դ� �ٵ��Ҵµ�..?
	g_Clients[id].is_active = false;
}

void NPC_RANDOM_MOVE(int id)
{
	int x = g_Clients[id].m_iX;
	int y = g_Clients[id].m_iY;

	switch (rand() % 4) {
	case 0: if (x > 0 && map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 3 && map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 4 &&
		map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 19 && map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 20 &&
		map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 34 && map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 35 &&
		map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 58 && map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 57 &&
		map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 10 && map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 9) x--; break;
	case 1: if (y > 0 && map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 3 && map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 4 &&
		map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 19 && map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 20 &&
		map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 34 && map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 35 &&
		map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 58 && map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 57 &&
		map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 10 && map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 9) y--; break;
	case 2: if (x < BOARD_WIDTH - 2 && map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 3 && map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 4 &&
		map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 19 && map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 20 &&
		map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 34 && map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 35 &&
		map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 58 && map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 57 &&
		map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 10 && map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 9) x++; break;
	case 3: if (y < BOARD_WIDTH - 2 && map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 3 && map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 4 &&
		map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 19 && map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 20 &&
		map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 34 && map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 35 &&
		map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 58 && map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 57 &&
		map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 10 && map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 9) y++; break;
	}
	g_Clients[id].m_iX = x;
	g_Clients[id].m_iY = y;

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (g_Clients[i].m_bConnect == true)
		{
			g_Clients[i].vl_lock.lock();
			if (g_Clients[i].view_list.count(id) != 0) {
				if (true == Is_Close(i, id)) {
					g_Clients[i].vl_lock.unlock();
					SendPositionPacket(i, id);
				}
				else {
					g_Clients[i].view_list.erase(id);
					g_Clients[i].vl_lock.unlock();
					SendRemovePlayerPacket(i, id);
				}
			}
			// �丮��Ʈ�� ����
			else {
				if (true == Is_Close(i, id)) {
					g_Clients[i].view_list.insert(id);
					g_Clients[i].vl_lock.unlock();
					SendPutPlayerPacket(i, id);
				}
				else {
					g_Clients[i].vl_lock.unlock();
				}
			}
		}
	}

	// ������ �÷��̾ ������ ������Ѵ�.
	for (int i = 0; i < MAX_USER; ++i) {
		if ((g_Clients[i].m_bConnect == true) && (Is_Close(i, id))) {

			Timer_Event t = { id,high_resolution_clock::now() + 1s, E_MOVE };
			timerqueue_lock.lock();
			timer_queue.push(t);
			timerqueue_lock.unlock();
			return;
		}
	}
	// ������� �����Դ� �ٵ��Ҵµ�..?
	g_Clients[id].is_active = false;
	// �׷��� �ڶ�!
	// ������. ������ �ٽ� �ȵ��´�. �׷��� �ָ��� ������Ѵ�.
}

void Worker_Thread()
{
	while (true)
	{
		// �񵿱� ����� �Ϸ� ��ٸ���. 
		DWORD io_size;					// �񵿱� ����� �۾��� ����Ʈ
		unsigned long long id;			// Ŭ���̾�Ʈ id
		OverlappedEx *over;				// ������ ����ü �ּ�
										// IOCP �� ����¿Ϸ� ��Ŷ�� ���� �� ���� ����Ѵ�. ����� �Ϸ� ��Ŷ�� IOCP�� ������
										// �ü���� ���� ���� �۾��� ������ ������ üũ�Ѵ�. �� ������ �ھ� �������� ������ ��� ���� �����带
										// �ƿ��� ����� �Ϸ� ��Ŷ�� ó���Ѵ�.
										// (<1> IOCP�ڵ� <2> �񵿱� ����� �۾��� ����Ʈ ���� <3> Ŭ���̾�Ʈ_id <4> OVERLAPPED ����ü�� �ּ� ����
		BOOL ret = GetQueuedCompletionStatus(g_Hiocp, &io_size, &id,
			reinterpret_cast<LPWSAOVERLAPPED *>(&over), INFINITE);

		// Error ó��
		if (ret == FALSE)
		{
			int err_no = WSAGetLastError();
			if (err_no == 64)
				DisconnectClient(id);
			else error_display("GQCS : ", WSAGetLastError());
		}
		if (io_size == 0)
		{
			DisconnectClient(id);
			continue;
		}

		// Send, Recv ó��
		if (over->m_Event_type == OP_RECV)
		{
			std::cout << "RECV from Client :" << id;
			std::cout << "  IO_SIZE : " << io_size << std::endl;
			unsigned char *buf = g_Clients[id].m_recv_over.m_IOCP_buf;
			unsigned cu_size = g_Clients[id].curr_packet_size;
			unsigned pr_size = g_Clients[id].prev_packet_data;

			while (io_size > 0)
			{
				if (cu_size == 0) cu_size = buf[0];		// ��Ŷ ����� 0 �̸�, �ٷ� ���� ó���ϴ� ��Ŷ�� ó���� ������ �Ǵ� accept �ϰ� ó�� �����͸� �޴´�.
														// io_size�� ��� 1�̴ϱ�, Ŭ���̾�Ʈ���� ����� �ǹ��ִ� �������̴�.

				if (io_size + pr_size >= cu_size)		// ��Ŷ�� �ϼ� �� �� �ִ�.
				{
					unsigned char packet[MAX_PACKET_SIZE];
					memcpy(packet, g_Clients[id].packet_buf, pr_size);
					memcpy(packet + pr_size, buf, cu_size - pr_size);
					ProcessPacket(static_cast<int>(id), packet);
					io_size -= cu_size - pr_size;
					buf += cu_size - pr_size;
					cu_size = 0; pr_size = 0;
				}
				else									// ��Ŷ�� �ϼ� ��ų �� ����.
				{
					memcpy(g_Clients[id].packet_buf + pr_size, buf, io_size);
					pr_size += io_size;
					io_size = 0;
				}
			}
			g_Clients[id].curr_packet_size = cu_size;
			g_Clients[id].prev_packet_data = pr_size;

			DWORD recv_flag = 0;
			WSARecv(g_Clients[id].m_client_socket,
				&g_Clients[id].m_recv_over.m_Wsabuf, 1,
				NULL, &recv_flag, &g_Clients[id].m_recv_over.m_Over, NULL);
		}//OP_RECV

		else if (over->m_Event_type == OP_SEND)
		{
			if (over->m_Wsabuf.len != io_size)
			{
				std::cout << "Send Incomplete Error!\n";
				closesocket(g_Clients[id].m_client_socket);
				g_Clients[id].m_bConnect = false;
			}
			delete over;
		}//OP_SEND
		else if (OP_DOAI == over->m_Event_type) {
			// ��Ƽ�������ε��� �� ���ϰ� ���ұ�...
			over->event_target = g_Clients[id].m_iTarget;
			//NPC_RANDOM_MOVE(id);
			NPC_ASTAR_MOVE(id, over->event_target);
			delete over;	// Postque delete
		}
		else if (OP_PLAYER_HEAL == over->m_Event_type)
		{
			PLAYER_HEAL(id);
			delete over;
		}
		else if (OP_PLAYER_HIT == over->m_Event_type)
		{
			PLAYER_HIT(id);
			delete over;
		}
		//else if (OP_PLAYER_MOVE_NOTIFY == over->m_Event_type) {
		//	lua_State *L = g_Clients[id].L;
		//	// L�� ��������� typing�� �����Ƿ�
		//	lua_getglobal(L, "player_move_notify");

		//	lua_pushnumber(L, over->event_target);
		//	if (0 != lua_pcall(L, 1, 0, 0))
		//	{
		//		cout << "lua error : player_move_notifty " << lua_tostring(L, -1) << endl;
		//		lua_close(L);
		//	}

		//	delete over;
		//}
		else
		{
			std::cout << "Unknown GQCS event!\n";
			while (true);
		}//UNKNOWN_GQCS

	}//Worker_Loop
}


void Accept_Thread()
{
	/*
	<Accept Thread>
	- ���� ������ ���� Ŭ���̾�Ʈ�� IOCP�� �ѱ�� ����
	- ���� ������ ���鼭,
	�� Accept() ȣ��
	�� �� Ŭ���̾�Ʈ ���� -> Ŭ���̾�Ʈ ���� ����ü ����
	�� IOCP�� ���� ��� (send/recv�� IOCP�� ���� ����)
	�� WSARecv() ȣ�� (Overlapped I/O recv ���¸� �׻� ����)
	*/
	while (true)
	{
		SOCKADDR_IN ClientAddr;
		ZeroMemory(&ClientAddr, sizeof(SOCKADDR_IN));
		ClientAddr.sin_family = AF_INET;
		ClientAddr.sin_port = htons(MY_SERVER_PORT);
		ClientAddr.sin_addr.s_addr = INADDR_ANY;
		int addr_size = sizeof(ClientAddr);
		SOCKET client_sock = WSAAccept(g_ServerSocket,
			reinterpret_cast<sockaddr *>(&ClientAddr), &addr_size, NULL, NULL);
#ifdef DEBUG
		std::cout << "[TCP����] Ŭ���̾�Ʈ ���� : IP =" << inet_ntoa(ClientAddr.sin_addr) << ", ��Ʈ ��ȣ = " <<
			ntohs(ClientAddr.sin_port) << "\n";
#endif
		// Accet loop ó��.
		int new_id{ -1 };
		for (int i = 0; i < MAX_USER; ++i)
			if (g_Clients[i].m_bConnect == false) { new_id = i; break; }
		if (new_id == -1) { std::cout << "MAX USER : " << MAX_USER << "�� ���� OVERFLOW\n"; closesocket(client_sock); continue; }


		g_Clients[new_id].m_bConnect = true;
		g_Clients[new_id].m_client_socket = client_sock;
		g_Clients[new_id].curr_packet_size = 0;
		g_Clients[new_id].prev_packet_data = 0;
		ZeroMemory(&g_Clients[new_id].m_recv_over, sizeof(g_Clients[new_id].m_recv_over));
		g_Clients[new_id].m_recv_over.m_Event_type = OP_RECV;
		// WSABUF <- IOCP ����
		g_Clients[new_id].m_recv_over.m_Wsabuf.buf =
			reinterpret_cast<CHAR *>(g_Clients[new_id].m_recv_over.m_IOCP_buf);
		g_Clients[new_id].m_recv_over.m_Wsabuf.len = sizeof(g_Clients[new_id].m_recv_over.m_IOCP_buf);
		
		// �񵿱� ����� ����
		DWORD recv_flag = 0;
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_sock), g_Hiocp, new_id, 0);
		WSARecv(client_sock, &g_Clients[new_id].m_recv_over.m_Wsabuf, 1,
			NULL, &recv_flag, &g_Clients[new_id].m_recv_over.m_Over, NULL);
	}
}


void Timer_Thread()
{
	for (;;) {
		Sleep(10);			// CPU ���� ���̱� ���ؼ�.
		for (;;) {
			// Timer ť�� ���� ����ð��� ���� �տ� �;��Ѵ�.
			timerqueue_lock.lock();
			if (timer_queue.size() == 0) {
				timerqueue_lock.unlock(); break;
			}
			Timer_Event t = timer_queue.top();
			if (t.exec_time > high_resolution_clock::now()) {
				timerqueue_lock.unlock(); break;
			}
			timer_queue.pop();
			timerqueue_lock.unlock();

			OverlappedEx* over = new OverlappedEx;
			if (t.event == E_MOVE) over->m_Event_type = OP_DOAI;
			else if (t.event == P_HEAL) over->m_Event_type = OP_PLAYER_HEAL;
			else if (t.event == P_HIT) over->m_Event_type = OP_PLAYER_HIT;

			PostQueuedCompletionStatus(g_Hiocp, 1, t.object_id, &over->m_Over);
		}
	}
}


void Close_Server()
{
	closesocket(g_ServerSocket);
	CloseHandle(g_Hiocp);
	WSACleanup();
}

int main()
{
	Init_Server();
	Init_DB();
	CreateMapFile(); // MAP ���� �ҷ�����.
	generator.setWorldSize({ BOARD_WIDTH, BOARD_HEIGHT });
	generator.setHeuristic(AStar::Heuristic::euclidean);
	generator.setDiagonalMovement(false);
	AStar::Vec2i coltmp;
	for (int i = 0; i<BOARD_HEIGHT; i++)
		for (int j = 0; j < BOARD_WIDTH; j++)
		{
			if (map[i][j] == 3 || map[i][j] == 19 || map[i][j] == 34 || map[i][j] == 57 || map[i][j] == 9
				|| map[i][j] == 4 || map[i][j] == 20 || map[i][j] == 35 || map[i][j] == 58 || map[i][j] == 10) {
				coltmp.x = j; coltmp.y = i;
				generator.addCollision(coltmp);
			}
		}


	// �۾��� ������ ����.
	std::vector<std::thread *> vWorker_threads;
	for (int i = 0; i < 6; ++i)			// �ھ�4 * 1.5 = 6
		vWorker_threads.push_back(new std::thread{ Worker_Thread });
	std::thread timer_thread{ Timer_Thread };

	std::thread accept_thread{ Accept_Thread };
	accept_thread.join();
	timer_thread.join();

	for (auto d : vWorker_threads)
	{
		d->join();
		delete d;
	}

	Close_DB();
	Close_Server();
}
