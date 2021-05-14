
#include "SN1V2_rtConfig.h"
#include <clockd_def.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/prctl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <mdc_ctrl.h>
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
using namespace std;

JDAUTOSEND * jdsvc_table();
JDAUTOSEND * jdsvc_time();
JDAUTOSEND * jdsvc_manuals();
JDAUTOSEND * jdsvc_corrects();
JDAUTOSEND * jdsvc_par_sets();
JDAUTOSEND * jdsvc_par_gets();
JDAUTOSEND * jdsvc_stops();
JDAUTOSEND * jdsvc_alarm_clean();

JDAUTOSEND *grp[] = {
	jdsvc_table(),	
	jdsvc_stops(),
	jdsvc_manuals(),
	jdsvc_corrects(),
	jdsvc_par_sets(),
	jdsvc_par_gets(),
	jdsvc_time(),
	jdsvc_alarm_clean(),
};


int master_svc_thread(JD_INFO * pjif)
{
	while (true) {
		unique_lock<timed_mutex> lck(pjif->enable_mtx);
		pjif->enable_cv.wait_for(lck, chrono::milliseconds(50));

		for (auto & t : grp) {
			if (t->need_service(*pjif)) {
				t->service_pro(*pjif);
				break;
			}		
		}
	}
}

//int JD_time_rec(JD_INFO & jif, JD_FRAME & jfr);
//int JD_manual_rec(JD_INFO & jif, JD_FRAME & jfr);
//int JD_correct_rec(JD_INFO & jif, JD_FRAME & jfr);
//int JD_parset_rec(JD_INFO & jif, JD_FRAME & jfr);
//int JD_parget_rec(JD_INFO & jif, JD_FRAME & jfr);
//int JD_stop_rec(JD_INFO & jif, JD_FRAME & jfr);
//int JD_clean_alarm_rec(JD_INFO & jif, JD_FRAME & jfr);
static int JD_time_rec(JD_INFO & jif, JD_FRAME & jfr);
int JD_cre_response(JD_INFO & jif, JD_FRAME & jfr);
int JD_file_service(JD_INFO & jif, JD_FRAME & jfr);
int JD_file_version(JD_INFO & jif, JD_FRAME & jfr);
int JD_TransMit_img(JD_INFO & jif, JD_FRAME & jfr);

int JD_TransMit_test(JD_INFO & jif);

JDPROSTRUCT JD_init_rec_group[] =
{
#if 0
	{ 0x0a | 0x80 ,JD_stop_rec},
	{ 0x0b | 0x80 ,JD_manual_rec},
	{ 0x0C | 0x80 ,JD_correct_rec},
	{ 0x11 | 0x80 ,JD_parset_rec},
	{ 0x12 | 0x80 ,JD_parget_rec},
	{ 0x13 | 0x80, JD_time_rec },
	//{ 0x34 | 0x80, JD_time_rec },
	{ 0x18 | 0x80,JD_clean_alarm_rec},
#else
	{ 0x34 , JD_time_rec }
	,{ 0x33, JD_cre_response }
	,{0x35 , JD_file_service }
	,{0x37 , JD_file_version}
	,{0x38 , JD_TransMit_img}
#endif

};

void merge_data(MDC_INFO * pjif, SCANF_DATA & dat);

static void scan_file(void * p, const char * fil)
{
	if (!p || !fil) {
		return ;
	}
	TimeInterval ttt("scanf:");
	JD_INFO * pjif = (JD_INFO *)p;
	printf("1111\n");
	auto dat = real_scan_file(fil);
	printf("2222\n");
	merge_data((MDC_INFO *)pjif, dat);
}



int register_master_svc(JD_INFO& jif)
{
	for (auto p : JD_init_rec_group) {
		JD_pro_ctl(jif, p.command, p.pro, 1);
	}

	#if 0
	thread p(master_svc_thread, &jif);
	p.detach();
	#endif

	scan_file(&jif, MDC_MODE_FILE);
	//printf("register_master_svc-------------scan_file-------------------->end \n");

	thread watchmod(SetWatchFile, MDC_MODE_FILE, scan_file, &jif);
	watchmod.detach();

	return 0;
}


int JDAUTOSEND::findMdc_addr(JD_INFO & injif, int addr)
{
	MDC_INFO & jif = (MDC_INFO &)injif;
	int using_index = -1;
	for (auto & p : jif.mdcCtrl) {
		if (p.addr == addr) {
			using_index = std::distance(&jif.mdcCtrl[0], &p);
			return using_index;
		}
	}
	return using_index;
}

/****************************原SN1V2 mdc业务函数**********************************/



static void tmktime(tm & NowTm, unsigned char *jd_data_buff)
{
	NowTm.tm_year = (int)jd_data_buff[0] + 100;
	NowTm.tm_mon = (int)jd_data_buff[1] - 1;
	NowTm.tm_mday = (int)jd_data_buff[2];

	NowTm.tm_hour = (int)jd_data_buff[3];
	NowTm.tm_min = (int)jd_data_buff[4];
	NowTm.tm_sec = (int)jd_data_buff[5];
}

static int diff_timeval_ms(timeval & a, timeval & b)
{
	timeval dif;
	dif.tv_sec = a.tv_sec - b.tv_sec;
	dif.tv_usec = a.tv_usec - b.tv_usec;

	return dif.tv_sec * 1000 + dif.tv_usec / 1000;
}


/*
static void mdc_alive_reflash(SN1_SHM * psn1)
{
	while (true) {
		sleep(1);

		timeval nowtv, rectv;
		rectv.tv_sec = psn1->last_tv_sec;
		rectv.tv_usec = psn1->last_tv_usec;

		gettimeofday(&nowtv, nullptr);

		int abs_ms = abs(diff_timeval_ms(nowtv, rectv));

		if (abs_ms / 1000 > psn1->max_time_out_second) {
			psn1->mdc_flag = SN1_SHM::MDC_TIME_FALSE;
			psn1->helo_status = SN1_SHM::Helo_not_ready;
		}
	}
}
*/

static int JD_ret_cap_status(JD_INFO & jif, JD_FRAME & jfr)
{
	MDC_INFO & jit = (MDC_INFO &)jif;

	SN1_SHM *psn1 = jit.psn1;

	struct 
	{
		uint16_t error_cnt;
		int16_t error_code;
	}sendp;

	//仅mdc.exe重启的时候使用，尚未进行拍摄
	static int initFlg = 0;
	if (initFlg == 0) {
		initFlg = 1;
		psn1->last_error_code = err_No_err_before_cap;
	}

	if (!jit.error_debug_flag)
	{
		sendp.error_code = psn1->last_error_code;
		sendp.error_cnt = psn1->error_count;
	}
	else
	{
		psn1->error_count++;
		sendp.error_code = -(psn1->error_count  % 1000);
		sendp.error_cnt = psn1->error_count;
	}

	jfr.jd_send_buff = &sendp; 
	jfr.jd_data_len = sizeof(sendp);
	jfr.jd_command |= 0x80;

	JD_send(jif, jfr);
	return JD_OK;
}

static int modify_raw_time(char * buffer, unsigned char * jd_data_buff)
{
	return sprintf(buffer, "%02d-%02d-%02d %02d:%02d:%02d.%03d status = %d"
		, (int)jd_data_buff[0]
		, (int)jd_data_buff[1]
		, (int)jd_data_buff[2]

		, (int)jd_data_buff[3]
		, (int)jd_data_buff[4]
		, (int)jd_data_buff[5]

		, (int)(jd_data_buff[6] | jd_data_buff[7] << 8)
		, (int)jd_data_buff[8]
	);
}


static void disp_raw_time(unsigned char * jd_data_buff)
{
	char buff[256];
	modify_raw_time(buff, jd_data_buff);
	printf("now time is %s\n", buff);
}

static void shm_set_time(JD_INFO & jif, timeval & tv, int status, unsigned char testbuff[])
{
	MDC_INFO & info_tim = (MDC_INFO &)jif;
	info_tim.psn1->mdc_flag = SN1_SHM::MDC_TIME_OK;
//	printf("status is ok!!!!!!!!!!!!\n");
	//status is ok
	
	if (status == 0) {
		//printf("status is 0 --- Helo_ok\n");
		if (info_tim.dbg_shm_ret_printf) printf("rec:mdc work flag ok!!!!!!!!!!!!\n");
		info_tim.psn1->helo_status = SN1_SHM::Helo_ok;
		info_tim.psn1->last_tv_sec = tv.tv_sec;
		info_tim.psn1->last_tv_usec = tv.tv_usec;
	}//other is error
	else {
		//printf("status is 1 --- Helo_not_ready\n");
		if (info_tim.dbg_shm_ret_printf)printf("rec:mdc work flag failed!!!!!!!!!!!!\n");
		info_tim.psn1->helo_status = SN1_SHM::Helo_not_ready;
	}
	
//	printf("status is test !!!!!!!!!!!!\n");
	if (info_tim.dbg_shm_ret_printf && testbuff) {
		printf("19 = %d\n", testbuff[0]);
		printf("20 = %d\n", testbuff[1]);
		printf("21 = %d\n", testbuff[2]);
		printf("22 = %d\n", testbuff[3]);
		printf("23 = %d\n", testbuff[4]);
	}
}

static int JD_time_rec(JD_INFO & jif, JD_FRAME & jfr)
{
	MDC_INFO & jit = (MDC_INFO &)jif;
	if (jfr.jd_data_len >= 9) {
		//printf("JD_time_rec **********mdc date true********\n");
		tm NowTm;
		tmktime(NowTm, jfr.jd_data_buff);

		time_t tt = mktime(&NowTm);

		timeval rectv;
		rectv.tv_sec = tt;
		rectv.tv_usec = (int)(jfr.jd_data_buff[6] | jfr.jd_data_buff[7] << 8) * 1000;

		timeval nowtv;
		gettimeofday(&nowtv, nullptr);

		//enable time diff && time set
		if (jit.time_diff_max) {
			int diff_ms = diff_timeval_ms(nowtv, rectv);
			int abs_ms = abs(diff_ms);
			//disp diff time
			//if (jit.dbg_tim_rec_printf) {
			if (1) {
				//printf("JD_time_rec **********dbg_tim_rec_printf********\n");
				char  buffer[256];
				int sz = 0;
				sz += sprintf(buffer + sz, "mdc time = ");
				sz += modify_raw_time(buffer + sz, jfr.jd_data_buff);
				sz += sprintf(buffer + sz, ",");
				tm & now_time = NowTm;
				sz += sprintf(buffer + sz, "sn1 time = %4d-%02d-%02d %02d:%02d:%02d--%06ld"
					, now_time.tm_year + 1900, now_time.tm_mon + 1, now_time.tm_mday
					, now_time.tm_hour, now_time.tm_min, now_time.tm_sec
					, nowtv.tv_usec
				);

				fprintf(jit.dbg_fp, "%s , diff tick = %d ,status = %d \n", buffer, diff_ms, (int)jfr.jd_data_buff[8]);
			}

			if (abs_ms > jit.time_diff_max) {
				settimeofday(&rectv, nullptr);
				gettimeofday(&nowtv, nullptr);
				disp_raw_time(jfr.jd_data_buff);

				if (jit.dbg_tim_rec_printf)fprintf(jit.dbg_fp, "time set ok \n");
				//printf("JD_time_rec **********set time ********\n");
			}
		}
		jit.psn1->mdc_id_num = jfr.jd_aim.value;
#if 1
		unsigned char buff[3];
		memcpy(buff, &jfr.jd_aim, 3);

		char outbuff[20];

		JD_Name_transfer(buff, outbuff, sizeof(outbuff));
		//printf("mdc id = %s\n", outbuff);
#endif
		//printf("JD_time_rec **********now time begin********\n");
		shm_set_time(jif, nowtv, (int)jfr.jd_data_buff[8], jfr.jd_data_len > 9 ? jfr.jd_data_buff + 9 : nullptr);
		//printf("JD_time_rec **********now time end********\n");
		//发送给马达控制器相应的数据
		JD_ret_cap_status(jif, jfr);
	}
	return JD_OK;
	//printf("JD_time_rec **********mdc date true JD_OK********\n");
}


int JD_TransMit_test(JD_INFO & jif)
{
	unsigned char AnsBuffer[20] = { 0 };
	int wrlen;
	AnsBuffer[0] = 0xAA;
	AnsBuffer[1] = 0xAA;
	AnsBuffer[2] = 0x11;
	AnsBuffer[3] = 0x22;
	AnsBuffer[4] = 0x33;
	AnsBuffer[5] = 0x44;
	AnsBuffer[6] = 0x55;
	AnsBuffer[7] = 0x66;
	unsigned int mkcrc = crc_make((unsigned char*)AnsBuffer, 8, 0xFFFF);
	AnsBuffer[8] = mkcrc & 0xff;
	AnsBuffer[9] = mkcrc >> 8 & 0xff;

	wrlen = write(jif.fd, AnsBuffer, 10);

	timeval tv;
	gettimeofday(&tv, nullptr);

	if (jif.dbg_pri_snd_len && jif.dbg_fp) fprintf(jif.dbg_fp, "sd len = %d time=%ld.%06ld\n", wrlen, tv.tv_sec, tv.tv_usec);
	if (jif.dbg_pri_snd_word && jif.dbg_fp) disp_x_buff(jif.dbg_fp, AnsBuffer, wrlen);

	return wrlen;

}

