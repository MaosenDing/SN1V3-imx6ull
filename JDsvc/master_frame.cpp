
#include "SN1V2_rtConfig.h"
#include "JDcomhead.h"
#include <clockd_def.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/prctl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <JDcomhead.h>
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
using namespace std;

JDAUTOSEND * jdsvc_table();
JDAUTOSEND * jdsvc_time();
JDAUTOSEND * jdsvc_manuals();
JDAUTOSEND *grp[] = {
	jdsvc_table(),
	jdsvc_time(),
	jdsvc_manuals(),
};


int master_svc_thread(JD_INFO * pjif)
{
	while (true) {
		unique_lock<timed_mutex> lck(pjif->enable_mtx);
		pjif->enable_cv.wait_for(lck, chrono::milliseconds(20));

		for (auto & t : grp) {
			if (t->need_service(*pjif)) {
				t->service_pro(*pjif);
				break;
			}		
		}
	}
}

int JD_time_rec(JD_INFO & jif, JD_FRAME & jfr);
int JD_table_rec(JD_INFO & jif, JD_FRAME & jfr);
int JD_manual_rec(JD_INFO & jif, JD_FRAME & jfr);
JDPROSTRUCT JD_init_rec_group[] =
{
	{ 0x34 | 0x80, JD_time_rec },
	{ 0x35 | 0x80 ,JD_table_rec},
	{ 0x36 | 0x80 ,JD_manual_rec},
};
//ERR_STA loadFile(char *fname, vector<uint8_t> & refVect)


void save_default(JD_INFO& jif, const char * fil)
{
	jif.JD_MOD = jif.mdc_mode_table;

	char defchar[] = "auto\n";

	saveBin((char *)fil, defchar, strlen(defchar));
}

static void scan_file(void * p, const char * fil)
{
	if (!p || !fil) {
		return ;
	}
	JD_INFO * pjif = (JD_INFO *)p;

	vector<uint8_t> stText;
	loadFile((char *)fil, stText);
	do{
		if (stText.empty()) {
			break;
		}

		if (strstr((char *)&stText.at(0), "auto")) {
			pjif->JD_MOD = pjif->mdc_mode_table;
			printf("scanf auto\n");
			return;
		}

		if (strstr((char *)&stText.at(0), "off")) {
			pjif->JD_MOD = pjif->mdc_mode_off;
			printf("scanf auto\n");
			return;
		}

		float f1, f2;
		int ret2 = sscanf((char *)&stText.at(0), "manual %f %f", &f1, &f2);

		if (ret2 == 2) {
			pjif->JD_MOD = pjif->mdc_mode_manual;
			printf("scanf manual = %f %f\n", f1, f2);
			return;
		}
	} while (0);
	printf("scanf null\n");
	save_default(*pjif, fil);
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












