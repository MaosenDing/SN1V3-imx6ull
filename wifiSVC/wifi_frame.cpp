#include "SN1V2_rtConfig.h"
#include "wifi_ctrl.h"
#include <clockd_def.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/prctl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include "JDcomhead.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <fstream>
#include <regex>
#include <sys/wait.h>
#include <thread>
#include "errHandle/errHandle.h"
#include "mem_share.h"
#include "versions.h"
#include "jd_share.h"
#include "iostream"
#include <memory>
#include <vector>
#include "wifi_snd.h"
using namespace std;


int wifi_open(WIFI_INFO & wifi)
{
	//启动接收服务
	init_rec_pro(&wifi);
	//上电

	//设置
	set_wifi_module(wifi);
	//对时
	if (0 != get_wifi_tim(wifi)) {
		return -3;
	}
	//其他
	return 0;
}


int wifi_close(WIFI_INFO & wifi)
{
	//
	close_rec_pro(&wifi);
	//关闭电源


	return 0;
}

shared_ptr < vector <unsigned int> > read_num(WIFI_INFO & wifi, int Milliseconds)
{
	WIFI_BASE_SESSION sec;
	wifi_reset_buff_status(wifi);
	chrono::time_point<std::chrono::system_clock> endpoint = chrono::system_clock::now() + chrono::milliseconds(Milliseconds);
	if (wifi.dbg_pri_wifi_ctrl) printf("index get start ...\n");
	do {
		mk_read_num_session(wifi, sec);

		transmit_session(wifi, sec);

		if (wifi.dbg_pri_wifi_ctrl) printf("index get wait ...\n");

		auto ret = wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool {
			return (session.code_num  == (CODE_READ_NUM | 0x80));
			}, 100);

		if (ret && ret->data_len == 4) {
			unsigned int start = ret->data[0] | ret->data[1] << 8;
			unsigned int end = ret->data[2] | ret->data[3] << 8;
			if (end >= start) {
				if (wifi.dbg_pri_wifi_ctrl) printf("index get OK\n");
				auto ret = make_shared<vector<unsigned int>>();
				do {
					ret->push_back(start);
				} while (start++ < end);
				return ret;
			}
		}

		if (ret && ret->data_len == 0) {
			if (wifi.dbg_pri_wifi_ctrl) printf("index get NULL\n");
		}

		if (!ret) {
			if (wifi.dbg_pri_wifi_ctrl) printf("index get no response\n");
		}

		this_thread::sleep_for(chrono::milliseconds(500));
	} while (chrono::system_clock::now() < endpoint);

	return shared_ptr<vector<unsigned int>>();
}

shared_ptr<WIFI_BASE_SESSION> read_first_message(WIFI_INFO & wifi, int message_id)
{
	WIFI_BASE_SESSION sec;

	mk_read_session(wifi, sec, message_id);

	transmit_session(wifi, sec);

	return wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool {return session.code_num == CODE_READ; }, wifi.max_delay_ms_ctrl);
}



void exec_read_message(WIFI_INFO & wifi, int message_id)
{
	auto psec = read_first_message(wifi, message_id);

	if (psec) {
		WIFI_DATA_SUB_PROTOCOL *sub = (WIFI_DATA_SUB_PROTOCOL*)psec->data;
		if (wifi.dbg_pri_rec_fun) {
			printf("receive fun = %d\n", sub->function_id);
		}

		auto fun = FindFunction(wifi, WIFI_BASE_FUNCTION::MASK_READ, sub->function_id);

		while (fun) {
			auto sta = fun->wifi_read(*psec);

			if (sta == WIFI_PRO_NEED_WRITE) {
				fun->wifi_write(*psec);

				transmit_session(wifi, *psec);

				psec = wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool {return session.code_num == CODE_READ; }, wifi.max_delay_ms_ctrl);
			} else {
				fun = nullptr;
			}
		}
	}
}

void exec_write_message(WIFI_INFO & wifi, WIFI_BASE_FUNCTION * fun)
{
	if (!fun) {
		return;
	}

	WIFI_BASE_SESSION sec;

	WIFI_PRO_STATUS sta;
	do {
		fun->wifi_write(sec);

		transmit_session(wifi, sec);

		auto ret = wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool {return session.code_num == CODE_WRITE; }, wifi.max_delay_ms_ctrl);

		sta = fun->wifi_read(sec);
	} while (sta == WIFI_PRO_NEED_WRITE);
}





int wifi_serivce(WIFI_INFO & wifi)
{
	if (0 != wifi_open(wifi)) {
		printf("wifi open fail\n");
		exit(0);
	}

	
	auto rdvec = read_num(wifi, 10 * 1000);

	if (rdvec && rdvec->size()) {
		for (auto p : *rdvec) {
			if (wifi.dbg_pri_wifi_ctrl) printf("get message = %d\n", p);
		}
	} else {
		if (wifi.dbg_pri_wifi_ctrl) printf("get no message\n");
	}

	for (auto & num : *rdvec) {
		exec_read_message(wifi, num);
	}
	exit(0);
	auto itr = wifi.write_fun_list.begin();

	while (itr != wifi.write_fun_list.end()) {
		exec_write_message(wifi, *itr);

		auto tmp = itr;
		itr++;
		(*tmp)->DESTORY_WRITE(wifi);
		wifi.write_fun_list.erase(tmp);
	}
	return 0;
}




