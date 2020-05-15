#ifndef __wifi_ctrl_h____
#define __wifi_ctrl_h____

#include "SN1V2_rtConfig.h"
#include <vector>
#include <list>
#include <memory>
#include <mutex>
#include <sys/time.h>
#include <vector>
#include <map>
#include <stdio.h>
#include <condition_variable>
#include <sys/time.h>

enum WIFI_ERR {
	wifi_ok = 0,
	wifi_disconnect = -1,
	wifi_send_error = -2,
	wifi_read_error = -3,
	wifi_pack_error = -4,
};


enum CODE_ID {
	CODE_INIT = 1,
	CODE_READ_NUM = 2,
	CODE_READ = 3,
	CODE_WRITE = 4,
	CODE_SELF_WRITE = 5,
#if 0
	CODE_INIT_MASK = 1 << (CODE_INIT + 1),
	CODE_READ_NUM_MASK = 1 << (CODE_READ_NUM + 1),
	CODE_READ_MASK = 1 << (CODE_READ + 1),
	CODE_WRITE_MASK = 1 << (CODE_WRITE + 1),
	CODE_SELF_WRITE_MASK = 1 << (CODE_SELF_WRITE + 1),
#endif
	CODE_ERR = -1,
};





struct WIFI_BASE_SESSION {
	timeval tv;
	int code_num = CODE_ERR;//命令编码 3 4 5
	int seq_num = 0;//帧顺序码 防止重复
	int frame_index;//帧编号

	int data_len = 0;
	unsigned char data[1000];
};

enum wifi_run_flg {
	wifi_run_null = 0,
	wifi_run_stop = -1,
	wifi_run_try_stop = -2,
	wifi_runing = 1,
};


struct WIFI_INFO {
	WIFI_INFO()
	{}
	int uartFD = -1;
	wifi_run_flg recRunFlg = wifi_run_null;
	int max_delay_ms_ctrl = 5 * 1000;
	int max_delay_ms_message = 10 * 1000;	

	unsigned char this_id[4] = { 0x1, 0x2,0x3,0x4 };
	const unsigned char server_id[4] = { 0x62,0x27,0x21,0x55 };

	SN1_SHM * psn1;

	std::list<std::shared_ptr <WIFI_BASE_SESSION> > rec_session_list;
	std::timed_mutex mtx_using_list;
	std::condition_variable_any enable_cv;

	//using in transmit
	std::mutex send_mtx;
	unsigned char sndbuf[1024];
	unsigned char send_seq = 0;
	//debug using
	int fake_check_flag = 0;
	int dbg_pri_chk_flag = 0;
	int dbg_pri_rd_len = 0;
	int dbg_pri_rd_word = 0;
};

//开、关接收服务
void init_rec_pro(WIFI_INFO * pwifi);
int close_rec_pro(WIFI_INFO * pwifi);


int wifi_serivce(WIFI_INFO & wifi);


std::shared_ptr<WIFI_BASE_SESSION> wait_rec_session(WIFI_INFO & wifi, bool(*ChkSession)(WIFI_BASE_SESSION &),int milliseconds);
#endif
