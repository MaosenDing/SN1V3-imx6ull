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
	{}
	std::mutex tableLock;

	inline int findMdc_addr(JD_INFO & jif, int addr)
	{
		int using_index = -1;
		for (auto & p : jif.mdcCtrl) {
			if (p.addr == addr) {
				using_index = std::distance(&jif.mdcCtrl[0], &p);
				return using_index;
			}
		}
		return using_index;
	}


	void trig_cpl(JD_INFO & jif, JD_FRAME & jfr)
	{
		int getIndex = findMdc_addr(jif, jfr.jd_aim.value);

		if (getIndex < 0) {
			printf("bad addr = 0x%x\n", jfr.jd_aim.value);
			return;
		}
		MDC_CTRL & ctl = jif.mdcCtrl[getIndex];

		if (jfr.jd_data_len != 3) {
			printf("bad rec len = %d\n", jfr.jd_data_len);
			return;
		}
		ctl.cpl_flag = 1;
	}
	
	inline int getUncpl(JD_INFO & jif)
	{
		int using_index = -1;
		for (auto &p : jif.mdcCtrl) {
			if (p.cpl_flag == 0 && p.retry_num < p.Max_retry) {
				using_index = std::distance(&jif.mdcCtrl[0], &p);
				printf("index = %d\n", using_index);
				return using_index;
			}
		}
		return using_index;
	}


	virtual int need_service(JD_INFO & jif) final
	{
		if (jif.JD_MOD != jif.mdc_mode_manual) {
			return 0;
		}
		
		if (getUncpl(jif) >= 0) {
			printf("555\n");
			return 1;
		}
		return 0;
	}


	virtual void service_pro(JD_INFO & jif)final
	{
		int using_index = getUncpl(jif);

		if (using_index < 0) {
			return;
		}
		MDC_CTRL &aim = jif.mdcCtrl[using_index];

		JD_FRAME jfr;

		float aimdeg = aim.manual_deg;
		unsigned int tmpdeg = Angle_Convert_UShort(aimdeg);
		printf("using %d ,cnt = %d, deg = %f\n", using_index, aim.retry_num, aimdeg);

		char databuff[3];
		databuff[0] = (tmpdeg >> (0 * 8)) & 0xff;
		databuff[1] = (tmpdeg >> (1 * 8)) & 0xff;
		databuff[2] = (tmpdeg >> (2 * 8)) & 0xff;

		jfr.jd_aim.value = using_index ? 0xbbbb00 : 0xaaaa00;

		jfr.jd_send_buff = &databuff;
		jfr.jd_data_len = 3;
		jfr.jd_command = 0xB;

		aim.retry_num++;
		JD_send(jif, jfr);
		return;
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

	jsvc.trig_cpl(jif, jfr);

	return JD_OK;
}


