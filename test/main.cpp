
#include <iostream>
#include <iomanip>
#include "property_test.h"

using namespace std;
using namespace spiritsaway::serialize;
using namespace spiritsaway::property;
using namespace spiritsaway::test;




void test_property_mutate()
{
	top_msg_queue cur_top_queue;
	std::deque<mutate_msg>& msg_cmd_queue = cur_top_queue.queue();

	std::vector<std::uint8_t> depth = { 1,2,3 };
	std::vector<int> temp_any_vec;
	temp_any_vec.push_back(1);
	temp_any_vec.push_back(2);

	std::unordered_map<std::string, int> temp_any_str_map;
	temp_any_str_map["hehe"] = 1;
	temp_any_str_map["haha"] = 2;

	std::unordered_map<int, int> temp_any_int_map;
	temp_any_int_map[1] = 2;
	temp_any_int_map[3] = 3;

	PropertyMap test_a;
	PropertyMap test_b;
	prop_record_proxy<PropertyMap> test_a_record_proxy(test_a, cur_top_queue, property_offset());

	mutate_msg msg;
	test_a_record_proxy.a().set(1);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	test_a_record_proxy.a().clear();
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	auto mut_b = test_a_record_proxy.b();
	mut_b.set(std::vector<std::string>{"hehe", "hahah"});
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_b.push_back("ee");
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_b.pop_back();
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}





	// mut_c begin
	auto mut_c = test_a_record_proxy.c();
	mut_c.insert(1, "eh");
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_c.insert(1, "ahaha");
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_c.clear();
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_c.insert(3, "ahaha");
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_c.erase(3);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	// mutate d begin
	auto mut_d = test_a_record_proxy.d();

	mut_d.set(std::vector<int>{1, 2});
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_d.push_back(1);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_d.push_back(2);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_d.push_back(3);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}



	mut_d.pop_back();
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_d.pop_back();
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	// mutate e begin
	auto mut_e = test_a_record_proxy.e();
	mut_e.insert(1, 2);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_e.insert(1, 3);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_e.clear();
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_e.insert(3, 4);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_e.erase(3);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_e.insert(5, 6);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	// mutate f begin

	auto mut_f = test_a_record_proxy.f();
	mut_f.insert("1", 1);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_f.insert("1", 1);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_f.clear();
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_f.insert("3", 4);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_f.erase("3");
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_f.insert("5", 5);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	// test g begin
	auto mut_g = test_a_record_proxy.g();
	json test_data;
	test_data["a"] = 1;
	test_data["id"] = 2;
	mut_g.insert(test_data);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	auto g_2_proxy = mut_g.get(2);
	if (!g_2_proxy)
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	g_2_proxy->a().set(3);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}


	mut_g.erase(2);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	std::cout << "test a is " << test_a.encode() << std::endl;
	std::cout << "test b is " << test_b.encode() << std::endl;
	mut_g.insert(test_data);
	msg = msg_cmd_queue.front();
	msg_cmd_queue.pop_front();
	test_b.replay_mutate_msg(msg.offset, msg.cmd, msg.data);
	if (!(test_a == test_b))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	std::cout << "test a is " << test_a.encode() << std::endl;
	std::cout << "test b is " << test_b.encode() << std::endl;

	std::cout << "test a save db is " << test_a.encode_with_flag(property_flags{ property_flags::save_db }) << std::endl;

	std::cout << "test a sync_clients is " << test_a.encode_with_flag(property_flags{ property_flags::sync_clients }) << std::endl;
	std::cout << "test a sync_clients  and save db is " << test_a.encode_with_flag(property_flags{ property_flags::sync_clients|property_flags::save_db }) << std::endl;
}

int main()
{
	test_property_mutate();
}