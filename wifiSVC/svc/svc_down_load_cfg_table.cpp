#include "../wifi_svc.h"
#include "../wifi_ctrl.h"
#include "svc_download_file.h"
#include <string.h>
#include "jd_share.h"
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;



struct WIFI_FUNCTION_DOWNLOAD_CFG_TABLE :public WIFI_FUNCTION_DOWNLOAD_FILE
{
	WIFI_FUNCTION_DOWNLOAD_CFG_TABLE(WIFI_INFO & info) :WIFI_FUNCTION_DOWNLOAD_FILE(info)
	{
		PRO_MASK = WIFI_BASE_FUNCTION::MASK_READ;
		functionID = 0x80;
	}

	WIFI_FUNCTION_DOWNLOAD_CFG_TABLE(WIFI_INFO & info, int downloadindex) :WIFI_FUNCTION_DOWNLOAD_FILE(info)
	{
		PRO_MASK = WIFI_BASE_FUNCTION::MASK_SELF_DOWNLOAD;
		functionID = 0x80;
		fileindex = downloadindex;
	}
	int fileindex = 0;

	virtual void contrl_read(WIFI_DATA_SUB_PROTOCOL & sub) final
	{
		ADD_FUN(new WIFI_FUNCTION_DOWNLOAD_CFG_TABLE(info, sub.function_data[0]));
	}
	virtual int first_data_frame(WIFI_BASE_SESSION & sec, uint16_t &outcrc, uint32_t &outlen)final
	{
		unsigned char * pdat = &sec.data[2];
		outcrc = pdat[0];
		outcrc |= pdat[1] << 8;

		outlen = pdat[2] << 0;
		outlen |= pdat[3] << 8;
		outlen |= pdat[4] << 16;
		outlen |= pdat[5] << 24;

		if (info.dbg_pri_msg) {
			printf("need downfile file index = %d,len = %d , crc = %x,\n"
				, sec.data[1]
				, outlen
				, outcrc
			);
		}
		return 0;
	}

	virtual int data_finish(char * data, int len)final
	{
		const CFG_GROUP * grp = find_group_by_cfg_index(fileindex);
		if (grp) {
			const char * updatepath = grp->cfgName;
			if (writebin(updatepath, data, len)) {
				return 0;
			}
		}
		return -1;
	}

	virtual int memcpy_write_fix_dat(unsigned char buff[], int maxlen) final
	{
		buff[0] = fileindex;
		return 1;
	}

	virtual const char * FUNCTION_NAME() final
	{
		return "download cfg file";
	}
};


WIFI_BASE_FUNCTION * Getdownloadcfgtable(WIFI_INFO & wifi)
{
	return new WIFI_FUNCTION_DOWNLOAD_CFG_TABLE(wifi);
}



