#include "JDcomhead.h"
#include "jd_share.h"
#include "svc.h"


struct jdtimesvc :public JDAUTOSEND {
	virtual int need_service() final
	{
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
};

static jdtimesvc jsvc;

JDAUTOSEND * jdsvc_time()
{
	return &jsvc;
}


int JD_time_rec(JD_INFO & jif, JD_FRAME & jfr)
{
	JD_INFO_TIM & jit = (JD_INFO_TIM &)jif;
	printf("rec data len = %d \n", jfr.jd_data_len);
	return JD_OK;
}


