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
#define MAX_SSID_GRP (3) //mcu 上最多有三组ssid
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

	for (int i = 0; i < MAX_RETRY_EXEC_CTRL; i++) {
		transmit_session(wifi, sec);
		auto ret = wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool {return (session.data[0] | 0x80) && session.code_num == (CODE_INIT | 0x80); }, wifi.max_delay_ms_ctrl);
		if (ret && (ret->data[0] == (code | 0x80))) {
			return ret;
		}
	}
	//返回失败
	return shared_ptr<WIFI_BASE_SESSION>();
}

int set_ssid(WIFI_INFO & wifi, int grpid, const char * wifiname)
{
	char buff[64];
	if (grpid >= MAX_SSID_GRP) {
		return -2;
	}

	buff[0] = grpid;
	strcpy(buff + 1, wifiname);
	int sndlen = 1 + strlen(wifiname);
	auto psec = exec_wifi_ctrl(wifi, 0x40, buff, sndlen);
	if (wifi.dbg_pri_wifi_ctrl) printf("set grp %d ssid = '%s'", grpid, wifiname);
	if (psec) {
		//成功
		if (wifi.dbg_pri_wifi_ctrl)printf(",success\n");
		return 0;
	}
	if (wifi.dbg_pri_wifi_ctrl)printf(",fail\n");
	return -1;
}

int set_pwd(WIFI_INFO & wifi, int grpid, const char * pwd)
{
	char buff[64];
	if (grpid >= MAX_SSID_GRP) {
		return -2;
	}

	buff[0] = grpid + MAX_SSID_GRP;
	strcpy(buff + 1, pwd);
	int sndlen = 1 + strlen(pwd);
	auto psec = exec_wifi_ctrl(wifi, 0x40, buff, sndlen);
	if (wifi.dbg_pri_wifi_ctrl) printf("set grp %d pwd = '%s'", grpid, pwd);
	if (psec) {
		//成功
		if (wifi.dbg_pri_wifi_ctrl)printf(",success\n");
		return 0;
	}
	if (wifi.dbg_pri_wifi_ctrl)printf(",fail\n");
	return -1;
}


int get_ssid(WIFI_INFO & wifi, int grpid, char * wifiname, int maxLen)
{
	if (grpid >= MAX_SSID_GRP) {
		return -2;
	}


	char c8id = grpid;
	auto psec = exec_wifi_ctrl(wifi, 0x41, &c8id, 1);

	if (psec) {
		if (psec->data_len >= 3) {
			int ascllen = psec->data_len - 2;
			memcpy(wifiname, psec->data + 2, ascllen);
			wifiname[psec->data_len] = 0;
			if (wifi.dbg_pri_wifi_ctrl)printf("get grp %d ssid = '%s'\n", grpid, wifiname);
			return 0;
		}
	}
	if (wifi.dbg_pri_wifi_ctrl)printf("get grp %d ssid fail\n", grpid);
	return -1;
}

int get_pwd(WIFI_INFO & wifi, int grpid, char * pwd, int maxLen)
{
	if (grpid >= MAX_SSID_GRP) {
		return -2;
	}

	char c8id = grpid + MAX_SSID_GRP;
	auto psec = exec_wifi_ctrl(wifi, 0x41, &c8id, 1);

	if (psec) {
		if (psec->data_len >= 3) {
			int ascllen = psec->data_len - 2;
			memcpy(pwd, psec->data + 2, ascllen);
			pwd[psec->data_len] = 0;
			if (wifi.dbg_pri_wifi_ctrl)printf("get grp %d pwd = '%s'\n", grpid, pwd);
			return 0;
		}
	}
	if (wifi.dbg_pri_wifi_ctrl)printf("get grp %d pwd fail\n", grpid);
	return -1;
}



int set_server(WIFI_INFO & wifi, unsigned char * serverip, int port)
{
	unsigned char buff[6] = { serverip[0],serverip[1],serverip[2],serverip[3],(unsigned char)port ,(unsigned char)(port >> 8) };

	auto psec = exec_wifi_ctrl(wifi, 0x42, &buff, 6);

	if (psec) {
		int datlen = psec->data_len - 1;
		if (datlen == 6) {
			if (wifi.dbg_pri_wifi_ctrl)printf("set remote ip = %d:%d:%d:%d,port = %d\n"
				, serverip[0]
				, serverip[1]
				, serverip[2]
				, serverip[3]
				,port				
			);
			return 0;
		}
		printf("len = %d\n",datlen);
	}
	if (wifi.dbg_pri_wifi_ctrl)printf("set ip fail\n");
	return -1;
}

int get_server(WIFI_INFO & wifi, unsigned char * serverip, int &port)
{
	auto psec = exec_wifi_ctrl(wifi, 0x42, nullptr, 0);

	if (psec) {
		int datlen = psec->data_len - 1;
		if (datlen == 6) {
			unsigned char * serverip = &psec->data[1];
			unsigned char * portpos = serverip + 4;
			port = portpos[0] | portpos[1] << 8;
			if (wifi.dbg_pri_wifi_ctrl)printf("get remote ip = %d:%d:%d:%d,port = %d\n"
				, serverip[0]
				, serverip[1]
				, serverip[2]
				, serverip[3]
				, port
			);
			return 0;
		}
		printf("len = %d\n", datlen);
	}
	if (wifi.dbg_pri_wifi_ctrl)printf("get ip fail\n");
	return -1;
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
	set_ssid(wifi, 0, "XINCHEN-2.4G");
	set_pwd(wifi, 0, "1000000001");
	char buff[32];
	get_ssid(wifi, 0, buff, 32);
	get_pwd(wifi, 0, buff, 32);

	unsigned char ip[] = { 192,168,1,205 };
	set_server(wifi, ip, 8888);
	unsigned char serverip[4];
	int port;
	get_server(wifi, serverip, port);

	return 0;
}




