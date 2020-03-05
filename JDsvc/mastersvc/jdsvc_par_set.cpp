#include "mdc_ctrl.h"
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

	void trig_cpl(JD_INFO & injif, JD_FRAME & jfr)
	{
		MDC_INFO& jif = (MDC_INFO &)injif;
		int getIndex = findMdc_addr(jif, jfr.jd_aim.value);

		if (getIndex < 0) {
			printf("bad addr = 0x%x\n", jfr.jd_aim.value);
			return;
		}
		Par_CTRL & par = jif.mdcCtrl[getIndex].par;

		par.cpl_flag = 1;


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
			auto & p = par.par;
			if (p.cpl_flag == 0 && p.retry_num < p.Max_retry && par.parget.succ_flag ) {
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

	inline void setPart2buff(char * databuff, Par_CTRL & aim)
	{
		char * point = databuff;
		*point++ = aim.initSpeed & 0xff;
		*point++ = aim.initSpeed >> 8 & 0xff;

		*point++ = aim.MaxSpeed & 0xff;
		*point++ = aim.MaxSpeed >> 8 & 0xff;

		*point++ = aim.period;

		*point++ = aim.Phase;

		*point++ = aim.current;

		*point++ = aim.Ratio & 0xff;
		*point++ = aim.Ratio >> 8 & 0xff;
	}

	inline void copydata(Par_GET & src, Par_CTRL &dst, int flg)
	{
		if (!(flg & diff_init0)) {
			dst.initSpeed = src.initSpeed;
		}

		if (!(flg & diff_max0)) {
			dst.MaxSpeed = src.MaxSpeed;
		}

		if (!(flg & diff_period0)) {
			dst.period = src.period;
		}

		if (!(flg & diff_phase0)) {
			dst.Phase = src.Phase;
		}

		if (!(flg & diff_currect0)) {
			dst.current = src.current;
		}

		if (!(flg & diff_ratio0)) {
			dst.Ratio = src.Ratio;
		}
	}

	virtual void service_pro(JD_INFO & injif)final
	{
		MDC_INFO& jif = (MDC_INFO &)injif;
		int using_index = getUncpl(jif);

		if (using_index < 0) {
			return;
		}
		Par_GET &org = jif.mdcCtrl[using_index].parget;
		Par_CTRL &aim = jif.mdcCtrl[using_index].par;

		copydata(org, aim, aim.setflg);

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
		jfr.jd_command = 0x11;

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


