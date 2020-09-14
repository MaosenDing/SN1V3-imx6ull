#include "mdc_ctrl.h"
#include "jd_share.h"
#include "svc.h"
#include <thread>
#include "SN1V2_com.h"
#include <mutex>
#include <iostream>
#include <algorithm>
using namespace std;



struct jdsvc_par_get :public JDAUTOSEND {
	jdsvc_par_get()
	{}

	void trig_cpl(JD_INFO & injif, JD_FRAME & jfr)
	{
		MDC_INFO& jif = (MDC_INFO &)injif;
		int getIndex = findMdc_addr(jif, jfr.jd_aim.value);

		if (getIndex < 0) {
			printf("bad addr = 0x%x\n", jfr.jd_aim.value);
			return;
		}
		Par_GET & get = jif.mdcCtrl[getIndex].parget;

		if (jfr.jd_data_len != 9) {
			printf("bad rec len = %d\n", jfr.jd_data_len);
			return;
		}

		get.initSpeed = jfr.jd_data_buff[0] | jfr.jd_data_buff[1] << 8;
		get.MaxSpeed = jfr.jd_data_buff[2] | jfr.jd_data_buff[3] << 8;
		get.period = jfr.jd_data_buff[4];
		get.Phase = jfr.jd_data_buff[5];
		get.current = jfr.jd_data_buff[6];
		get.Ratio = jfr.jd_data_buff[7] | jfr.jd_data_buff[8] << 8;

		get.cpl_flag = 1;
		get.succ_flag = 1;

		printf("get par %x, init %d,max %d,period %d,phase %d, current %d,ratio %d\n"
			, jif.mdcCtrl[getIndex].addr
			, get.initSpeed, get.MaxSpeed, get.period, get.Phase, get.current, get.Ratio
		);

	}

	inline int getUncpl(MDC_INFO & jif)
	{
		int using_index = -1;
		for (auto &par : jif.mdcCtrl) {
			auto & p = par.parget;
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
		Par_GET &aim = jif.mdcCtrl[using_index].parget;

		JD_FRAME jfr;


		printf("parget using %d ,cnt = %d\n", using_index, aim.retry_num);

		jfr.jd_aim.value = jif.mdcCtrl[using_index].addr;

		jfr.jd_send_buff = nullptr;
		jfr.jd_data_len = 0;
		jfr.jd_command = 0x12;

		aim.retry_num++;
		JD_send(jif, jfr);
		return;
	}
};

static jdsvc_par_get jsvc;

JDAUTOSEND * jdsvc_par_gets()
{
	return &jsvc;
}
int JD_parget_rec(JD_INFO & jif, JD_FRAME & jfr)
{
	__attribute__((unused)) JD_INFO_TIM & jit = (JD_INFO_TIM &)jif;

	jsvc.trig_cpl(jif, jfr);

	return JD_OK;
}


