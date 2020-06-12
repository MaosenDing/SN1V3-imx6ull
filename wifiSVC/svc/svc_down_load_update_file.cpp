#include "../wifi_svc.h"
#include "../wifi_ctrl.h"
#include "svc_download_file.h"
struct WIFI_FUNCTION_DOWNLOAD_UPDATE_FILE :public WIFI_FUNCTION_DOWNLOAD_FILE
{
	WIFI_FUNCTION_DOWNLOAD_UPDATE_FILE(WIFI_INFO & info) :WIFI_FUNCTION_DOWNLOAD_FILE(info)
	{
		PRO_MASK = WIFI_BASE_FUNCTION::MASK_READ;
		functionID = 0x83;
	}
	
	WIFI_FUNCTION_DOWNLOAD_UPDATE_FILE(WIFI_INFO & info, int downloadindex) :WIFI_FUNCTION_DOWNLOAD_FILE(info)
	{
		PRO_MASK = WIFI_BASE_FUNCTION::MASK_SELF_DOWNLOAD;
		functionID = 0x83;
		fileindex = downloadindex;
	}
	int fileindex = 0;
	uint16_t crc;
	uint32_t len;
	int usingindex = 0;
	virtual WIFI_PRO_STATUS wifi_read(WIFI_BASE_SESSION & sec) final
	{
		WIFI_DATA_SUB_PROTOCOL sub;
		mk_WIFI_DATA_SUB_PROTOCOL(sec, sub);
		//控制信令
		if (PRO_MASK == WIFI_BASE_FUNCTION::MASK_READ) {
			if (sec.frame_index == -1) {
				if (sub.datalen == 1) {
					if (info.dbg_pri_msg) {
						printf("need downfile index = %d\n", sub.function_data[0]);
					}
					ADD_FUN(new WIFI_FUNCTION_DOWNLOAD_UPDATE_FILE(info, sub.function_data[0]));
				}
			}
		}
		//数据信令
		if (PRO_MASK == WIFI_BASE_FUNCTION::MASK_SELF_DOWNLOAD) {
			if (sec.frame_index == 0) {
				unsigned char * pdat = &sec.data[2];
				crc = pdat[0];
				crc |= pdat[1] << 8;

				len = pdat[2] << 0;
				len |= pdat[3] << 8;
				len |= pdat[4] << 16;
				len |= pdat[5] << 24;

				if (info.dbg_pri_msg) {
					printf("need downfile index = %d,len = %d , crc = %x,\n"
						, sec.data[1]
						, len
						, crc
					);
				}
			} else {
				if (info.dbg_pri_msg) {
					printf("get frame index = %d\n"
						, sec.frame_index
					);
				}
			}
			usingindex++;
		}
		return WIFI_PRO_STATUS::WIFI_PRO_NEED_WRITE;
	}

	virtual WIFI_PRO_STATUS wifi_write(WIFI_BASE_SESSION & sec) final
	{
		sec.data_len = 2;
		sec.data[0] = functionID;
		sec.data[1] = fileindex;
		sec.frame_index = usingindex;

		if (info.dbg_pri_msg) {
			printf("try get frame index = %d\n"
				, usingindex
			);
		}
		return  WIFI_PRO_STATUS::WIFI_PRO_END;
	}

	virtual void destor_write_fun()final
	{
		delete this;
	}

	virtual const char * FUNCTION_NAME() final
	{
		return "download update file";
	}
};


WIFI_BASE_FUNCTION * Getdownloadupatefile(WIFI_INFO & wifi)
{
	return new WIFI_FUNCTION_DOWNLOAD_UPDATE_FILE(wifi);
}



