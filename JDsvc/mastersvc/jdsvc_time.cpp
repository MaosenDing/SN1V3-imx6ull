#include "JDcomhead.h"
#include "jd_share.h"
#include "svc.h"
#include <sys/time.h>
#include <string.h>
#include "SN1V2_com.h"
#include <iostream>
#include <math.h>
#include "SN1V2_com.h"
using namespace  std;
struct jdtimesvc :public JDAUTOSEND {

	uint64_t difftv_ms(timeval & ntv, timeval &ltv)
	{
		int64_t ret = (ntv.tv_sec - ltv.tv_sec) * 1000 + (ntv.tv_usec - ltv.tv_usec) / 1000;
		return ret > 0 ? ret : -ret;
	}



	int searchUncoplete(JD_INFO & jif)
	{
		timeval tv;
		gettimeofday(&tv, nullptr);

		for (auto &p : jif.mdcCtrl) {
			auto &sta = p.sta;

			if (difftv_ms(tv,sta.last_tv) > 300) {
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
		jfr.jd_command = 0x13;
		aim.retry_num++;

		JD_send(jif, jfr);
	}

	bool checkDeg(JD_INFO & jif)
	{		
		for (auto & p : jif.mdcCtrl) {
			double tmp = p.manual.manual_deg - p.sta.deg;
			if (fabs(tmp) > 0.05f) {
				return false;
			}
		}
		return true;
	}


	void trig_cpl(JD_INFO & jif, JD_FRAME & jfr)
	{
		int getIndex = findMdc_addr(jif, jfr.jd_aim.value);

		if (getIndex < 0) {
			printf("bad addr = 0x%x\n", jfr.jd_aim.value);
			return;
		}
		MDC_STA & sta = jif.mdcCtrl[getIndex].sta;

		if (jfr.jd_data_len != 9) {
			printf("bad rec len = %d\n", jfr.jd_data_len);
			return;
		}
		
		sta.deg = Angle_Convert(jfr.jd_data_buff);

		sta.temperature = jfr.jd_data_buff[3];
		sta.current = jfr.jd_data_buff[4];
		memcpy(sta.status, &jfr.jd_data_buff[5], 4);


		char name[128];
		snprintf(name, 128, "%s%d", MDC_STATUS_FILE, getIndex);

		char buff[128];
		
		sta.statusint = sta.status[0]
			| sta.status[1] << 8
			| sta.status[2] << 16
			| sta.status[3] << 24;

		sta.runningflg = (sta.statusint & 1 << 9) ? 0 : 1;

		int len = snprintf(buff, 128, "addr %x , deg %f,tem %d,cur %d ,%02x %02x %02x %02x run %d\n", jif.mdcCtrl[getIndex].addr,
			sta.deg, sta.temperature, sta.current
			, sta.status[0], sta.status[1], sta.status[2], sta.status[3], sta.runningflg
		);


		gettimeofday(&sta.last_tv, nullptr);

		if (checkDeg(jif)) {
			auto &pp = (JD_INFO_TIM &)jif;
			auto psn1 = pp.psn1;
			psn1->helo_status = psn1->Helo_ok;
		}
		printf(buff);

		saveBin(name, buff, len);
		sta.cpl_flag = 1;
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


