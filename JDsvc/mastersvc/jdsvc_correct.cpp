#include "mdc_ctrl.h"
#include "jd_share.h"
#include "svc.h"
#include <thread>
#include "SN1V2_com.h"
#include <mutex>
#include <iostream>
#include <algorithm>
using namespace std;



struct jdsvc_correct :public JDAUTOSEND {
	jdsvc_correct()
	{}

	void trig_cpl(JD_INFO & injif, JD_FRAME & jfr)
	{
		MDC_INFO& jif = (MDC_INFO &)injif;
		int getIndex = findMdc_addr(jif, jfr.jd_aim.value);

		if (getIndex < 0) {
			printf("bad addr = 0x%x\n", jfr.jd_aim.value);
			return;
		}
		CTRL_BASE & ctl = jif.mdcCtrl[getIndex].correct;

		if (jfr.jd_data_len != 3) {
			printf("bad rec len = %d\n", jfr.jd_data_len);
			return;
		}
		ctl.cpl_flag = 1;
	}

	inline int getUncpl(MDC_INFO & jif)
	{
		int using_index = -1;
		for (auto &par : jif.mdcCtrl) {
			auto & p = par.correct;
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
		if (getUncpl(jif) >= 0) {
			return 1;
		}
		return 0;
	}


	virtual void service_pro(JD_INFO & injif)final
	{
		MDC_INFO& jif = (MDC_INFO &)injif;
		int using_index = getUncpl(jif);

		if (using_index < 0) {
			return;
		}
		Man_CTRL &aim = jif.mdcCtrl[using_index].correct;

		JD_FRAME jfr;

		float aimdeg = aim.manual_deg;
		unsigned int tmpdeg = Angle_Convert_UShort(aimdeg);
		printf("correct using %d ,cnt = %d, deg = %f\n", using_index, aim.retry_num, aimdeg);

		char databuff[3];
		databuff[0] = (tmpdeg >> (0 * 8)) & 0xff;
		databuff[1] = (tmpdeg >> (1 * 8)) & 0xff;
		databuff[2] = (tmpdeg >> (2 * 8)) & 0xff;

		jfr.jd_aim.value = jif.mdcCtrl[using_index].addr;

		jfr.jd_send_buff = &databuff;
		jfr.jd_data_len = 3;
		jfr.jd_command = 0xc;

		aim.retry_num++;
		JD_send(jif, jfr);
		return;
	}
};

static jdsvc_correct jsvc;

JDAUTOSEND * jdsvc_corrects()
{
	return &jsvc;
}
int JD_correct_rec(JD_INFO & jif, JD_FRAME & jfr)
{
	__attribute__((unused)) JD_INFO_TIM & jit = (JD_INFO_TIM &)jif;

	jsvc.trig_cpl(jif, jfr);

	return JD_OK;
}


