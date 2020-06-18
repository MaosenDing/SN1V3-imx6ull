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

	char *pbuffer = 0;

	int writebin(const char * path, const char * data, const int len)
	{
		int goodflg = 0;
		{
			ofstream writefile("./update", ios::binary);
			if (writefile.good()) {
				writefile.write(pbuffer, len);
				goodflg = 1;
			}
		}
		sync();
		return goodflg;
	}


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
				usingindex++;
				if (pbuffer) {
					delete pbuffer;
				}

				pbuffer = new char[len];

				if (!pbuffer) {
					printf("error no enough memory\n");
					exit(-1);
				}

				if (info.dbg_pri_msg) {
					printf("need downfile file index = %d,len = %d , crc = %x,\n"
						, sec.data[1]
						, len
						, crc
					);
				}
			} else if (sec.frame_index == -1) {
				if (info.dbg_pri_msg)
					printf("waiting...len = %d\n", sec.data_len);
			} else if (sec.frame_index == -3) {
				//完成标志
				unsigned int calcrc = crc_make((unsigned char *)pbuffer, len, 0xffff);

				if (calcrc == crc) {
					if (info.dbg_pri_msg)
						printf("crc check ok\n");
					const char * updatepath = "./update";
					if (writebin(updatepath, pbuffer, len)) {
						int pid = fork();
						if (pid == 0) {
							printf("chmod\n");
							chmod(updatepath, 0777);
							execv(updatepath, nullptr);
							exit(0);
						} else {
							signal(SIGCHLD, child_handler);
						}
					}
				} else {
					if (info.dbg_pri_msg)
						printf("crc check error,rec crc = %x ,cal crc = %x,len = %d\n"
							, crc, calcrc, len);

				}
				delete[] pbuffer;
				return WIFI_PRO_STATUS::WIFI_PRO_END;
			} else if (sec.frame_index == usingindex) {
				//获取到数据
				unsigned char * dat = &sec.data[1];
				int datlen = sec.data_len - 1;
				char * buffpos = &pbuffer[(1024 - MIN_PACK_SZ - 1) * (sec.frame_index - 1)];
				memcpy(buffpos, dat, datlen);
				if (info.dbg_pri_msg) {
					printf("get frame index = %d\n"
						, sec.frame_index
					);
				}
				usingindex++;
			} else {
				//其他错误信息
				if (info.dbg_pri_msg) {
					printf("get frame index error = %d\n", sec.frame_index);
				}
			}
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



