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
#include <vector>
#include "SN1V2_com.h"
#include "svc_upload_file.h"
using namespace std;





struct WIFI_FUNCTION_UPLOAD_LOG_DAT :public WIFI_FUNCTION_UPLOADFILE_FILE_DAT
{
	WIFI_FUNCTION_UPLOAD_LOG_DAT(WIFI_INFO & info, int downloadindex, unsigned char *intim, int inmsgid)
		:WIFI_FUNCTION_UPLOADFILE_FILE_DAT(info, inmsgid)
	{
		functionID = 0x86;
		fileindex = downloadindex;
		memcpy(tim, intim, 4);
	}
	int fileindex = 0;
	unsigned char tim[4];



	virtual void load_data(vector<uint8_t> &dat) final
	{
		loadFile("./1.txt", dat);	
	}

	virtual int fil_first_frame_head(unsigned char * dat, int maxlen) final
	{
		dat[0] = fileindex;
		memcpy(&dat[1], tim, 4);
		return 5;
	}

	virtual const char * FUNCTION_NAME()
	{
		return "upload log fil";
	}

};




struct WIFI_FUNCTION_UPLOAD_LOG_HEAD :public WIFI_FUNCTION_UPLOADFILE_FILE_HEAD
{
	WIFI_FUNCTION_UPLOAD_LOG_HEAD(WIFI_INFO & info) :WIFI_FUNCTION_UPLOADFILE_FILE_HEAD(info)
	{
		functionID = 0x86;
	}
	virtual const char * FUNCTION_NAME()
	{
		return "upload log fil";
	}

	virtual void contrl_read(WIFI_DATA_SUB_PROTOCOL & sub) final
	{
		if (sub.datalen == 5) {
			ADD_FUN(new WIFI_FUNCTION_UPLOAD_LOG_DAT(
				info, sub.function_data[0]
				, &sub.function_data[1]
				, WIFI_BASE_FUNCTION::static_msg_id++
			));
		}
	}
};


WIFI_BASE_FUNCTION * Getuploadupatefile(WIFI_INFO & wifi)
{
	return new WIFI_FUNCTION_UPLOAD_LOG_HEAD(wifi);
}



