#include "mdc_ctrl.h"
#include "jd_share.h"
#include "svc.h"
#include <thread>
#include "SN1V2_com.h"
#include <mutex>
#include <iostream>
#include <algorithm>
using namespace std;



struct jdsvc_stop :public JDAUTOSEND {
	jdsvc_stop()
	{}

	void trig_cpl(JD_INFO & injif, JD_FRAME & jfr)
	{
		MDC_INFO& jif = (MDC_INFO &)injif;
		int getIndex = findMdc_addr(jif, jfr.jd_aim.value);
		
		printf("findMdc_addr = 0x%x\n", jfr.jd_aim.value);

		if (getIndex < 0) {
			printf("bad addr = 0x%x\n", jfr.jd_aim.value);
			return;
		}
		CTRL_BASE & ctl = jif.mdcCtrl[getIndex].stop;

		ctl.cpl_flag = 1;
	}
	
	inline int getUncpl(MDC_INFO & jif)
	{
		int using_index = -1;
		for (auto &par : jif.mdcCtrl) {
			auto & p = par.stop;
			if (p.cpl_flag == 0 && p.retry_num < p.Max_retry) {
				using_index = std::distance(&jif.mdcCtrl[0], &par);
				return using_index;
			}
		}
		return using_index;
	}


	virtual int need_service(JD_INFO & injif) final
	{
		MDC_INFO& jif = (MDC_INFO &)injif;
		if (jif.work_mod != mdc_mode_off) {
			return 0;
		}
		
		if (getUncpl(jif) >= 0) {
			return 1;
		}
		return 0;
	}


	virtual void service_pro(JD_INFO & injif)final
	{
		printf("jdsvc_stop -----------service_pro\n");
		MDC_INFO& jif = (MDC_INFO &)injif;
		int using_index = getUncpl(jif);

		if (using_index < 0) {
			return;
		}
		CTRL_BASE &aim = jif.mdcCtrl[using_index].stop;

		JD_FRAME jfr;

		jfr.jd_aim.value = jif.mdcCtrl[using_index].addr;

		jfr.jd_send_buff = nullptr;
		jfr.jd_data_len = 0;
		jfr.jd_command = 0xa;

		aim.retry_num++;
		JD_send(jif, jfr);
		printf("JD_send = jdsvc_stops 0000000\n");
		return;
	}
};

static jdsvc_stop jsvc;

JDAUTOSEND * jdsvc_stops()
{
	return &jsvc;
}
int JD_stop_rec(JD_INFO & jif, JD_FRAME & jfr)
{
	__attribute__((unused)) JD_INFO_TIM & jit = (JD_INFO_TIM &)jif;

	jsvc.trig_cpl(jif, jfr);

	return JD_OK;
}


