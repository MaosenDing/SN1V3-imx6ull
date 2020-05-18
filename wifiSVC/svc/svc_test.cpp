#include "../wifi_svc.h"
#include "../wifi_ctrl.h"



WIFI_PRO_STATUS wifi_test_read(WIFI_INFO & wifi, WIFI_BASE_SESSION & sec)
{
	//WIFI_DATA_SUB_PROTOCOL *sub = (WIFI_DATA_SUB_PROTOCOL*)sec.data;

	return WIFI_PRO_STATUS::WIFI_PRO_END;
}


int wifi_test_write(WIFI_INFO & wifi, WIFI_BASE_SESSION & sec)
{
	return 0;
}


