#include "../wifi_svc.h"
#include "../wifi_ctrl.h"

struct WIFI_TEST_FUNCTION :public WIFI_BASE_FUNCTION
{
	WIFI_TEST_FUNCTION(WIFI_INFO & info) :WIFI_BASE_FUNCTION(info)
	{

	}

	virtual WIFI_PRO_STATUS wifi_read(WIFI_BASE_SESSION & sec) final
	{
		//WIFI_DATA_SUB_PROTOCOL *sub = (WIFI_DATA_SUB_PROTOCOL*)sec.data;

		return WIFI_PRO_STATUS::WIFI_PRO_END;
	}

	virtual WIFI_PRO_STATUS wifi_write(WIFI_BASE_SESSION & sec) final
	{
		return  WIFI_PRO_STATUS::WIFI_PRO_NEED_WRITE;
	}



	virtual void DESTORY_FIRST(WIFI_INFO & info) final
	{
		delete this;
	}

	virtual void DESTORY_WRITE(WIFI_INFO & info)final
	{

	}

	virtual const char * FUNCTION_NAME()final
	{
		return "test function";
	}
};


WIFI_BASE_FUNCTION * GetTEST(WIFI_INFO & wifi)
{
	return new WIFI_TEST_FUNCTION(wifi);
}



