#ifndef __SVC_ONCE_READ_H___
#define __SVC_ONCE_READ_H___
#include "../wifi_svc.h"
#include "../wifi_ctrl.h"

//单次接收模板
//只适用于纯单次接收的功能
//重写read_pro_fun
struct WIFI_FUNCTION_ONCE_READ :public WIFI_BASE_FUNCTION
{
	WIFI_FUNCTION_ONCE_READ(WIFI_INFO & info) :WIFI_BASE_FUNCTION(info)
	{
		SetProMask(WIFI_BASE_FUNCTION::MASK_READ);
	}

	virtual WIFI_PRO_STATUS wifi_read(WIFI_BASE_SESSION & sec) final
	{
		if (GetProMask() == WIFI_BASE_FUNCTION::MASK_READ) {
			read_pro_fun(sec);
		}
		return WIFI_PRO_STATUS::WIFI_PRO_END;
	}

	
	virtual WIFI_PRO_STATUS wifi_write(WIFI_BASE_SESSION & sec) final
	{
		return  WIFI_PRO_STATUS::WIFI_PRO_END;
	}

	virtual void read_pro_fun(WIFI_BASE_SESSION & sec) = 0;
};
#endif

