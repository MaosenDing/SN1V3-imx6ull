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


struct WIFI_FUNCTION_UPLOAD_FILE :public WIFI_FUNCTION_UPLOADFILE_FILE
{
	WIFI_FUNCTION_UPLOAD_FILE(WIFI_INFO & info) :WIFI_FUNCTION_UPLOADFILE_FILE(info)
	{
		PRO_MASK = WIFI_BASE_FUNCTION::MASK_READ;
		functionID = 0x86;
	}

	WIFI_FUNCTION_UPLOAD_FILE(WIFI_INFO & info, int downloadindex, unsigned char *intim) :WIFI_FUNCTION_UPLOADFILE_FILE(info)
	{
		PRO_MASK = WIFI_BASE_FUNCTION::MASK_SELF_UPLOAD;
		functionID = 0x86;
		fileindex = downloadindex;
		memcpy(tim, intim, 4);
	}
	int fileindex = 0;
	unsigned char tim[4];

	virtual void contrl_read(WIFI_DATA_SUB_PROTOCOL & sub) final
	{
		if (sub.datalen == 5) {
			ADD_FUN(new WIFI_FUNCTION_UPLOAD_FILE(
				info, sub.function_data[0], &sub.function_data[1]));
		}
	}

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


WIFI_BASE_FUNCTION * Getuploadupatefile(WIFI_INFO & wifi)
{
	return new WIFI_FUNCTION_UPLOAD_FILE(wifi);
}



