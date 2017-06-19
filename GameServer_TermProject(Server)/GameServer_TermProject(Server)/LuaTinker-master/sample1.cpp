// sample1.cpp : C++ 와 Lua 상호간의 함수 실행을 알아본다.
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


	// Lua 를 초기화 한다.
	lua_State* L = lua_open();

	// Lua 기본 함수들을 로드한다.- print() 사용
	luaopen_base(L);

	// LuaTinker 를 이용해서 함수를 등록한다.
	lua_tinker::def(L, "cpp_func", cpp_func);

	// sample1.lua 파일을 로드/실행한다.
	lua_tinker::dofile(L, "sample1.lua");

	// sample1.lua 의 함수를 호출한다.
	int result = lua_tinker::call<int>(L, "lua_func", 3, 4);
	
	// lua_func(3,4) 의 결과물 출력
	printf("lua_func(3,4) = %d\n", result);

	// 프로그램 종료
	lua_close(L);
	return 0;

	//int rows, cols;
	//lua_State *L = luaL_newstate(); //루아를연다.
	//luaL_openlibs(L); //루아표준라이브러리를연다.
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
