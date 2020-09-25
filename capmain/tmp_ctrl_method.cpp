
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include "SN1V2_com.h"
#include "timSet.h"
#include <algorithm>
#include "time_interval.h"
#include <sys/stat.h>
#include "errHandle/errHandle.h"
#include "tableWork.h"
#include <thread>
#include <unistd.h>
#include <chrono>
#include <regex>
#include <sys/prctl.h>
#include "camera.h"
//#include "communicate.h"
#include "errHandle/errHandle.h"
#include <jd_share.h>
#include <iostream>
#include <math.h>
#include "timeTableV2.h"
#include <fstream>


#include "SunPosTable.h"
#include "sn1v3cfg.h"
#include "tableWork.h"
#include "configOperator.h"
#include "JDcomhead.h"
#include "jd_share.h"
#include "SN1V2_error.h"
#include "errHandle.h"


using namespace std;


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
	if (jfr.jd_data_len > 0) {
		jfr.jd_data_buff = &rxbuf[8];
	} else {
		jfr.jd_data_buff = nullptr;
	}
}

shared_ptr< JD_FRAME> JD_pro_bare_buff(unsigned char * rxbuf, int num, JD_INFO & jif)
{
	for (int i = 0; i < num; i++) {
		int remainLen = num - i;
		if ((rxbuf[i] == 0XAA) && (rxbuf[i + 1] == 0XAA)) {
			if (remainLen > 7) {
				int recpackLen = rxbuf[i + 7];

				if (crc_check(recpackLen, &(*(rxbuf + i)), 0XFFFF) == 1) {
					if (jif.dbg_pri_chk_flag && jif.dbg_fp) fprintf(jif.dbg_fp, "crc ok\n");
					auto jfr = make_shared< JD_FRAME>();
					make_rec_pack(rxbuf + i, num - i, *jfr);

					return jfr;
				} else {
					if (jif.dbg_pri_chk_flag && jif.dbg_fp) fprintf(jif.dbg_fp, "crc error\n");
				}
			}
		}
	}
	return shared_ptr< JD_FRAME>();
}
static volatile float diff_ctrl_deg1 = 0 , diff_ctrl_deg2 = 0;
static volatile float abs_ctrl_deg_1 = 0, abs_ctrl_deg_2 = 0;
static volatile int abs_ctrl_flag = 0;
static std::condition_variable_any enable_ctrl;
static std::timed_mutex mutex_ctrl;


void set_deg(int flg,float deg1,float deg2)
{
	abs_ctrl_flag = flg;
	if (flg) {
		abs_ctrl_deg_1 = deg1;
		abs_ctrl_deg_2 = deg2;
	} else {
		diff_ctrl_deg1 = deg1;
		diff_ctrl_deg2 = deg2;
	}
	enable_ctrl.notify_all();
}


void ctrl_thread(void)
{
	char namebuff[64];
	unsigned char buff[1024];
	strcpy(namebuff, "/dev/ttyUSB0");

	int fd = UARTX_Init(namebuff, 115200, 0, 8, 1, 0);	

	JD_INFO jif;
	jif.fd = fd;
	//jif.dbg_pri_snd_word = 1;
	JD_FRAME jfr;
	while (true) {
		unique_lock<timed_mutex> lck(mutex_ctrl);
		enable_ctrl.wait(lck);
		float deg1 = 0, deg2 = 0;
		//读取角度
		jfr.jd_aim.byte_value.low_byte = 0xff;
		jfr.jd_aim.byte_value.mlow_byte = 0xff;
		jfr.jd_aim.byte_value.mhigh_byte = 0xff;
		jfr.jd_command = 0x13;
		JD_send(jif, jfr);
		int rdsz = read(fd, buff, 1024);
		if (rdsz) {

			auto dat = JD_pro_bare_buff(buff, rdsz, jif);
			if (dat && dat->jd_data_len >= 6) {
				unsigned char tmpbuff[16];
				memcpy(tmpbuff, dat->jd_data_buff, 6);
				deg1 = Angle_Convert(&tmpbuff[0]);
				deg2 = Angle_Convert(&tmpbuff[3]);
				//printf("x = %f,%f\n", deg1, deg2);
			}
		}
		//设定新角度
		jfr.jd_aim.byte_value.low_byte = 0xff;
		jfr.jd_aim.byte_value.mlow_byte = 0xff;
		jfr.jd_aim.byte_value.mhigh_byte = 0xff;
		jfr.jd_command = 0x0B;
		
		unsigned char sndbuf[20];
		//if ((fabs(ctrl_deg1) < 0.00001f) || (fabs(ctrl_deg2) < 0.00001f)) {
		//	continue;
		//}

		if (abs_ctrl_flag) {
			float aimf1 = abs_ctrl_deg_1;
			float aimf2 = abs_ctrl_deg_2;
			printf("using abs deg ctrl %f,%f,%f,%f\n", deg1, deg2, abs_ctrl_deg_1,
			       abs_ctrl_deg_2);
			Angle_Convert_UShort(aimf1, sndbuf + 0);
			Angle_Convert_UShort(aimf2, sndbuf + 3);
		} else {
			float aimf1 = deg1 + diff_ctrl_deg1;
			float aimf2 = deg2 + diff_ctrl_deg2;
			printf("using diff deg %f,%f,%f,%f\n", deg1, deg2, diff_ctrl_deg1,
			       diff_ctrl_deg2);
			Angle_Convert_UShort(aimf1, sndbuf + 0);
			Angle_Convert_UShort(aimf2, sndbuf + 3);
		}

		jfr.jd_send_buff = sndbuf;
		jfr.jd_data_len = 6;
		JD_send(jif, jfr);

		rdsz = read(fd, buff, 1024);
		diff_ctrl_deg1 = 0;
		diff_ctrl_deg2 = 0;
		if (rdsz) {

			auto dat = JD_pro_bare_buff(buff, rdsz, jif);
			if (dat) {
				printf("sendok\n");
			}
		}
	}
}

void init_ctrl_thread(void)
{
	thread t_ctrl(ctrl_thread);
    t_ctrl.detach();
}
