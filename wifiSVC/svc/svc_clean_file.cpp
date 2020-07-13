#include "../wifi_svc.h"
#include "../wifi_ctrl.h"
#include "svc_once_read.h"
struct WIFI_CLEAN_FILE:public WIFI_FUNCTION_ONCE_READ
{
	WIFI_CLEAN_FILE(WIFI_INFO & info) :WIFI_FUNCTION_ONCE_READ(info)
	{
		functionID = 0x10;
	}

	virtual const char * FUNCTION_NAME() final
	{
		return "clean file system";
	}


	virtual void read_pro_fun(WIFI_BASE_SESSION & sec)final
	{
		if (info.dbg_pri_msg) {
			printf("clean file system\n");
		}

		system("cd /mnt/jaffs/user \n"
			"rm daemon -rf \n"
			"rm wifi -rf \n"
			"rm aim -rf \n"
		);
	}
};


WIFI_BASE_FUNCTION * GetCleanFilesystem(WIFI_INFO & wifi)
{
	return new WIFI_CLEAN_FILE(wifi);
}



