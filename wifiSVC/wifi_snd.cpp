
#include "SN1V2_rtConfig.h"
#include <clockd_def.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/prctl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <string.h>
#include <string>
#include <fstream>
#include <regex>
#include <sys/wait.h>
#include "errHandle/errHandle.h"
#include "mem_share.h"
#include "versions.h"
#include "jd_share.h"
#include <iostream>
#include <thread>
#include "svc.h"
#include "SN1V2_com.h"
#include "time_interval.h"
#include "wifi_ctrl.h"
#include <sys/poll.h>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <sys/timerfd.h>
using namespace std;


void mk_read_num_session(WIFI_INFO & wifi, WIFI_BASE_SESSION & session)
{
	session.code_num = CODE_READ_NUM;
	session.frame_index = 0;
	session.data_len = 0;
	session.seq_num = wifi.send_seq;
}


int transmit_session(WIFI_INFO & wifi, WIFI_BASE_SESSION & session)
{
	std::unique_lock<std::mutex> lk(wifi.send_mtx);
	
	wifi.sndbuf[0] = 0xaa;
	wifi.sndbuf[1] = 0xaa;
	wifi.sndbuf[2] = session.code_num;
	wifi.sndbuf[3] = session.seq_num;
	memcpy(wifi.sndbuf + 4, wifi.server_id, 4);
	memcpy(wifi.sndbuf + 8, wifi.this_id, 4);
	wifi.sndbuf[12] = session.frame_index;
	wifi.sndbuf[13] = session.frame_index >> 8;

	memcpy(wifi.sndbuf + 14, session.data, session.data_len);

	uint16_t crc0 = crc_make(wifi.sndbuf, session.data_len + 14, 0xffff);

	wifi.sndbuf[14 + session.data_len] = crc0;
	wifi.sndbuf[15 + session.data_len] = crc0 >> 8;

	return write(wifi.uartFD, wifi.sndbuf, session.data_len + 16);
}

void exce_session(WIFI_INFO & wifi, WIFI_BASE_SESSION & session)
{
	switch (session.code_num) {
	case CODE_INIT:

		break;

	case CODE_READ_NUM:
		transmit_session(wifi, session);
		wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool{return session.code_num & CODE_READ_NUM; });
		
		break;

	case CODE_READ:
		break;

	case CODE_WRITE:
		break;

	case CODE_SELF_WRITE:
		break;
	default:
		break;
	}
}


