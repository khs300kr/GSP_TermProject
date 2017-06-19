myid = 9999;

function set_uid(x)
	myid= x;
end

function event_player_move(player)
	player_x = API_get_x(player);
	player_y = API_get_y(player);
	my_x = API_get_x(myid);
	my_y = API_get_y(myid);
	if (player_x == my_x) then
		if (player_y == my_y) then
			API_SendMessage(myid, player, "HELLO");
		end
	end
end

function npc_move_random(randnum)
	my_x = API_get_x(myid);
	my_y = API_get_y(myid);
	my_move_count = API_get_movecount(myid);

	if (my_move_count < 3) then
		if (randnum == 0) then
			my_x = my_x-1;
			my_move_count = my_move_count+1;
		elseif (randnum == 1) then
			my_y = my_y-1;
			my_move_count = my_move_count+1;
		elseif (randnum == 2) then
			my_x = my_x+1;
			my_move_count = my_move_count+1;
		elseif (randnum == 3) then
			my_y = my_y+1;
			my_move_count = my_move_count+1;
		end
	elseif (my_move_count == 3)then
		API_SendBYEMessage(myid,"BYE");
		my_move_count = 0;
	end
	return my_x, my_y, my_move_count
end





-- 평화몬스터(고정)
peace_num = 300;
function return_x(arg)
	if (arg == 0) then -- 평화
		return math.random(26,88);
	end
end

function return_y(arg)
	if (arg == 0) then -- 평화
		return math.random(34,83);
	end
end

function return_level(arg)
	if (arg == 0) then -- 평화
		return math.random(1,30);
	end
end

function return_hp(arg,level)
	if (arg == 0) then -- 평화
		return level * 5;
	end
end

function return_exp(arg,level)
	if (arg == 0) then -- 평화
		return level * 3;
	end
end

function return_att(arg,level)
	if (arg == 0) then -- 평화
		return level;
	end
end



