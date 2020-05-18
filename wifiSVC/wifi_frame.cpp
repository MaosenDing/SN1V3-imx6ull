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

	//对时

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

shared_ptr < vector <unsigned int> > read_num(WIFI_INFO & wifi)
{
	WIFI_BASE_SESSION sec;

	mk_read_num_session(wifi, sec);

	transmit_session(wifi, sec);

	auto ret = wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool {return session.code_num == CODE_READ_NUM; }, wifi.max_delay_ms_ctrl);

	if (ret && ret->data_len == 4) {
		unsigned int start = ret->data[0] | ret->data[1] << 8;
		unsigned int end = ret->data[2] | ret->data[3] << 8;
		if (end >= start) {
			auto ret = make_shared<vector<unsigned int>>();
			do {
				ret->push_back(start);
			} while (start++ < end);
			return ret;
		}
	}
	return shared_ptr<vector<unsigned int>>();
}

shared_ptr<WIFI_BASE_SESSION> read_first_message(WIFI_INFO & wifi, int message_id)
{
	WIFI_BASE_SESSION sec;

	mk_read_session(wifi, sec, message_id);

	transmit_session(wifi, sec);

	return wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool {return session.code_num == CODE_READ; }, wifi.max_delay_ms_ctrl);
}

WIFI_PRO_STATUS wifi_test_read(WIFI_INFO & wifi, WIFI_BASE_SESSION & sec);
int wifi_test_write(WIFI_INFO & wifi, WIFI_BASE_SESSION & sec);



void exec_read_message(WIFI_INFO & wifi, int message_id)
{
	while (true) {
		auto fst = read_first_message(wifi, message_id);

		if (fst) {
			WIFI_DATA_SUB_PROTOCOL *sub = (WIFI_DATA_SUB_PROTOCOL*)fst->data;
			if (wifi.dbg_pri_rec_fun) {
				printf("receive fun = %d\n", sub->function_id);
			}

			auto sta = wifi_test_read(wifi, *fst);

			if (sta == WIFI_PRO_NEED_WRITE) {
				wifi_test_write(wifi, *fst);


			}
		}
	}
}



int wifi_serivce(WIFI_INFO & wifi)
{

	wifi_open(wifi);

	auto rdvec = read_num(wifi);

	for (auto & num : *rdvec)
	{
		
	}





	return 0;
}
