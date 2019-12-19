#include <JDcomhead.h>
#include <iostream>
#include <thread> 
#include <sys/poll.h>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>
#include <map>
#include "jd_share.h"
using namespace std;

static long diffMS(timeval & tstart, timeval & tend)
{
	long sec = tend.tv_sec - tstart.tv_sec;
	long mil = tend.tv_usec - tstart.tv_usec;

	return sec * 1000 + mil / 1000;
}






static unsigned char crc_check(unsigned int len, unsigned char *Buff, unsigned int firstcrc, unsigned char *match_byte,JD_INFO & jif)
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
	if (jif.fake_check_flag != JD_OK)
	{
		if (crc1 == 0xffff)
		{
			if (jif.dbg_pri_chk_flag == 2 && jif.dbg_fp) fprintf(jif.dbg_fp,"fake crc flag,crc = %#x \n", crc0);
			return 1;
		}
	}
	if (crc0 == crc1)
		return 1;
	else
		return 0;
}


typedef union
{
	struct
	{
		unsigned char low_byte;
		unsigned char high_byte;
	}byte_value;
	unsigned short value;
}MY_USHORT;

int JD_send(JD_INFO & jif, JD_FRAME & jfr)
{
	unsigned char AnsBuffer[256] = { 0 };
	static int thisseq;
	AnsBuffer[0] = 0xAA;
	AnsBuffer[1] = 0xAA;
	AnsBuffer[2] = jfr.jd_command;
	//AnsBuffer[3] = jfr.seq;
	AnsBuffer[3] = thisseq++;

	AnsBuffer[4] = jfr.jd_aim.byte_value.low_byte;
	AnsBuffer[5] = jfr.jd_aim.byte_value.mlow_byte;
	AnsBuffer[6] = jfr.jd_aim.byte_value.mhigh_byte;

	int wrlen = 10 + jfr.jd_data_len;
	if (wrlen > 0xff)
	{
		return JD_ERROR_LEN;
	}

	AnsBuffer[7] = wrlen;

	if (jfr.jd_data_len > 0)
	{
		memcpy(&AnsBuffer[8], jfr.jd_data_buff, jfr.jd_data_len);
	}

	unsigned int mkcrc = crc_make((unsigned char*)AnsBuffer, wrlen - 2, 0xFFFF);
	AnsBuffer[wrlen - 2] = mkcrc & 0xff;
	AnsBuffer[wrlen - 1] = mkcrc >> 8 & 0xff;

	wrlen = write(jif.fd, AnsBuffer, wrlen);

	timeval tv;
	gettimeofday(&tv, nullptr);

	if (jif.dbg_pri_snd_len && jif.dbg_fp) fprintf(jif.dbg_fp, "sd len = %d time=%ld.%06ld\n", wrlen, tv.tv_sec, tv.tv_usec);
	if (jif.dbg_pri_snd_word && jif.dbg_fp) disp_x_buff(jif.dbg_fp, AnsBuffer, wrlen);

	return wrlen;
}


static int JD_default_response(JD_INFO & jif, JD_FRAME & jfr)
{
	if (jif.dbg_pri_error_cmd) fprintf(jif.dbg_fp, "unknown command = %#02x\n", jfr.jd_command);
	return JD_UNKNOWN_COMMAND;
}

JD_INFO::JD_INFO()
{
	default_err_cmd = JD_default_response;

	mdcCtrl[0].addr = 0xaaaa << 8;
	mdcCtrl[1].addr = 0xbbbb << 8;

	mdcCtrl[0].parget.cpl_flag = 0;
	mdcCtrl[1].parget.cpl_flag = 0;
}

void JD_pro_ctl(JD_INFO & jif,int cmd ,JD_INFO::JD_PRO profun, int ctl)
{
	std::unique_lock<std::mutex> lk(jif.pro_mtd_mtx);

	if (ctl)
	{
		jif.jd_pro_method[cmd] = profun;
	}
	else
	{
		jif.jd_pro_method.erase(cmd);
	}
}


static int JD_command_respon(JD_INFO & jif, JD_FRAME & jfr)
{
	try
	{
		JD_INFO::JD_PRO profun = nullptr;
		{
			std::unique_lock<std::mutex> lk(jif.pro_mtd_mtx);
			profun = jif.jd_pro_method.at(jfr.jd_command);
		}
		return profun(jif,jfr);
	}
	catch (out_of_range & p)
	{
		return jif.default_err_cmd(jif, jfr);
	}
}

static void make_rec_pack(unsigned char * rxbuf, int num, JD_FRAME & jfr)
{
	//recoder the head
	jfr.jd_frame_head = rxbuf;
	//jd command
	jfr.jd_command = rxbuf[2];
	jfr.seq = rxbuf[3];
	//aim address
	//maybe the device address
	jfr.jd_aim.byte_value.low_byte = rxbuf[4];  
	jfr.jd_aim.byte_value.mlow_byte = rxbuf[5];
	jfr.jd_aim.byte_value.mhigh_byte = rxbuf[6];
	jfr.jd_aim.byte_value.high_byte = 0x00;
	//jd data load
	jfr.jd_data_len = rxbuf[7] - 10;
	if (jfr.jd_data_len > 0)
	{
		jfr.jd_data_buff = &rxbuf[8];
	}
	else
	{
		jfr.jd_data_buff = nullptr;
	}
}

static int JD_pro_bare_buff(unsigned char * rxbuf, int num, JD_INFO & jif, int *remove_len)
{
	for (int i = 0; i < num; i++) {
		int remainLen = num - i;
		if ((rxbuf[i] == 0XAA) && (rxbuf[i + 1] == 0XAA)) {
			if (remainLen > 7) {
				int recpackLen = rxbuf[i + 7];

				if (crc_check(recpackLen, &(*(rxbuf + i)), 0XFFFF, NULL, jif) == 1) {
					if (jif.dbg_pri_chk_flag && jif.dbg_fp) fprintf(jif.dbg_fp, "crc ok\n");
					JD_FRAME jfr;
					make_rec_pack(rxbuf + i, num - i, jfr);

					int ret = JD_command_respon(jif, jfr);

					if (ret == JD_UNKNOWN_COMMAND) {
						if (remove_len) {
							*remove_len = i + recpackLen;
						}
						continue;
					}

					if (ret == JD_CLOSE_FRAME) {
						return JD_CLOSE_FRAME;
					}
					return ret;
				} else {
					if (jif.dbg_pri_chk_flag && jif.dbg_fp) fprintf(jif.dbg_fp, "crc error\n");
				}
			}
		}
	}
	return JD_CONTINUE;
}



int JD_run_poll(JD_INFO& jif, int TimeOutMS)
{
	struct timeval tStart, tEnd;
	gettimeofday(&tStart, nullptr);

	enum {
		MAX_RX_BUFF = 300,
		RX_MAX_ONCE = 256,
	};
	unsigned char rxbuf[MAX_RX_BUFF];
	int rxlen = 0;

	while (true) {
		gettimeofday(&tEnd, nullptr);

		long dms = diffMS(tStart, tEnd);

		if (TimeOutMS < 0) {//no time out
		 //do nothing
		} else if (dms > TimeOutMS) {
			cout << "diff ms" << dms << endl;
			return JD_TIME_OUT;
		}

		pollfd pfd[1];
		//receive fd
		pfd[0].fd = jif.fd;
		pfd[0].events = POLLIN;
		int ret;
		if ((ret = poll(pfd, 1, 20)) > 0) {
			unsigned char newrxbuf[RX_MAX_ONCE];
			int newReadLen = read(jif.fd, newrxbuf, RX_MAX_ONCE);

			if (newReadLen + rxlen > MAX_RX_BUFF) {
				int reduceNum = newReadLen + rxlen - MAX_RX_BUFF;
				int cpNum = rxlen - reduceNum;
				memcpy(&rxbuf[0], &rxbuf[reduceNum], cpNum);
				rxlen = cpNum;
			}
			memcpy(&rxbuf[rxlen], &newrxbuf[0], newReadLen);
			rxlen += newReadLen;
			timeval tv;
			gettimeofday(&tv, nullptr);

			if (jif.dbg_pri_rd_len && jif.dbg_fp) fprintf(jif.dbg_fp, "rd len = %d time=%ld.%06ld\n", rxlen, tv.tv_sec, tv.tv_usec);

			if (jif.dbg_pri_rd_word && jif.dbg_fp)disp_x_buff(jif.dbg_fp, rxbuf, rxlen);

			int removed_Len = 0;

			int retPro = JD_pro_bare_buff(rxbuf, rxlen, jif, &removed_Len);

			if (JD_CLOSE_FRAME == retPro) {
				return JD_CLOSE_FRAME;
			}

			if (JD_CONTINUE == retPro) {
			}

			if (JD_OK == retPro) {
				jif.enable_cv.notify_all();
				rxlen = 0;
			}

			if (removed_Len && removed_Len < rxlen) {
				rxlen = rxlen - removed_Len;
				memcpy(rxbuf, rxbuf + removed_Len, rxlen);
			}
		}
	}
}













