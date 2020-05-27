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

	sec.data[0] = code;
	unsigned char * datstar = &sec.data[1];

	if (data && datalen > 0) {
		memcpy(datstar, data, datalen);
		sec.data_len = datalen;
	} else {
		sec.data_len = 0;
	}
	sec.data_len += 1;

	sec.frame_index = 0;
	sec.seq_num = 0;

	transmit_session(wifi, sec);

	auto ret = wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool {return session.code_num == (CODE_INIT | 0x80); }, wifi.max_delay_ms_ctrl);

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
	}
	return 0;
}

int get_ssid(WIFI_INFO & wifi, int grpid, char * wifiname, int maxLen)
{
	char c8id = grpid;
	auto psec = exec_wifi_ctrl(wifi, 0x41, &c8id, 0);

	if (psec) {
		if (psec->data_len >= 3) {
			int ascllen = psec->data_len - 2;
			memcpy(wifiname, psec->data + 2, ascllen);
			wifiname[psec->data_len] = 0;
		}
	}
	return 0;
}

int set_server(WIFI_INFO & wifi, unsigned char * serverip, int port)
{
	unsigned char buff[6] = { serverip[0],serverip[1],serverip[2],serverip[3],(unsigned char)port ,(unsigned char)port >> 8 };

	auto psec = exec_wifi_ctrl(wifi, 0x42, &buff, 6);
	
	if (psec) {
	}

	return 0;
}

int set_local(WIFI_INFO & wifi, unsigned char * serverip)
{
	unsigned char buff[4] = { serverip[0],serverip[1],serverip[2],serverip[3] };

	auto psec = exec_wifi_ctrl(wifi, 0x43, &buff, 4);

	if (psec) {
	}

	return 0;
}


int set_connect(WIFI_INFO & wifi)
{
	auto psec = exec_wifi_ctrl(wifi, 0x45, nullptr, 0);

	return 0;
}

int set_sleep(WIFI_INFO & wifi)
{
	auto psec = exec_wifi_ctrl(wifi, 0x46, nullptr, 0);

	if (psec) {
	}

	return 0 ;
}

int get_cache(WIFI_INFO & wifi)
{
	auto psec = exec_wifi_ctrl(wifi, 0x48, nullptr, 0);

	if (psec) {
	}
	return 0;
}


int get_status(WIFI_INFO & wifi)
{
	auto psec = exec_wifi_ctrl(wifi, 0x49, nullptr, 0);

	if (psec) {
	}
	return 0;
}


int set_wifi_module(WIFI_INFO & wifi)
{
	set_ssid(wifi, 0, "123");

	char buff[32];

	get_ssid(wifi, 0, buff, 32);

	printf("get ssid = %s\n", buff);



	exit(0);
}




