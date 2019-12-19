#include "JDcomhead.h"
#include "jd_share.h"
#include "svc.h"
#include <sys/time.h>

struct jdtimesvc :public JDAUTOSEND {

	int searchUncoplete(JD_INFO & jif)
	{
		timeval tv;
		gettimeofday(&tv, nullptr);

		for (auto &p : jif.mdcCtrl) {
			auto &sta = p.sta;

			if (tv.tv_sec != sta.last_tv.tv_sec) {
				sta.last_tv.tv_sec = tv.tv_sec;
				sta.trig_set_init();
				return  std::distance(&jif.mdcCtrl[0], &p);
			} else {
				if (sta.cpl_flag == 0 && sta.retry_num < sta.Max_retry) {
					return  std::distance(&jif.mdcCtrl[0], &p);
				}
			}
		}
		return -1;
	}


	virtual int need_service(JD_INFO & jif) final
	{
		if (searchUncoplete(jif) >= 0) {
			return 1;
		}
		return 0;
	}


	virtual void service_pro(JD_INFO & jif)final
	{
		int using_index = searchUncoplete(jif);
		if (using_index < 0) {
			return;
		}
		MDC_STA &aim = jif.mdcCtrl[using_index].sta;

		JD_FRAME jfr;

		jfr.jd_aim.value = jif.mdcCtrl[using_index].addr;
		jfr.jd_send_buff = nullptr;
		jfr.jd_data_len = 0;
		jfr.jd_command = 0xd;
		aim.retry_num++;

		JD_send(jif, jfr);
	}

	void trig_cpl(JD_INFO & jif, JD_FRAME & jfr)
	{
		int getIndex = findMdc_addr(jif, jfr.jd_aim.value);

		if (getIndex < 0) {
			printf("bad addr = 0x%x\n", jfr.jd_aim.value);
			return;
		}
		MDC_STA & sta = jif.mdcCtrl[getIndex].sta;

		if (jfr.jd_data_len != 3) {
			printf("bad rec len = %d\n", jfr.jd_data_len);
			return;
		}		
		sta.cpl_flag = 1;
		gettimeofday(&sta.last_tv, nullptr);
	}
};

static jdtimesvc jsvc;

JDAUTOSEND * jdsvc_time()
{
	return &jsvc;
}


int JD_time_rec(JD_INFO & jif, JD_FRAME & jfr)
{
	JD_INFO_TIM & jit = (JD_INFO_TIM &)jif;
	jsvc.trig_cpl(jif, jfr);
	return JD_OK;
}


