#pragma once
#define DEBUG							// DEBUG
#pragma warning(disable:4996)
#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <winsock.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <unordered_set>				// order�� �ʿ������ ���� ���
#include <mutex>
#include "protocol.h"

// DB
#define UNICODE
#include <sqlext.h>
using namespace std;