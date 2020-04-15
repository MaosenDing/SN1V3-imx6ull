
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

int JD_time_rec(JD_INFO & jif, JD_FRAME & jfr);
int JD_manual_rec(JD_INFO & jif, JD_FRAME & jfr);
int JD_correct_rec(JD_INFO & jif, JD_FRAME & jfr);
int JD_parset_rec(JD_INFO & jif, JD_FRAME & jfr);
int JD_parget_rec(JD_INFO & jif, JD_FRAME & jfr);
int JD_stop_rec(JD_INFO & jif, JD_FRAME & jfr);
int JD_clean_alarm_rec(JD_INFO & jif, JD_FRAME & jfr);

JDPROSTRUCT JD_init_rec_group[] =
{
	{ 0x0a | 0x80 ,JD_stop_rec},
	{ 0x0b | 0x80 ,JD_manual_rec},
	{ 0x0C | 0x80 ,JD_correct_rec},
	{ 0x11 | 0x80 ,JD_parset_rec},
	{ 0x12 | 0x80 ,JD_parget_rec},
	{ 0x13 | 0x80, JD_time_rec },
	{ 0x18 | 0x80,JD_clean_alarm_rec},
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
	
	thread p(master_svc_thread, &jif);
	p.detach();

	scan_file(&jif, MDC_MODE_FILE);

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


