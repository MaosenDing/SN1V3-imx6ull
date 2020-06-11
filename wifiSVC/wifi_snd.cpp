
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
	session.seq_num = wifi.send_seq++;
}

void mk_read_session(WIFI_INFO & wifi, WIFI_BASE_SESSION & session, int message_id, int pack_index = 0)
{
	session.code_num = CODE_READ;
	session.frame_index = pack_index;
	session.seq_num = wifi.send_seq++;

	session.data_len = 2 ;
	session.data[0] = (message_id >> 0) & 0xff;
	session.data[1] = (message_id >> 8) & 0xff;
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

	session.pack_len = session.data_len + MIN_PACK_SZ;
	int sndlen = session.pack_len;

	wifi.sndbuf[14] = sndlen;
	wifi.sndbuf[15] = sndlen >> 8;

	memcpy(wifi.sndbuf + 16, session.data, session.pack_len);

	uint16_t crc0 = crc_make(wifi.sndbuf, sndlen - 2, 0xffff);

	wifi.sndbuf[sndlen - 2] = crc0;
	wifi.sndbuf[sndlen - 1] = crc0 >> 8;

	if (wifi.dbg_pri_snd) {
		printf("snd len = %d,", sndlen);
		disp_x_buff(wifi.sndbuf, sndlen);
	}
	return write(wifi.uartFD, wifi.sndbuf, sndlen);
}

