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
#include <unordered_set>				// order가 필요없으면 성능 향상
#include <mutex>
#include <string>
#include <chrono>
#include <queue>
#include "AStar.hpp"
#include "protocol.h"
// Lua
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};
#include "lua_tinker.h"

// DB
#include <sqlext.h>
using namespace std;
using namespace chrono;
