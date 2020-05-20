#include "../wifi_svc.h"
#include "../wifi_ctrl.h"

//单次接收模板
//只适用于纯单次接收的功能
//重写read_pro_fun和接收掩码以及功能编号
struct WIFI_FUNCTION_ONCE_READ :public WIFI_BASE_FUNCTION
{
	WIFI_FUNCTION_ONCE_READ(WIFI_INFO & info) :WIFI_BASE_FUNCTION(info)
	{
		
	}

	virtual WIFI_PRO_STATUS wifi_read(WIFI_BASE_SESSION & sec) final
	{
		read_pro_fun(sec);
		return WIFI_PRO_STATUS::WIFI_PRO_END;
	}

	
	virtual WIFI_PRO_STATUS wifi_write(WIFI_BASE_SESSION & sec) final
	{
		return  WIFI_PRO_STATUS::WIFI_PRO_END;
	}

	virtual void DESTORY_FIRST(WIFI_INFO & info) final
	{
		delete this;
	}

	virtual void DESTORY_WRITE(WIFI_INFO & info)final
	{
	
	}

	virtual void read_pro_fun(WIFI_BASE_SESSION & sec) = 0;
};