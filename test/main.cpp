
#include <iostream>
#include <iomanip>
#include "property_test.h"

using namespace std;
using namespace spiritsaway::serialize;
using namespace spiritsaway::property;
using namespace spiritsaway::test;



void test_number(top_msg_queue& cur_top_queue, prop_record_proxy<PropertyMap> test_a_record_proxy, prop_replay_proxy<PropertyMap> test_b_replay_proxy)
{
	mutate_msg msg;
	test_a_record_proxy.a().set(1);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	test_a_record_proxy.a().clear();
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
}


void test_vector(top_msg_queue& cur_top_queue, prop_record_proxy<PropertyMap> test_a_record_proxy, prop_replay_proxy<PropertyMap> test_b_replay_proxy)
{
	mutate_msg msg;

	auto mut_b = test_a_record_proxy.b();
	mut_b.set(std::vector<std::string>{"hehe", "hahah"});
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_b.push_back("ee");
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_b.pop_back();
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	// mutate d begin
	auto mut_d = test_a_record_proxy.d();

	mut_d.set(std::vector<int>{1, 2});
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_d.push_back(1);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_d.push_back(2);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_d.push_back(3);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}



	mut_d.pop_back();
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_d.pop_back();
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
}

void test_map(top_msg_queue& cur_top_queue, prop_record_proxy<PropertyMap> test_a_record_proxy, prop_replay_proxy<PropertyMap> test_b_replay_proxy)
{
	mutate_msg msg;

	// mut_c begin
	auto mut_c = test_a_record_proxy.c();
	mut_c.insert(1, "eh");
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_c.insert(1, "ahaha");
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_c.clear();
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_c.insert(3, "ahaha");
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_c.erase(3);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	// mutate e begin
	auto mut_e = test_a_record_proxy.e();
	mut_e.insert(1, 2);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_e.insert(1, 3);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_e.clear();
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_e.insert(3, 4);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_e.erase(3);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_e.insert(5, 6);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	// mutate f begin

	auto mut_f = test_a_record_proxy.f();
	mut_f.insert("1", 1);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_f.insert("1", 1);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_f.clear();
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	mut_f.insert("3", 4);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_f.erase("3");
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	mut_f.insert("5", 5);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
}

void test_bag(top_msg_queue& cur_top_queue, prop_record_proxy<PropertyMap> test_a_record_proxy, prop_replay_proxy<PropertyMap> test_b_replay_proxy)
{
	mutate_msg msg;
	// test g begin
	auto mut_g = test_a_record_proxy.g();
	json test_data;
	test_data["a"] = 1;
	test_data["id"] = 2;
	mut_g.insert(test_data);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	auto g_2_proxy = mut_g.get(2);
	if (!g_2_proxy)
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	g_2_proxy->a().set(3);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	test_data["a"] = 1;
	test_data["id"] = 3;
	mut_g.insert(test_data);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	auto g_3_proxy = mut_g.get(3);
	if (!g_3_proxy)
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}


	mut_g.erase(2);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	std::cout << "test a is " << test_a_record_proxy.get_PropertyMap().encode() << std::endl;
	std::cout << "test b is " << test_b_replay_proxy.data().encode() << std::endl;
	mut_g.insert(test_data);
	msg = cur_top_queue.front();
	cur_top_queue.pop_front();
	test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}

	auto g_4_proxy = mut_g.get_insert(2);
	g_4_proxy.a().set(3);

	while(!cur_top_queue.empty())
	{
		msg = cur_top_queue.front();
		cur_top_queue.pop_front();
		test_b_replay_proxy.replay(msg.offset.to_replay_offset(), msg.cmd, msg.data);
	}
	if (!(test_a_record_proxy.get_PropertyMap() == test_b_replay_proxy.data()))
	{
		std::cout << "fail to relay " << __LINE__ << std::endl;
	}
	std::cout << "test a is " << test_a_record_proxy.get_PropertyMap().encode() << std::endl;
	std::cout << "test b is " << test_b_replay_proxy.data().encode() << std::endl;
}
void test_set(top_msg_queue& cur_top_queue, prop_record_proxy<PropertyMap> test_a_record_proxy, prop_replay_proxy<PropertyMap> test_b_replay_proxy)
{
	mutate_msg msg;
}

void test_flags(top_msg_queue& cur_top_queue, prop_record_proxy<PropertyMap> test_a_record_proxy, prop_replay_proxy<PropertyMap> test_b_replay_proxy)
{
	mutate_msg msg;
	std::cout << "test a save db is " << test_a_record_proxy.get_PropertyMap().encode_with_flag(property_flags{ test_property_flags::save_db }, true, false) << std::endl;

	std::cout << "test a sync_clients is " << test_a_record_proxy.get_PropertyMap().encode_with_flag(property_flags{ test_property_flags::sync_clients }, true, false) << std::endl;
	std::cout << "test a sync_clients  and save db is " << test_a_record_proxy.get_PropertyMap().encode_with_flag(property_flags{ test_property_flags::sync_clients | test_property_flags::save_db }, true, false) << std::endl;

	std::vector< property_flags> filter_flags_vec;
	filter_flags_vec.push_back(property_flags{ test_property_flags::sync_clients });
	filter_flags_vec.push_back(property_flags{ test_property_flags::save_db });
	filter_flags_vec.push_back(property_flags{ test_property_flags::sync_ghost });
	filter_flags_vec.push_back(property_flags{ test_property_flags::save_db | test_property_flags::sync_clients });

	auto mut_g = test_a_record_proxy.g();
	json test_data;
	test_data["a"] = 1;
	test_data["id"] = 3;
	test_data["b"] = "hehe";
	mut_g.insert(test_data);

	
	while (!cur_top_queue.empty())
	{
		msg = cur_top_queue.front();
		cur_top_queue.pop_front();
		std::cout << "flag " <<msg.flag.value<<" data "<< msg.data.dump(4) << std::endl;

	}

	

}



int main()
{
	std::vector<property_flags> need_flags;
	need_flags.push_back(property_flags{ 0 });
	top_msg_queue cur_top_queue(need_flags, true, true);
	PropertyMap test_a;
	PropertyMap test_b;
	prop_record_proxy<PropertyMap> test_a_record_proxy(test_a, cur_top_queue, property_record_offset(), property_flags{ test_property_flags::mask_all });
	prop_replay_proxy<PropertyMap> test_b_replay_proxy(test_b);

	test_number(cur_top_queue, test_a_record_proxy, test_b_replay_proxy);
	test_vector(cur_top_queue, test_a_record_proxy, test_b_replay_proxy);
	test_map(cur_top_queue, test_a_record_proxy, test_b_replay_proxy);
	test_bag(cur_top_queue, test_a_record_proxy, test_b_replay_proxy);
	need_flags.push_back(property_flags{ test_property_flags::save_db });
	need_flags.push_back(property_flags{ test_property_flags::sync_clients });
	test_flags(cur_top_queue, test_a_record_proxy, test_b_replay_proxy);
	return 1;
}