#include "define.h"
#include "Global.h"
#include "PacketFunc.h"
#include "DBFunc.h"

bool Is_Close(int from, int to)
{
	//return (g_Clients[from].m_iX - g_Clients[to].m_iX) 
	//	* (g_Clients[from].m_iX - g_Clients[to].m_iX)
	//	+ (g_Clients[from].m_iY - g_Clients[to].m_iY)
	//	* (g_Clients[from].m_iY - g_Clients[to].m_iY) <= VIEW_RADIUS * VIEW_RADIUS; 
	return ((abs(g_Clients[from].m_iX - g_Clients[to].m_iX) < 7) &&
		(abs(g_Clients[from].m_iY - g_Clients[to].m_iY) < 7));
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


void SendPutPlayerPacket(int client, int object)
{
	sc_packet_put_player packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_PUT_PLAYER;
	packet.x = g_Clients[object].m_iX;
	packet.y = g_Clients[object].m_iY;
	packet.dir = g_Clients[object].m_Dir;

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

void ViewListSend(int id, unsigned char packet[])
{
	SendPositionPacket(id, id);

	// <�þ߱���>
	// �̵��� �����ϱ� �� ����Ʈ�� �ϳ� ����.
	std::unordered_set<int> new_view_list;
	for (int i = 0; i < MAX_USER; ++i)
		if (g_Clients[i].m_bConnect == true)
			if (i != id)
				if (Is_Close(id, i) == true) new_view_list.insert(i);	// �ڱ� �ڽ��� �����ϰ�.

	std::unordered_set<int> vlc;
	g_Clients[id].vl_lock.lock();
	vlc = g_Clients[id].view_list;	// ���� �ɰ� ī���ؾ��Ѵ�.
	g_Clients[id].vl_lock.unlock();

	for (auto target : new_view_list)
		if (vlc.count(target) == 0) {			// view_list�ȿ� new_view_list�� ��
												// ���� �߰��Ǵ� ��ü
			SendPutPlayerPacket(id, target);
			vlc.insert(target);
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
			// �������� �����ϴ� ��ü (�����ְ�, ���濡�Ե� �ְ�) - �����̴°� ���ϱ� ������ �� �ʿ䰡 ����, ������ �˾ƾ��Ѵ�.
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

		// �þ߿��� �־��� ��ü
		std::unordered_set<int> faraway_id_list;
		for (auto target : vlc)	// ������ �ִµ� ������ ���� ��
			if (new_view_list.count(target) == 0)
			{
				SendRemovePlayerPacket(id, target);			// ������ ������
				faraway_id_list.insert(target);

				g_Clients[target].vl_lock.lock();		// lock
				if (g_Clients[target].view_list.count(id) != 0)	// ���濡 ���� �ֳ� ���� Ȯ��.
				{
					g_Clients[target].view_list.erase(id);
					g_Clients[target].vl_lock.unlock();		// unlock
					SendRemovePlayerPacket(target, id);			// ���浵 ���� ������
				}
				else g_Clients[target].vl_lock.unlock();		// unlock

			}

		g_Clients[id].vl_lock.lock();	// lock
		for (auto p : vlc)
			g_Clients[id].view_list.insert(p);
		for (auto d : faraway_id_list)
			g_Clients[id].view_list.erase(d);
		g_Clients[id].vl_lock.unlock();	// unlock
}


void ProcessPacket(int id, unsigned char packet[])
{
	switch (packet[1])
	{
	case CS_UP:
		if (g_Clients[id].m_iY > 0 && g_Clients[id].m_Dir == SC_UP) g_Clients[id].m_iY--;
		g_Clients[id].m_Dir = SC_UP; ViewListSend(id, packet);
		break;
	case CS_DOWN: 
		if (g_Clients[id].m_iY < BOARD_HEIGHT - 1 && g_Clients[id].m_Dir == SC_DOWN) g_Clients[id].m_iY++;
		g_Clients[id].m_Dir = SC_DOWN; ViewListSend(id, packet);
		break;
	case CS_LEFT:
		if (g_Clients[id].m_iX > 0 && g_Clients[id].m_Dir == SC_LEFT) g_Clients[id].m_iX--;
		g_Clients[id].m_Dir = SC_LEFT; ViewListSend(id, packet);
		break;
	case CS_RIGHT:
		if (g_Clients[id].m_iX < BOARD_WIDTH - 1 && g_Clients[id].m_Dir == SC_RIGHT) g_Clients[id].m_iX++;
		g_Clients[id].m_Dir = SC_RIGHT; ViewListSend(id, packet);
		break;
	case CS_LOGIN:
	{
		cs_packet_login* my_packet = reinterpret_cast<cs_packet_login*>(packet);
		wcout << my_packet->GAME_ID << endl;
		Client_Login(my_packet->GAME_ID, id);
		break;
	}
	case CS_LOGOUT:
	{
		cs_packet_logout* my_packet = reinterpret_cast<cs_packet_logout*>(packet);
		wcout << my_packet->GAME_ID << endl;
		Client_Logout(my_packet->GAME_ID, id);
		break;
	}
	default: std::cout << "Unknown Packet Type from Client : " << id << std::endl;
		while (true);
	}


}

void DisconnectClient(int id)
{
	closesocket(g_Clients[id].m_client_socket);
	g_Clients[id].m_bConnect = false;

	std::unordered_set<int> lvl;	// local view list
	g_Clients[id].vl_lock.lock();
	lvl = g_Clients[id].view_list;
	g_Clients[id].vl_lock.unlock();

	// <�þ߱���>
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