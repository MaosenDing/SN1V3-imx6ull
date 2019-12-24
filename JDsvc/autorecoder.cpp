#include <thread>
#include "SN1V2_com.h"
#include "JDcomhead.h"
#include "errHandle/errHandle.h"
#include <fstream>
using namespace  std;



void auto_recoder_main(JD_INFO_TIM * pjif)
{
	while (true) {

		timeval tvnow;
		gettimeofday(&tvnow, NULL);

		tm now_time;
		localtime_r(&tvnow.tv_sec, &now_time);

		char recoderbuff[128];

		int len = snprintf(recoderbuff, 128, "time test %4d-%02d-%02d %02d:%02d:%02d status %d time %d\n"
			, now_time.tm_year + 1900, now_time.tm_mon + 1, now_time.tm_mday
			, now_time.tm_hour, now_time.tm_min, now_time.tm_sec
			, pjif->psn1->last_error_code, pjif->psn1->error_count
		);

		ofstream f("/home/sn1ppp", ios::out | ios::app);

		if (f)
			f << recoderbuff;
	}
}


void init_auto_recoder(JD_INFO_TIM & jif)
{
	thread p(auto_recoder_main, &jif);
	p.detach();
}
