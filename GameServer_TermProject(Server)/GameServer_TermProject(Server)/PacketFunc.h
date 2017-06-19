#pragma once

// Contents Logic
bool Is_NPC(int id);
bool Is_Close(int from, int to);
bool Is_Peace(int id);
bool Is_Hit(int from, int to);
void WakeUpNPC(int id);
void HealPlayer(int id);
void PlayerHit(int id);
// Server Logic
void Send_Packet(int client, void* packet);
void ProcessPacket(int id, unsigned char packet[]);
void DisconnectClient(int id);

// Auth
void SendLoginFail(int client, int ojbect);
void SendCharDBinfo(int client, int object);
void SendMoninfo(int client, int object);

// Move
void SendPutPlayerPacket(int client, int object);
void SendPositionPacket(int client, int object);
void SendRemovePlayerPacket(int client, int object);
void ViewListSend(int id, unsigned char packet[]);

// Ingame
void SendChatPacket(int client, WCHAR id[], WCHAR message[]);
void SendAttackPacket(int client, int object);

