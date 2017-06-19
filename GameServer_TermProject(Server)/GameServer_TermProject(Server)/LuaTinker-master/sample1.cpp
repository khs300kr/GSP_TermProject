// sample1.cpp : C++ �� Lua ��ȣ���� �Լ� ������ �˾ƺ���.
//

#include <iostream>

extern "C" 
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
};

#include "lua_tinker.h"

int cpp_func(int arg1, int arg2)
{
	return arg1 + arg2;
}

int API_addnum(lua_State *L)
{
	int a = (int)lua_tonumber(L, -2);
	int b = (int)lua_tonumber(L, -1);
	lua_pop(L, 3);
	int c = a + b;
	lua_pushnumber(L, c);
	return 1;
}

int main()
{
	//char buff[256];
	//int error;
	//
	//lua_State* L = luaL_newstate();
	//luaL_openlibs(L);
	//
	//while (fgets(buff, sizeof(buff), stdin) != NULL) {
	//	error = luaL_loadbuffer(L, buff, strlen(buff), "line")
	//		|| lua_pcall(L, 0, 0, 0);
	//	if (error) {
	//		fprintf(stderr, "%s\n", lua_tostring(L, -1));
	//		lua_pop(L, 1);
	//	}
	//}
	//lua_close(L);
	//return 0;


	// Lua �� �ʱ�ȭ �Ѵ�.
	lua_State* L = lua_open();

	// Lua �⺻ �Լ����� �ε��Ѵ�.- print() ���
	luaopen_base(L);

	// LuaTinker �� �̿��ؼ� �Լ��� ����Ѵ�.
	lua_tinker::def(L, "cpp_func", cpp_func);

	// sample1.lua ������ �ε�/�����Ѵ�.
	lua_tinker::dofile(L, "sample1.lua");

	// sample1.lua �� �Լ��� ȣ���Ѵ�.
	int result = lua_tinker::call<int>(L, "lua_func", 3, 4);
	
	// lua_func(3,4) �� ����� ���
	printf("lua_func(3,4) = %d\n", result);

	// ���α׷� ����
	lua_close(L);
	return 0;

	//int rows, cols;
	//lua_State *L = luaL_newstate(); //��Ƹ�����.
	//luaL_openlibs(L); //���ǥ�ض��̺귯��������.
	//luaL_loadfile(L, "test.lua");

	//lua_pcall(L, 0, 0, 0);

	//lua_register(L, "c_addnum", API_addnum);

	//lua_getglobal(L, "addnum");
	//lua_pushnumber(L, 100);
	//lua_pushnumber(L, 555);
	//lua_pcall(L, 2, 1, 0);
	//int result = (int)lua_tonumber(L, -1);
	//printf("Result of addnum %d\n", result);
	//lua_pop(L, 1);

	//lua_close(L);
	//return 0;


	
}
