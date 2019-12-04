#include "JDcomhead.h"
#include "jd_share.h"
#include "svc.h"
#include <thread>
#include "SN1V2_com.h"
#include <mutex>
#include <iostream>
#include <algorithm>
using namespace std;



struct jdsvc_manual :public JDAUTOSEND {
	jdsvc_manual()
	{
		scan_file(this, MDC_MODE_FILE);
		thread watch(SetWatchFile, MDC_MODE_FILE, scan_file, this);
		watch.detach();
	}
	std::mutex tableLock;

	void trig_cpl(JD_FRAME & jfr)
	{

	}


	static void scan_file(void * p, const char * fil)
	{
		if (!p || !fil) {
			return;
		}

		jdsvc_manual *thissvc = (jdsvc_manual*)p;

		//thissvc->_scan_file(fil);
	}

	int using_index;

	struct AIM{
		int succ_flag;
		int retry_cnt;
	}aim[2];

	uint64_t last_send_tim;
	int send_period_s = 1;

	virtual int need_service(JD_INFO & jif) final
	{
		if (jif.JD_MOD != jif.mdc_mode_manual) {
			return 0;
		}
		timeval tv;
		gettimeofday(&tv, nullptr);

		if (tv.tv_sec == last_tv.tv_sec) {
			return 0;
		}
		return 1;
	}

	virtual void service_pro(JD_INFO & jif)final
	{


	}
};

static jdsvc_manual jsvc;

JDAUTOSEND * jdsvc_manuals()
{
	return &jsvc;
}


int JD_manual_rec(JD_INFO & jif, JD_FRAME & jfr)
{
	JD_INFO_TIM & jit = (JD_INFO_TIM &)jif;

	jsvc.trig_cpl(jfr);

	return JD_OK;
}


