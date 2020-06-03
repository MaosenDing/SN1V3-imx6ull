
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




static unsigned char crc_check(unsigned int len, unsigned char *Buff, unsigned int firstcrc, unsigned char *match_byte, WIFI_INFO * pwifi)
{
	unsigned int crc0, crc1;

	if (match_byte == NULL) {
		if (len > 2) {
			crc0 = crc_make(Buff, (len - 2), firstcrc);
			crc1 = (Buff[len - 1] << 8) + Buff[len - 2];
		} else
			return 0;
	} else {
		crc0 = crc_make(Buff, len, firstcrc);
		crc1 = (match_byte[1] << 8) + match_byte[0];
	}

#if 0
	printf("\n");
	for (int i = 0; i < len; i++) {
		printf("%02x ", Buff[i]);
	}

	printf("rec =%04x , chk =%04x\n", crc1, crc0);
#endif
	//fake crc just for test
	if (pwifi->fake_check_flag != 0) {
		if (crc1 == 0xffff) {
			if (pwifi->dbg_pri_chk_flag == 2) printf("fake crc flag,crc = %#x \n", crc0);
			return 1;
		}
	}
	if (crc0 == crc1)
		return 1;
	else
		return 0;
}

static inline shared_ptr<WIFI_BASE_SESSION> make_receive_session(unsigned char * rxbuff, int num)
{
	auto ret = make_shared<WIFI_BASE_SESSION>();

	gettimeofday(&ret->tv, nullptr);

	ret->code_num = rxbuff[2];

	ret->seq_num = rxbuff[3];

	ret->frame_index = rxbuff[12] | (rxbuff[13] << 8);

	ret->pack_len = rxbuff[14] | (rxbuff[15] << 8);

	ret->data_len = ret->pack_len - MIN_PACK_SZ;

	memcpy(ret->data, rxbuff + 16, num - MIN_PACK_SZ);

	return ret;
}




static void wifi_pro_bare_buff(unsigned char * rxbuf, int num, WIFI_INFO * pwifi, int &remove_len)
{
	for (int i = 0; i < num; i++) {
		int remainLen = num - i;
		if ((rxbuf[i] == 0XAA) && (rxbuf[i + 1] == 0XAA)) {
			if (remainLen >= MIN_PACK_SZ) {
				int recpackLen = rxbuf[i + 14] | (rxbuf[i + 15] << 8);
				if (recpackLen >= MIN_PACK_SZ && recpackLen <= MAX_PACK_SZ && remainLen >= recpackLen) {

					if (crc_check(recpackLen, &(*(rxbuf + i)), 0XFFFF, NULL, pwifi) == 1) {
						if (pwifi->dbg_pri_chk_flag) printf("crc ok\n");

						auto pack = make_receive_session(rxbuf + i, recpackLen);

						if (pack && (pack->code_num & 0xf0)) {

							if (pwifi->dbg_pri_useful) {
								printf("useful buff len = %d,", recpackLen);
								disp_x_buff(rxbuf + i, recpackLen);
							}
							unique_lock <timed_mutex> lck(pwifi->mtx_using_list);

							pwifi->rec_session_list.push_back(std::move(pack));

							pwifi->enable_cv.notify_all();
						}
						remove_len = i + recpackLen;
					} else {
						if (pwifi->dbg_pri_chk_flag) printf("crc error\n");
					}
				}
			}
		}
	}
}


shared_ptr<WIFI_BASE_SESSION> wait_rec_session(WIFI_INFO & wifi, bool(*ChkSession)(WIFI_BASE_SESSION &), int milliseconds)
{
	unique_lock<timed_mutex> lck(wifi.mtx_using_list);

	chrono::time_point<std::chrono::system_clock> endpoint = chrono::system_clock::now() + chrono::milliseconds(milliseconds);

	do {
		auto itr = wifi.rec_session_list.begin();

		while (itr != wifi.rec_session_list.end()) {

			if ((*itr) && (itr->use_count()) && (ChkSession(**itr))) {
				shared_ptr<WIFI_BASE_SESSION> ret(move(*itr));
				wifi.rec_session_list.erase(itr);
				return ret;
			}
		}
	} while (cv_status::timeout != wifi.enable_cv.wait_until(lck, endpoint));

	return shared_ptr<WIFI_BASE_SESSION>();
}

void Wifi_rec_thread(WIFI_INFO * pwifi)
{
	if (!pwifi) {
		return;
	}

	enum {
		MAX_RX_BUFF = 1024 * 10,
	};
	unsigned char  * rxbuf = new unsigned char[MAX_RX_BUFF];
	unique_ptr<unsigned char[]> ppp(rxbuf, default_delete<unsigned char[]>());
	int rxlen = 0;

	while (pwifi->recRunFlg == wifi_runing) {
		pollfd pfd[1];
		//receive fd
		pfd[0].fd = pwifi->uartFD;
		pfd[0].events = POLLIN;
		int ret;
		if ((ret = poll(pfd, 1, 20)) > 0) {

			int newReadLen = read(pfd[0].fd, rxbuf + rxlen, MAX_RX_BUFF - rxlen);
			if (newReadLen < 0) {
				continue;
			}

			rxlen += newReadLen;
			timeval tv;
			gettimeofday(&tv, nullptr);

			if (pwifi->dbg_pri_rd_len) printf("rd len = %d time=%ld.%06ld\n", rxlen, tv.tv_sec, tv.tv_usec);

			if (pwifi->dbg_pri_rd_word)disp_x_buff(rxbuf, rxlen);

			int removed_Len = 0;

			wifi_pro_bare_buff(rxbuf, rxlen, pwifi, removed_Len);

			if (removed_Len && removed_Len <= rxlen) {
				rxlen = rxlen - removed_Len;
				memcpy(rxbuf, rxbuf + removed_Len, rxlen);
			}
		}
	}
	pwifi->recRunFlg = wifi_run_stop;
}

void init_rec_pro(WIFI_INFO * pwifi)
{
	pwifi->recRunFlg = wifi_runing;
	thread tp(Wifi_rec_thread, pwifi);
	tp.detach();
}

int close_rec_pro(WIFI_INFO * pwifi)
{
	pwifi->recRunFlg = wifi_run_try_stop;

	for (int i = 0; i < 10; i++) {
		this_thread::sleep_for(chrono::milliseconds(100));
		if (pwifi->recRunFlg == wifi_run_stop) {
			return 0;
		}
	}
	return -1;
}