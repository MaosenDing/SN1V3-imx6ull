
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

#include <thread>
using namespace std;


static int diff_timeval_ms(timeval & a, timeval & b)
{
	timeval dif;
	dif.tv_sec = a.tv_sec - b.tv_sec;
	dif.tv_usec = a.tv_usec - b.tv_usec;

	return dif.tv_sec * 1000 + dif.tv_usec / 1000;
}


void mdc_alive_reflash(SN1_SHM * psn1)
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


int regist_timer_auto_flush(SN1_SHM * psn1)
{
	thread p(mdc_alive_reflash, psn1);
	p.detach();
	return 0;
}


