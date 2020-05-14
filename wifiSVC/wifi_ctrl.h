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


enum CODE_ID {
	CODE_INIT = 1,
	CODE_READ_NUM = 2,
	CODE_READ = 3,
	CODE_WRITE = 4,
	CODE_SELF_WRITE = 5,
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


struct WIFI_INFO {
	WIFI_INFO()
	{}
	int uartFD = -1;
	int recRunFlg = 0;
	int max_delay_ms_ctrl = 5 * 1000;
	int max_delay_ms_message = 10 * 1000;
	SN1_SHM * psn1;

	std::list<std::shared_ptr <WIFI_BASE_SESSION> > rec_session_list;
	std::timed_mutex mtx_using_list;
	std::condition_variable_any enable_cv;
	//debug using
	int fake_check_flag = 0;
	int dbg_pri_chk_flag = 0;
	int dbg_pri_rd_len = 0;
	int dbg_pri_rd_word = 0;
};

void init_rec_pro(WIFI_INFO * pwifi);
#endif
