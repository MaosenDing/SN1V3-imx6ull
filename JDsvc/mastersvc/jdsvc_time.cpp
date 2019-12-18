#include "JDcomhead.h"
#include "jd_share.h"
#include "svc.h"
#include <sys/time.h>

struct jdtimesvc :public JDAUTOSEND {

	timeval last_tv;

	virtual int need_service(JD_INFO & jif) final
	{
		return 0;
		printf("nnn\n");
		timeval tv;
		gettimeofday(&tv, nullptr);

		if (tv.tv_sec == last_tv.tv_sec) {
			return 0;
		}
		return 1;
	}


	virtual void service_pro(JD_INFO & jif)final
	{
		JD_FRAME jfr;

		unsigned char testbuff[] = "send test";

		jfr.jd_send_buff = &testbuff;
		jfr.jd_data_len = sizeof(testbuff);
		jfr.jd_command = 0x34;

		JD_send(jif, jfr);
	}

	void trig_cpl()
	{
		timeval tv;
		gettimeofday(&tv, nullptr);
		last_tv.tv_sec = tv.tv_sec;
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
	jsvc.trig_cpl();
	//printf("rec data len = %d \n", jfr.jd_data_len);
	return JD_OK;
}


