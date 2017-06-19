#include "define.h"
#include "Global.h"
#include "PacketFunc.h"
#include "DBFunc.h"

bool Is_NPC(int id)
{
	return ((id >= NPC_START) && (id < NUM_OF_NPC));
}

bool Is_Peace(int id)
{
	return ((id >= NPC_START) && (id < NPC_START + 500));
}

bool Is_Hit(int from, int to)
{
	if (((g_Clients[from].m_iX - 1 == g_Clients[to].m_iX) && (g_Clients[from].m_iY == g_Clients[to].m_iY)) || // 왼쪽
		((g_Clients[from].m_iX + 1 == g_Clients[to].m_iX) && (g_Clients[from].m_iY == g_Clients[to].m_iY)) ||
		((g_Clients[from].m_iX == g_Clients[to].m_iX) && (g_Clients[from].m_iY - 1 == g_Clients[to].m_iY)) || // 위쪽
		((g_Clients[from].m_iX == g_Clients[to].m_iX) && (g_Clients[from].m_iY + 1 == g_Clients[to].m_iY)))
		return true;
	else return false;
}


bool Is_Close(int from, int to)
{
	//return (g_Clients[from].m_iX - g_Clients[to].m_iX) 
	//	* (g_Clients[from].m_iX - g_Clients[to].m_iX)
	//	+ (g_Clients[from].m_iY - g_Clients[to].m_iY)
	//	* (g_Clients[from].m_iY - g_Clients[to].m_iY) <= VIEW_RADIUS * VIEW_RADIUS; 
	return ((abs(g_Clients[from].m_iX - g_Clients[to].m_iX) < 9) &&
		(abs(g_Clients[from].m_iY - g_Clients[to].m_iY) < 9));
}
void WakeUpNPC(int id)
{
	// 이미 움직인애는 움직이면 안된다. 그래서 상태가 있어야한다.
	if (g_Clients[id].is_active == true) return;
	g_Clients[id].is_active = true;
	Timer_Event event = { id,high_resolution_clock::now() + 1s, E_MOVE };
	timerqueue_lock.lock();
	timer_queue.push(event);
	timerqueue_lock.unlock();
}

void HealPlayer(int id)
{
	if (g_Clients[id].m_bConnect == false) return;
	g_Clients[id].is_active = true;
	Timer_Event event = { id,high_resolution_clock::now() + 5s, P_HEAL };
	timerqueue_lock.lock();
	timer_queue.push(event);
	timerqueue_lock.unlock();
}

void PlayerHit(int id)
{
	if (g_Clients[id].m_bConnect == false) return;
	Timer_Event event = { id,high_resolution_clock::now() + 1s, P_HIT };
	timerqueue_lock.lock();
	timer_queue.push(event);
	timerqueue_lock.unlock();
}


void Send_Packet(int client, void* packet)
{
	int packet_size = reinterpret_cast<unsigned char *>(packet)[0];
	int pcket_type = reinterpret_cast<unsigned char *>(packet)[1];
	OverlappedEx *over = new OverlappedEx;
	over->m_Event_type = OP_SEND;
	memcpy(over->m_IOCP_buf, packet, packet_size);
	ZeroMemory(&over->m_Over, sizeof(over->m_Over));
	over->m_Wsabuf.buf = reinterpret_cast<CHAR *>(over->m_IOCP_buf);
	over->m_Wsabuf.len = packet_size;

	int ret = WSASend(g_Clients[client].m_client_socket, &over->m_Wsabuf, 1, NULL, 0,
		&over->m_Over, NULL);
	if (ret != 0)
	{
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			error_display("Error in SendPacket:", err_no);
	}
	std::cout << "Send Packet [" << pcket_type << "] To Client : " << client << std::endl;
}


void SendLoginFail(int client, int ojbect)
{
	sc_packet_login_fail packet;
	packet.size = sizeof(packet);
	packet.type = SC_LOGIN_FAIL;

	Send_Packet(client, &packet);
}

void SendCharDBinfo(int client, int object)
{
	sc_packet_char_dbinfo packet;
	packet.size = sizeof(packet);
	packet.type = SC_CHAR_DBINFO;
	packet.Level = g_Clients[object].m_Level;
	packet.Exp = g_Clients[object].m_Exp;
	packet.HP = g_Clients[object].m_HP;
	packet.ATT = g_Clients[object].m_ATT;
	packet.Gold = g_Clients[object].m_Gold;

	Send_Packet(client, &packet);
}

void SendMoninfo(int client, int object)
{
	sc_packet_mon_info packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_MON_INFO;
	packet.Level = g_Clients[object].m_Level;
	packet.Exp = g_Clients[object].m_Exp;
	packet.HP = g_Clients[object].m_HP;
	packet.ATT = g_Clients[object].m_ATT;

	Send_Packet(client, &packet);
}

void SendPutPlayerPacket(int client, int object)
{
	sc_packet_put_player packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_PUT_PLAYER;
	packet.x = g_Clients[object].m_iX;
	packet.y = g_Clients[object].m_iY;
	packet.dir = g_Clients[object].m_Dir;
	packet.mon_type = g_Clients[object].m_MonType;

	Send_Packet(client, &packet);
}

void SendPositionPacket(int client, int object)
{
	sc_packet_pos packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = g_Clients[object].m_Dir;
	packet.x = g_Clients[object].m_iX;
	packet.y = g_Clients[object].m_iY;
	packet.dir = g_Clients[object].m_Dir;
	Send_Packet(client, &packet);
}

void SendRemovePlayerPacket(int client, int object)
{
	sc_packet_pos packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_REMOVE_OBJECT;

	Send_Packet(client, &packet);
}
void SendChatPacket(int client, WCHAR id[], WCHAR message[])
{
	sc_packet_chat packet;
	packet.size = sizeof(packet);
	packet.type = SC_CHAT;
	wcsncpy_s(packet.char_id, id,MAX_ID_LEN);
	wcsncpy_s(packet.message, message, MAX_STR_SIZE);
	Send_Packet(client, &packet);
}
void SendAttackPacket(int client, int object)
{
	sc_packet_attack packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_ATTACK;

	Send_Packet(client, &packet);
}

void ViewListSend(int id, unsigned char packet[])
{
	SendPositionPacket(id, id);

	// <시야구현>
	// 이동을 했으니까 뷰 리스트를 하나 생성.
	std::unordered_set<int> new_view_list;
	for (int i = 0; i < MAX_USER; ++i)
		if (g_Clients[i].m_bConnect == true)
			if (i != id)
				if (Is_Close(id, i) == true) new_view_list.insert(i);	// 자기 자신은 제외하고.					
    //
	for (int i = NPC_START; i < NUM_OF_NPC; ++i)
		if (true == Is_Close(id, i)) new_view_list.insert(i);
	//
	std::unordered_set<int> vlc;
	g_Clients[id].vl_lock.lock();
	vlc = g_Clients[id].view_list;	// 락을 걸고 카피해야한다.
	g_Clients[id].vl_lock.unlock();

	for (auto target : new_view_list)
		if (vlc.count(target) == 0) {			// view_list안에 new_view_list를 비교
			// 새로 추가되는 객체
			SendPutPlayerPacket(id, target);
			SendMoninfo(id, target);
			vlc.insert(target);
			//
			if (true == Is_NPC(target)) {
				//WakeUpNPC(target);
				continue;
			}//
			g_Clients[target].vl_lock.lock();		// lock
			if (g_Clients[target].view_list.count(id) != 0)
			{
				g_Clients[target].vl_lock.unlock();
				SendPositionPacket(target, id);
			}
			else
			{
				g_Clients[target].view_list.insert(id);
				g_Clients[target].vl_lock.unlock();		// unlock
				SendPutPlayerPacket(target, id);
			}
		}
		else {
			if (true == Is_NPC(target)) continue;
			// 변동없이 존재하는 객체 (나도있고, 상대방에게도 있고) - 움직이는게 나니까 나한테 알 필요가 없고, 상대방이 알아야한다.
			g_Clients[target].vl_lock.lock();		// lock

			if (g_Clients[target].view_list.count(id) != 0)
			{
				g_Clients[target].vl_lock.unlock();		// unlock
				SendPositionPacket(target, id);
			}
			else {
				g_Clients[target].view_list.insert(id);
				g_Clients[target].vl_lock.unlock();		// unlock
				SendPutPlayerPacket(target, id);
			}
		}

		// 시야에서 멀어진 객체
		std::unordered_set<int> faraway_id_list;
		for (auto target : vlc)	// 원래는 있는데 지금은 없는 것
			if (new_view_list.count(target) == 0)
			{
				SendRemovePlayerPacket(id, target);			// 나에게 지워라
				faraway_id_list.insert(target);
				//
				if (true == Is_NPC(target)) continue;
				//
				g_Clients[target].vl_lock.lock();		// lock
				if (g_Clients[target].view_list.count(id) != 0)	// 상대방에 내가 있나 없나 확인.
				{
					g_Clients[target].view_list.erase(id);
					g_Clients[target].vl_lock.unlock();		// unlock
					SendRemovePlayerPacket(target, id);			// 상대방도 같이 지워라
				}
				else g_Clients[target].vl_lock.unlock();		// unlock

			}

		g_Clients[id].vl_lock.lock();	// lock
		for (auto p : vlc)
			g_Clients[id].view_list.insert(p);
		for (auto d : faraway_id_list)
			g_Clients[id].view_list.erase(d);
		g_Clients[id].vl_lock.unlock();	// unlock

		//for (auto npc : new_view_list) {
		//	if (false == Is_NPC(npc)) continue;	// NPC가 아니면

		//	OverlappedEx *over_ex = new OverlappedEx;
		//	over_ex->event_target = id; // 타깃은 나 ci 클라이언트 인덱스
		//	over_ex->m_Event_type = OP_PLAYER_MOVE_NOTIFY;

		//	PostQueuedCompletionStatus(g_Hiocp, 1, npc, &over_ex->m_Over);
		//}
}


void ProcessPacket(int id, unsigned char packet[])
{
	switch (packet[1])
	{
	case CS_UP:
		if (map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 3 && map[g_Clients[id].m_iY  - 1][g_Clients[id].m_iX] != 4 &&
			map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 19 && map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 20 &&
			map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 34 && map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 35 &&
			map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 58 && map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 57 &&
			map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 10 && map[g_Clients[id].m_iY - 1][g_Clients[id].m_iX] != 9)
		{
			if (g_Clients[id].m_iY > 0 && g_Clients[id].m_Dir == SC_UP) g_Clients[id].m_iY--;
		}
			g_Clients[id].m_Dir = SC_UP; ViewListSend(id, packet);
		break;
	case CS_DOWN:
		if (map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 3 && map[g_Clients[id].m_iY  + 1][g_Clients[id].m_iX] != 4 &&
			map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 19 && map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 20 &&
			map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 34 && map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 35 &&
			map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 58 && map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 57 &&
			map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 10 && map[g_Clients[id].m_iY + 1][g_Clients[id].m_iX] != 9)
		{
			if (g_Clients[id].m_iY < BOARD_HEIGHT - 1 && g_Clients[id].m_Dir == SC_DOWN) g_Clients[id].m_iY++;
		}
			g_Clients[id].m_Dir = SC_DOWN; ViewListSend(id, packet);
		break;
	case CS_LEFT:
		if (map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 3 && map[g_Clients[id].m_iY][g_Clients[id].m_iX  - 1] != 4 &&
			map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 19 && map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 20 &&
			map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 34 && map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 35 &&
			map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 58 && map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 57 &&
			map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 10 && map[g_Clients[id].m_iY][g_Clients[id].m_iX - 1] != 9)
		{
			if (g_Clients[id].m_iX > 0 && g_Clients[id].m_Dir == SC_LEFT) g_Clients[id].m_iX--;
		}
			g_Clients[id].m_Dir = SC_LEFT; ViewListSend(id, packet);
		break;
	case CS_RIGHT:
		if (map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 3 && map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 4 &&
			map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 19 && map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 20 &&
			map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 34 && map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 35 &&
			map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 58 && map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 57 &&
			map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 10 && map[g_Clients[id].m_iY][g_Clients[id].m_iX + 1] != 9)
		{
			if (g_Clients[id].m_iX < BOARD_WIDTH - 1 && g_Clients[id].m_Dir == SC_RIGHT) g_Clients[id].m_iX++;
		}
			g_Clients[id].m_Dir = SC_RIGHT; ViewListSend(id, packet);
		break;
	case CS_LOGIN:
	{
		cs_packet_login* my_packet = reinterpret_cast<cs_packet_login*>(packet);
		//wcout << "LOGIN : " << my_packet->GAME_ID << endl;
		Client_Login(my_packet->GAME_ID, id);
		break;
	}
	case CS_LOGOUT:
	{
		cs_packet_logout* my_packet = reinterpret_cast<cs_packet_logout*>(packet);
		//wcout << "LOGOUT : " << my_packet->GAME_ID << endl;
		//cout << my_packet->x << endl;
		//cout << my_packet->y << endl;
		//cout << (int)my_packet->Level << endl;
		//cout << my_packet->Exp << endl;
		//cout << my_packet->HP << endl;
		//cout << my_packet->ATT << endl;
		//cout << my_packet->Gold << endl;

		Client_Logout(my_packet->GAME_ID,my_packet->x, my_packet->y,my_packet->Level,
			my_packet->Exp, my_packet->HP,my_packet->ATT,my_packet->Gold,	id);
		break;
	}
	// INGAME
	case CS_CHAT:
	{
		cs_packet_chat* my_packet = reinterpret_cast<cs_packet_chat*>(packet);
		
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (g_Clients[i].m_bConnect == true) {
				if (Is_Close(id, i) == true)
					SendChatPacket(i, my_packet->char_id,my_packet->message);
			}
		}

		break;
	}
	case CS_ATTACK:
	{

		int hitmon{ -1 };
		for (auto vl : g_Clients[id].view_list)		// 내 시야 범위 안에있는놈들 중에서
		{
			if (vl < NPC_START) // 다른 유저면
			{
			}
			else // 몬스터면
			{
				g_Clients[id].vl_lock.lock();
				if (Is_Hit(id, vl) == true)
				{
					hitmon = vl;
					g_Clients[vl].m_iTarget = id;
					WakeUpNPC(vl);
					g_Clients[vl].m_HP -= g_Clients[id].m_ATT;	// HIT
				}
				g_Clients[id].vl_lock.unlock();
				if (g_Clients[vl].m_HP <= 0)		// 죽으면.
				{
					g_Clients[vl].m_iX = -200;
					g_Clients[vl].m_iY = -200;

					g_Clients[id].m_Exp += g_Clients[vl].m_Level * 5;	// 경험치를 얻고
																		//SendCharDBinfo(id, id);	// 보낸다.
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
	break;
	default: std::cout << "Unknown Packet Type from Client : " << id << std::endl;
		while (true);
	}


}

void DisconnectClient(int id)
{
	closesocket(g_Clients[id].m_client_socket);
	g_Clients[id].m_bConnect = false;
	g_Clients[id].is_active = false;

	std::unordered_set<int> lvl;	// local view list
	g_Clients[id].vl_lock.lock();
	lvl = g_Clients[id].view_list;
	g_Clients[id].vl_lock.unlock();

	// <시야구현>
	for (auto target : lvl) {
		g_Clients[target].vl_lock.lock();
		if (g_Clients[target].view_list.count(id) != 0)
		{
			g_Clients[target].view_list.erase(id);
			g_Clients[target].vl_lock.unlock();
			SendRemovePlayerPacket(target, id);
		}
		else g_Clients[target].vl_lock.unlock();
	}

	g_Clients[id].vl_lock.lock();
	g_Clients[id].view_list.clear();
	g_Clients[id].vl_lock.unlock();

}
