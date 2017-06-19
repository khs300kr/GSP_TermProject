-- C++ 에서 등록한 변수를 출력한다.
print("cpp_int = "..cpp_int)


-- Lua 쪽 변수에 값을 할당한다.
lua_int = {1,1,1,2,3};

-- LuaTinker 에게 테이블을 넘긴다.
function return_table()
	local ret = {1,1,2,3}
	return ret
end
