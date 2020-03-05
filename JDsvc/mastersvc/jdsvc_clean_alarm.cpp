#include "mdc_ctrl.h"
#include "jd_share.h"
#include "svc.h"
#include <sys/time.h>
#include <string.h>
#include "SN1V2_com.h"
#include <iostream>
#include <math.h>
#include "SN1V2_com.h"
#include "led_ctrl.h"
using namespace  std;

struct jdcleanAlarm :public JDAUTOSEND {


	inline int searchUncoplete(MDC_INFO & jif)
	{
		timeval tv;
		gettimeofday(&tv, nullptr);

		for (auto &p : jif.mdcCtrl) {
			auto &alm = p.alarm;

			if (alm.cpl_flag == 0 && alm.retry_num < alm.Max_retry) {
				return  std::distance(&jif.mdcCtrl[0], &p);
			}
		}
		return -1;
	}


	virtual int need_service(JD_INFO & injif) final
	{
		MDC_INFO& jif = (MDC_INFO &)injif;
		if (searchUncoplete(jif) >= 0) {
			return 1;
		}
		return 0;
	}


	virtual void service_pro(JD_INFO & injif)final
	{
		MDC_INFO& jif = (MDC_INFO &)injif;
		int using_index = searchUncoplete(jif);
		if (using_index < 0) {
			return;
		}
		
		auto &aim = jif.mdcCtrl[using_index].alarm;

		JD_FRAME jfr;

		jfr.jd_aim.value = jif.mdcCtrl[using_index].addr;
		jfr.jd_send_buff = nullptr;
		jfr.jd_data_len = 0;
		jfr.jd_command = 0x18;
		aim.retry_num++;
		printf("try alarm clean %x using %d\n", jif.mdcCtrl[using_index].addr, aim.retry_num);
		JD_send(jif, jfr);
	}

	void trig_cpl(JD_INFO & injif, JD_FRAME & jfr)
	{
		MDC_INFO& jif = (MDC_INFO &)injif;
		int getIndex = findMdc_addr(jif, jfr.jd_aim.value);

		if (getIndex < 0) {
			printf("bad addr = 0x%x\n", jfr.jd_aim.value);
			return;
		}
		printf("alarm clean complete %x\n", jfr.jd_aim.value);
		auto & alarm = jif.mdcCtrl[getIndex].alarm;
		alarm.cpl_flag = 1;
	}
};

static jdcleanAlarm jsvc;

JDAUTOSEND * jdsvc_alarm_clean()
{
	return &jsvc;
}


int JD_clean_alarm_rec(JD_INFO & jif, JD_FRAME & jfr)
{
	JD_INFO_TIM & jit = (JD_INFO_TIM &)jif;
	jsvc.trig_cpl(jif, jfr);
	return JD_OK;
}


