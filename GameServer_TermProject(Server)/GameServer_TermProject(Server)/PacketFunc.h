#pragma once

// Contents Logic
bool Is_Close(int from, int to);

// Server Logic
void Send_Packet(int client, void* packet);
void ProcessPacket(int id, unsigned char packet[]);
void DisconnectClient(int id);

// Auth
void SendLoginFail(int client, int ojbect);

// Move
void SendPutPlayerPacket(int client, int object);
void SendPositionPacket(int client, int object);
void SendRemovePlayerPacket(int client, int object);
void ViewListSend(int id, unsigned char packet[]);