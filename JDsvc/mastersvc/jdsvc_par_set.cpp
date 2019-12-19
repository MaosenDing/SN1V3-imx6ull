#include "JDcomhead.h"
#include "jd_share.h"
#include "svc.h"
#include <thread>
#include "SN1V2_com.h"
#include <mutex>
#include <iostream>
#include <algorithm>
using namespace std;



struct jdsvc_par_set :public JDAUTOSEND {
	jdsvc_par_set()
	{}

	void trig_cpl(JD_INFO & jif, JD_FRAME & jfr)
	{
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

	inline int getUncpl(JD_INFO & jif)
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


	virtual int need_service(JD_INFO & jif) final
	{
		if (getUncpl(jif) >= 0) {
			return 1;
		}
		return 0;
	}

	inline void setPart2buff(char * databuff, Par_CTRL & aim)
	{
		char * point = databuff;
		*point++ = aim.initSpeed >> 8 & 0xff;
		*point++ = aim.initSpeed & 0xff;

		*point++ = aim.MaxSpeed >> 8 & 0xff;
		*point++ = aim.MaxSpeed & 0xff;

		*point++ = aim.period;

		*point++ = aim.Phase;

		*point++ = aim.current;

		*point++ = aim.Ratio >> 8 & 0xff;
		*point++ = aim.Ratio & 0xff;
	}


	virtual void service_pro(JD_INFO & jif)final
	{
		int using_index = getUncpl(jif);

		if (using_index < 0) {
			return;
		}
		Par_CTRL &aim = jif.mdcCtrl[using_index].par;

		JD_FRAME jfr;


		printf("parset using %d ,cnt = %d, init %d,max %d,period %d,phase %d, current %d,ratio %d\n"
			, using_index, aim.retry_num
			,aim.initSpeed,aim.MaxSpeed,aim.period,aim.Phase,aim.current,aim.Ratio		
		);

		char databuff[9];
		setPart2buff(databuff, aim);

		jfr.jd_aim.value = jif.mdcCtrl[using_index].addr;

		jfr.jd_send_buff = &databuff;
		jfr.jd_data_len = 9;
		jfr.jd_command = 0xc;

		aim.retry_num++;
		JD_send(jif, jfr);
		return;
	}
};

static jdsvc_par_set jsvc;

JDAUTOSEND * jdsvc_par_sets()
{
	return &jsvc;
}
int JD_parset_rec(JD_INFO & jif, JD_FRAME & jfr)
{
	JD_INFO_TIM & jit = (JD_INFO_TIM &)jif;

	jsvc.trig_cpl(jif, jfr);

	return JD_OK;
}


