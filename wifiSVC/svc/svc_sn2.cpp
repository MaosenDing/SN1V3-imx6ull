#include "../wifi_svc.h"
#include "../wifi_ctrl.h"
#include "svc_once_read.h"
struct WIFI_SN2_CENTER_FUNCTION :public WIFI_FUNCTION_ONCE_READ
{
	WIFI_SN2_CENTER_FUNCTION(WIFI_INFO & info) :WIFI_FUNCTION_ONCE_READ(info)
	{
		PRO_MASK = WIFI_BASE_FUNCTION::MASK_READ;
		functionID = 6;
	}

	virtual const char * FUNCTION_NAME() final
	{
		return "sn2 center tag";
	}


	virtual void read_pro_fun(WIFI_BASE_SESSION & sec)final
	{
		WIFI_DATA_SUB_PROTOCOL sub;
		mk_WIFI_DATA_SUB_PROTOCOL(sec, sub);

		if (info.dbg_pri_msg) {

			printf("sn2 center start %d:%d, end %d:%d\n"
				, sub.function_data[0]
				, sub.function_data[1]
				, sub.function_data[2]
				, sub.function_data[3]
			);
		}
	}
};


WIFI_BASE_FUNCTION * GetSn2Center(WIFI_INFO & wifi)
{
	return new WIFI_SN2_CENTER_FUNCTION(wifi);
}



