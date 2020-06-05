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


static shared_ptr<WIFI_BASE_SESSION> exec_wifi_tim(WIFI_INFO & wifi)
{
	WIFI_BASE_SESSION sec;

	sec.code_num = CODE_INIT;

	sec.frame_index = 0;
	sec.seq_num = 0;

	for (int i = 0; i < MAX_RETRY_EXEC_CTRL; i++) {
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

		if (ret && (ret->data_len == 0)) {
			if (wifi.dbg_pri_wifi_ctrl) printf("get tim waiting ...\n");
		}
	}
	//返回失败
	return shared_ptr<WIFI_BASE_SESSION>();
}


int get_wifi_tim(WIFI_INFO & wifi)
{
	shared_ptr<WIFI_BASE_SESSION> ret;
	
	//do{
		printf("test tim \n");
		ret = exec_wifi_tim(wifi);
	//} while (!ret);

	exit(0);
	

	return 0;
}




