#include "../wifi_svc.h"
#include "../wifi_ctrl.h"
#include "svc_once_write.h"
struct WIFI_QUERY_SINGLE_DATA :public WIFI_FUNCTION_ONCE_WRITE
{
	WIFI_QUERY_SINGLE_DATA(WIFI_INFO & info) :WIFI_FUNCTION_ONCE_WRITE(info)
	{
		PRO_MASK = WIFI_BASE_FUNCTION::MASK_READ;
		functionID = 2;
	}
	WIFI_BASE_SESSION thissec;
	WIFI_QUERY_SINGLE_DATA(WIFI_INFO & info, WIFI_BASE_SESSION & sec) :WIFI_FUNCTION_ONCE_WRITE(info)
	{
		PRO_MASK = WIFI_BASE_FUNCTION::MASK_WRITE;
		thissec = sec;
		functionID = 2;
	}


	virtual const char * FUNCTION_NAME() final
	{
		return "query single data";
	}


	virtual void read_pro_fun(WIFI_BASE_SESSION & sec)final
	{
		WIFI_DATA_SUB_PROTOCOL sub;
		mk_WIFI_DATA_SUB_PROTOCOL(sec, sub);

		if (sec.frame_index == -1 && sub.datalen == 2) {
			if (info.dbg_pri_msg) {
				printf("table =%d, id = %d\n"
					, sub.function_data[0]
					, sub.function_data[1]
				);
			}
			ADD_FUN(new WIFI_QUERY_SINGLE_DATA(info, sec));
		}
	}

	virtual void mk_write_session_data(WIFI_BASE_SESSION & sec) final
	{
		sec = thissec;
		sec.data_len += 1;

		WIFI_DATA_SUB_PROTOCOL sub;
		mk_WIFI_DATA_SUB_PROTOCOL(sec, sub);
		sub.function_data[2] = 5;
	}

	virtual void destor_write_fun()final
	{
		delete this;
	}
};


WIFI_BASE_FUNCTION * GetWIFI_QUERY_SINGLE_DATA(WIFI_INFO & wifi)
{
	return new WIFI_QUERY_SINGLE_DATA(wifi);
}



