
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
using namespace std;


JDAUTOSEND * jdsvc_time();

int master_svc_thread(JD_INFO * pjif)
{
	while (true) {

		struct timespec ts;
		ts.tv_nsec += 20 * 1000;
		clock_gettime(CLOCK_REALTIME, &ts);

		sem_timedwait(&pjif->sem_enable,&ts);

		JDAUTOSEND *t = jdsvc_time();

		if (t->need_service()) {
			t->service_pro(*pjif);
		}
	}
}

int JD_time_rec(JD_INFO & jif, JD_FRAME & jfr);
JDPROSTRUCT JD_init_rec_group[] =
{
	{ 0x34 | 0x80, JD_time_rec }

};






int register_master_svc(JD_INFO& jif)
{
	for (auto p : JD_init_rec_group) {
		JD_pro_ctl(jif, p.command, p.pro, 1);
	}

	thread p(master_svc_thread, &jif);
	p.detach();
	return 0;
}












