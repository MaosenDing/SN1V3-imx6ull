
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


static int wifi_pro_bare_buff(unsigned char * rxbuf, int num, WIFI_INFO * pwifi, int &remove_len)
{
	for (int i = 0; i < num; i++) {
		int remainLen = num - i;
		if ((rxbuf[i] == 0XAA) && (rxbuf[i + 1] == 0XAA)) {
			if (remainLen > 7) {
				int recpackLen = rxbuf[i + 7];

				if (crc_check(recpackLen, &(*(rxbuf + i)), 0XFFFF, NULL, pwifi) == 1) {
					if (pwifi->dbg_pri_chk_flag) printf("crc ok\n");
					{
						


					}
					//JD_FRAME jfr;
					//make_rec_pack(rxbuf + i, num - i, jfr);

					//int ret = JD_command_respon(jif, jfr);

					//if (ret == JD_UNKNOWN_COMMAND) {
					//	remove_len = i + recpackLen;
					//	continue;
					//}

					//if (ret == JD_CLOSE_FRAME) {
					//	return JD_CLOSE_FRAME;
					//}
					return 0;
				} else {
					if (pwifi->dbg_pri_chk_flag) printf("crc error\n");
				}
			}
		}
	}
	return 0;
}



void Wifi_rec_thread(WIFI_INFO * pwifi)
{
	if (!pwifi) {
		return;
	}

	enum {
		MAX_RX_BUFF = 1024 * 10,
		RX_MAX_ONCE = 256,
	};
	unsigned char  * rxbuf = new unsigned char[MAX_RX_BUFF];
	unique_ptr<unsigned char[]> ppp(rxbuf, default_delete<unsigned char[]>());
	int rxlen = 0;

	while (pwifi->recRunFlg) {
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

			if (pwifi->dbg_pri_rd_len ) printf("rd len = %d time=%ld.%06ld\n", rxlen, tv.tv_sec, tv.tv_usec);

			if (pwifi->dbg_pri_rd_word )disp_x_buff(rxbuf, rxlen);

			int removed_Len = 0;

			int retPro = wifi_pro_bare_buff(rxbuf, rxlen, pwifi, removed_Len);

			if (removed_Len && removed_Len < rxlen) {
				rxlen = rxlen - removed_Len;
				memcpy(rxbuf, rxbuf + removed_Len, rxlen);
			}
		}
	}
}



