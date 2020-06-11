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
#include <string.h>
#include <chrono>
using namespace std;


static shared_ptr<WIFI_BASE_SESSION> exec_wifi_tim(WIFI_INFO & wifi, int maxMS)
{
	WIFI_BASE_SESSION sec;

	sec.code_num = CODE_INIT;

	sec.frame_index = 0;
	sec.seq_num = 0;

	chrono::time_point<std::chrono::system_clock> endpoint = chrono::system_clock::now() + chrono::milliseconds(maxMS);

	do {
		transmit_session(wifi, sec);
		shared_ptr<WIFI_BASE_SESSION>  ret = wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool {return (session.data[0] | 0x80) && session.code_num == (CODE_INIT | 0x80); }, wifi.max_delay_ms_ctrl);
		if (ret && (ret->data_len == 8)) {
			if (wifi.dbg_pri_wifi_ctrl) printf("get tim ok = %d-%d-%d %d:%d:%d,%d\n",
				ret->data[0], ret->data[1], ret->data[2],
				ret->data[3], ret->data[4], ret->data[5],
				ret->data[6] | (ret->data[7] << 8)
			);
			return ret;
		}

		if (!ret) {
			if (wifi.dbg_pri_wifi_ctrl) printf("get tim ,no response\n");
		}

		if (ret && (ret->data_len == 0)) {
			if (wifi.dbg_pri_wifi_ctrl) printf("get tim waiting ...\n");
		}
		this_thread::sleep_for(chrono::milliseconds(500));
	} while (chrono::system_clock::now() < endpoint);
	//返回失败
	return shared_ptr<WIFI_BASE_SESSION>();
}


int get_wifi_tim(WIFI_INFO & wifi)
{
	shared_ptr<WIFI_BASE_SESSION> ret;

	printf("test tim \n");
	ret = exec_wifi_tim(wifi, 60 * 1000);
	if (ret) {
		printf("tim get ok\n");
		return 0;
	}
	return -1;
}




