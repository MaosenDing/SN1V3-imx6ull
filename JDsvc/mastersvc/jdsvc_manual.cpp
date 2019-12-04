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
	}
	std::mutex tableLock;

	void trig_cpl(JD_FRAME & jfr)
	{

	}
	const int max_retry_cnt = 5;
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

		uint64_t now_send_tim = tv.tv_sec / send_period_s;

		if (now_send_tim == last_send_tim) {
			for (auto &p : aim) {
				if (p.succ_flag == 0 && p.retry_cnt < max_retry_cnt) {
					using_index = std::distance(&aim[0], &p);
					return 1;
				}
			}
		} else {
			last_send_tim = now_send_tim;
			for (auto &p : aim) {
				p.succ_flag = 0;
				p.retry_cnt = 0;
			}
			using_index = 0;
			return 1;
		}
		return 0;
	}

	virtual void service_pro(JD_INFO & jif)final
	{
		for (auto &thisaim : aim) {
			if (thisaim.succ_flag == 0 && thisaim.retry_cnt < max_retry_cnt) {
				thisaim.retry_cnt++;

				JD_FRAME jfr;

				char databuff[64];

				printf("using %d ,cnt = %d\n", using_index, thisaim.retry_cnt);
				int len = snprintf(databuff, 64, "set %f\n",jif.manual_deg[using_index]);
				printf(databuff);

				jfr.jd_aim.value = using_index;
				jfr.jd_send_buff = &databuff;
				jfr.jd_data_len = len;
				jfr.jd_command = 0x35;

				JD_send(jif, jfr);
				return;
			}
		}
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


