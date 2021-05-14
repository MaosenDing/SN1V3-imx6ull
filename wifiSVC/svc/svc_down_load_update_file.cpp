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
#include "svc_once_read.h"
using namespace std;

void child_handler(int num)
{
	//SIGCHLD的信号
	int status;
	int pid = waitpid(-1, &status, WNOHANG);
	if (WIFEXITED(status)) {
		printf("The child %d exit with code %d\n", pid, WEXITSTATUS(status));
	}
}



struct WIFI_FUNCTION_DOWNLOAD_UPDATE_FILE :public WIFI_FUNCTION_DOWNLOAD_FILE
{
	WIFI_FUNCTION_DOWNLOAD_UPDATE_FILE(WIFI_INFO & info, int downloadindex) :WIFI_FUNCTION_DOWNLOAD_FILE(info)
	{
		functionID = 0x83;
		fileindex = downloadindex;
	}
	int fileindex = 0;

	virtual void contrl_read(WIFI_DATA_SUB_PROTOCOL & sub) final
	{
		ADD_FUN(new WIFI_FUNCTION_DOWNLOAD_UPDATE_FILE(info, sub.function_data[0]));
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
		const char * updatepath = "/tmp/update";
		//const char * updatepath = "/home/root/";
		if (writebin(updatepath, data, len)) {
			signal(SIGCHLD, child_handler);
			int pid = fork();
			if (pid == 0) {
				printf("exev %s\n", updatepath);
				chmod(updatepath, 0777);
				execl(updatepath, updatepath, (char *)NULL);
				exit(0);
			}
		}
		return 0;
	}

	virtual int memcpy_write_fix_dat(unsigned char buff[], int maxlen) final
	{
		buff[0] = fileindex;
		return 1;
	}

	virtual const char * FUNCTION_NAME() final
	{
		return "download update file";
	}
};

struct WIFI_FUNCTION_DOWNLOAD_UPDATE_FILE_HEAD :public WIFI_FUNCTION_ONCE_READ
{
	WIFI_FUNCTION_DOWNLOAD_UPDATE_FILE_HEAD(WIFI_INFO & info) :WIFI_FUNCTION_ONCE_READ(info)
	{
		functionID = 0x83;
	}

	virtual void read_pro_fun(WIFI_BASE_SESSION & sec) final
	{
		WIFI_DATA_SUB_PROTOCOL sub;
		mk_WIFI_DATA_SUB_PROTOCOL(sec, sub);
		//控制信令
		if (GetProMask() == WIFI_BASE_FUNCTION::MASK_READ) {
			if (sec.frame_index == -1) {
				if (sub.datalen == 1) {
					if (info.dbg_pri_msg) {
						printf("need downfile index = %d\n", sub.function_data[0]);
					}
					ADD_FUN(new WIFI_FUNCTION_DOWNLOAD_UPDATE_FILE(info, sub.function_data[0]));
				}
			}
		}
	}
	virtual const char * FUNCTION_NAME() final
	{
		return "download update file";
	}
};



WIFI_BASE_FUNCTION * Getdownloadupatefile(WIFI_INFO & wifi)
{
	return new WIFI_FUNCTION_DOWNLOAD_UPDATE_FILE_HEAD(wifi);
}



