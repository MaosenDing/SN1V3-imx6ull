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
using namespace std;


shared_ptr<WIFI_BASE_SESSION> exec_wifi_ctrl(WIFI_INFO & wifi, int code, void *data, int datalen)
{
	WIFI_BASE_SESSION sec;

	sec.code_num = CODE_INIT;

	if (data && datalen > 0) {
		memcpy(sec.data, data, datalen);
		sec.data_len = datalen;
	} else {
		sec.data_len = 0;
	}
	sec.frame_index = 0;
	sec.seq_num = 0;

	transmit_session(wifi, sec);

	auto ret = wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool {return session.code_num == CODE_READ; }, wifi.max_delay_ms_ctrl);

	return ret;
}

int set_ssid(WIFI_INFO & wifi, int grpid, char * wifiname)
{
	char buff[64];
	buff[0] = grpid;
	strcpy(buff + 1, wifiname);
	int sndlen = 1 + strlen(wifiname);
	auto psec = exec_wifi_ctrl(wifi, 0x40, buff, 1 + strlen(wifiname));

	if (psec) {
		if (psec->data_len == 1 + strlen(wifiname)) {
			return 0;
		}
		else {
			return -1;
		}
	}
	return -2;
}

int set_wifi_module(WIFI_INFO & wifi)
{
	set_ssid(wifi, 0, "123");
	exit(0);
}
