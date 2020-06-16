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
			}, wifi.max_delay_ms_muc_response);

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
		this_thread::sleep_for(chrono::milliseconds(wifi.max_delay_ms_muc_response));
	} while (chrono::system_clock::now() < endpoint);

	return shared_ptr<vector<unsigned int>>();
}

shared_ptr<WIFI_BASE_SESSION> read_first_message(WIFI_INFO & wifi, int message_id, int Milliseconds)
{
	WIFI_BASE_SESSION sec;

	mk_read_session(wifi, sec, message_id);
	chrono::time_point<std::chrono::system_clock> endpoint = chrono::system_clock::now() + chrono::milliseconds(Milliseconds);

	do {
		transmit_session(wifi, sec);
		auto ret = wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool {return session.code_num == (CODE_READ | 0x80); }
		, wifi.max_delay_ms_muc_response);

		if (ret && ret->data_len >= 2) {
			if (ret->frame_index == -2) {
			}
			if (ret->frame_index >= -1) {
				return ret;
			}
		}
		this_thread::sleep_for(chrono::milliseconds(wifi.max_delay_ms_muc_response));
	} while (chrono::system_clock::now() < endpoint);
	return make_shared<WIFI_BASE_SESSION>();
}

int mk_WIFI_DATA_SUB_PROTOCOL(WIFI_BASE_SESSION & sesssion, WIFI_DATA_SUB_PROTOCOL & pro)
{
	if (sesssion.data_len < 3) {
		return -1;
	}

	pro.message_id = sesssion.data[0] | (sesssion.data[1] << 8);
	pro.function_id = sesssion.data[2];
	if (sesssion.data_len > 3) {
		pro.function_data = &sesssion.data[3];
		pro.datalen = sesssion.data_len - 3;
	}

	return 0;
}

void exec_read_message(WIFI_INFO & wifi, int message_id)
{
	auto psec = read_first_message(wifi, message_id, wifi.max_delay_ms_session_response);

	if (psec) {
		WIFI_DATA_SUB_PROTOCOL sub;
		if (0 == mk_WIFI_DATA_SUB_PROTOCOL(*psec, sub)) {
			if (wifi.dbg_pri_wifi_data) {
				printf("msg id = %d ,receive fun = %d,datlen = %d\n"
					, sub.message_id, sub.function_id, sub.datalen);
			}
		}

		auto fun = FindFunction(wifi, WIFI_BASE_FUNCTION::MASK_READ, sub.function_id);

		if (fun) {
			if (wifi.dbg_pri_msg && fun->FUNCTION_NAME()) {
				printf("rec fun name = %s\n", fun->FUNCTION_NAME());
			}
			fun->wifi_read(*psec);
		}
	}
}

int exec_write_message(WIFI_INFO & wifi, WIFI_BASE_FUNCTION * fun)
{
	if (!fun) {
		//发送节点有问题
		//删除
		return 0;
	}

	WIFI_BASE_SESSION sec;

	WIFI_PRO_STATUS sta;
	do {
		fun->wifi_write(sec);
		sec.code_num = 0x04;
		chrono::time_point<std::chrono::system_clock> endpoint = chrono::system_clock::now() + chrono::milliseconds(wifi.max_delay_ms_session_response * 100);
		shared_ptr<WIFI_BASE_SESSION> ret;
		do {
			transmit_session(wifi, sec);
			ret = wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool {return session.code_num == (CODE_WRITE | 0x80); }
			, wifi.max_delay_ms_muc_response);

			if (ret && ret->frame_index > -2) {
				break;
			}
			this_thread::sleep_for(chrono::milliseconds(wifi.max_delay_ms_muc_response));
		} while (chrono::system_clock::now() < endpoint);

		if (ret && ret->frame_index <= -2) {
			//发送错误  下次再发送
			return -1;
		}
		sta = fun->wifi_read(*ret);
	} while (sta == WIFI_PRO_NEED_WRITE);
	//完成
	//删除
	return 0;
}


int exec_download_message(WIFI_INFO & wifi, WIFI_BASE_FUNCTION * fun)
{
	if (!fun) {
		//发送节点有问题
		//删除
		return 0;
	}

	WIFI_BASE_SESSION sec;

	WIFI_PRO_STATUS sta;
	do {
		fun->wifi_write(sec);
		sec.code_num = 0x05;
		chrono::time_point<std::chrono::system_clock> endpoint = chrono::system_clock::now() + chrono::milliseconds(wifi.max_delay_ms_session_response*1000);
		shared_ptr<WIFI_BASE_SESSION> ret;
		do {
			transmit_session(wifi, sec);
			ret = wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool {return session.code_num == (CODE_SELF_WRITE | 0x80); }
			, wifi.max_delay_ms_muc_response);

			if (ret && ret->frame_index > -2) {
				break;
			}
			this_thread::sleep_for(chrono::milliseconds(wifi.max_delay_ms_muc_response));
		} while (chrono::system_clock::now() < endpoint);

		if (ret && ret->frame_index <= -2) {
			//发送错误  下次再发送
			return -1;
		}
		sta = fun->wifi_read(*ret);
	} while (sta == WIFI_PRO_NEED_WRITE);
	//完成
	//删除
	return 0;
}


void exec_write_stage(WIFI_INFO & wifi)
{
	if (wifi.dbg_pri_msg)printf("write stage\n");
	auto itr = wifi.write_fun_list.begin();

	while (itr != wifi.write_fun_list.end()) {
		if ((*itr)->GetProMask() & WIFI_BASE_FUNCTION::MASK_WRITE) {
			printf("exec write = %s\n", (*itr)->FUNCTION_NAME());
			if (0 == exec_write_message(wifi, *itr)) {
				std::unique_lock<std::mutex> lk(wifi.mtx_write_fun_list);
				auto tmp = itr;
				++itr;
				(*tmp)->DESTORY_WRITE(wifi);
				wifi.write_fun_list.erase(tmp);
			} else {
				++itr;
			}
		} else {
			++itr;
		}
	}
}

void exec_read_stage(WIFI_INFO & wifi)
{
	if (wifi.dbg_pri_msg)printf("read stage\n");
	auto itr = wifi.write_fun_list.begin();

	while (itr != wifi.write_fun_list.end()) {
		if ((*itr)->GetProMask() & WIFI_BASE_FUNCTION::MASK_SELF_DOWNLOAD) {
			printf("exec write = %s\n", (*itr)->FUNCTION_NAME());
			if (0 == exec_download_message(wifi, *itr)) {
				std::unique_lock<std::mutex> lk(wifi.mtx_write_fun_list);
				auto tmp = itr;
				++itr;
				(*tmp)->DESTORY_WRITE(wifi);
				wifi.write_fun_list.erase(tmp);
			} else {
				++itr;
			}
		} else {
			++itr;
		}
	}
}



int wifi_serivce(WIFI_INFO & wifi)
{
	if (0 != wifi_open(wifi)) {
		printf("wifi open fail\n");
		exit(0);
	}

	auto rdvec = read_num(wifi, wifi.max_delay_ms_session_response);

	if (rdvec && rdvec->size()) {
		for (auto p : *rdvec) {
			if (wifi.dbg_pri_wifi_data) printf("get message = %d\n", p);
		}
	} else {
		if (wifi.dbg_pri_wifi_data) printf("get no message\n");
	}

	for (auto & num : *rdvec) {
		exec_read_message(wifi, num);
	}

	exec_write_stage(wifi);

	exec_read_stage(wifi);

	exit(0);
	return 0;
}




