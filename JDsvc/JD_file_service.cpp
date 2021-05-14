#include <clockd_def.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <JDcomhead.h>
#include <stdio.h>
#include <thread>
#include <string.h>
#include "JDcomhead.h"
#include <fstream>
#include "SN1V2_error.h"
#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "mdc_ctrl.h"     /*******dms*20210121******/
#include <sys/stat.h>     /*******dms*20210121******/

using namespace std;

#if 0
static void writeBin(const char * path, JDFILETRANSFER & jfile)
{
	ofstream writefile(path);
	if (writefile.good()) {
		int pos0 = jfile.PackIndex * 200;
		writefile.seekp(pos0);
		writefile.write(jfile.pdata, jfile.datalen);
	}	
}
#endif

struct JDSN1UpdateBase {
	virtual int getCode() = 0;
	virtual int Work(unsigned char * buff, int len,unsigned char * retBuff) = 0;
	enum {
		JD_CODE_RESET = 0x10,
		JD_CODE_SET_FILE_LEN = 0x11,
		JD_CODE_DOWNLOAD_DATA = 0x12,
		
		JD_CODE_EXEC_UPDATE = 0x14,
		JD_CODE_UPLOAD_MD5 = 0x15,
		JD_CODE_FILE_VERSION = 0x16,
	};

	enum {
		FIX_JD_DATA_PACK_LEN = 200,
	};
};


static uint32_t crc_make(unsigned char* ptr, int len, uint32_t firstcrc)
{
	uint32_t crc = firstcrc;
	char i;
	int ptrindex = 0;
	while (len != 0) {
		if (len < 0)
			len = 0;
		crc ^= ptr[ptrindex];
		for (i = 0; i < 8; i++) {
			if ((crc & 0x0001) == 0)
				crc = crc >> 1;
			else {
				crc = crc >> 1;
				crc ^= 0xa001;
			}
		}
		len -= 1;
		ptrindex++;
	}
	return crc;
}



struct SN1Update_inf {
	vector<int> packLenGroup;
	bool startFlag = false;
	const char * getPath() { return testpath; }
	void Reset()
	{
		packLenGroup.clear();
		remove(getPath());
		GetFileLen = 0;
		if (update_tmpbuff) {
			free(update_tmpbuff);
			update_tmpbuff = 0;
		}
	}

	void set_restart()
	{
		Reset();
		startFlag = true;
	}

	void SetLen(int len)
	{
		GetFileLen = len;
		maxBlock = len / BlockSize;
		if (len % BlockSize) {
			maxBlock++;
		}
		packLenGroup.resize(maxBlock);
		update_tmpbuff = (unsigned char *)malloc(
			maxBlock * JDSN1UpdateBase::FIX_JD_DATA_PACK_LEN);

		if (!update_tmpbuff) {
			printf("malloc error \n");
		}
		printf("malloc = %d\n", maxBlock * JDSN1UpdateBase::FIX_JD_DATA_PACK_LEN);
	}
	int GetLen()
	{
		return GetFileLen;
	}

	int SetData(int PackIndex, int datalen, unsigned char * databuff)
	{
		packLenGroup[PackIndex] = datalen;

		if (PackIndex >= maxBlock) {
			return -1;
		}

		int pos0 = PackIndex * JDSN1UpdateBase::FIX_JD_DATA_PACK_LEN;
		memcpy(update_tmpbuff + pos0, databuff, datalen);
		return 0;
	}

	int LenCheck()
	{
		for (auto & p : packLenGroup) {
			if (p == 0) {
				return -1;
			}
		}
		return 0;
	}

	int DataCheck()
	{
		//char buff[200];
		printf("------------LenCheck = %d-----------\n", LenCheck());
		if (0 == LenCheck()) {

			printf("get len = %d\n", GetFileLen - 2);
			uint16_t testcrc;
			
			int len = strlen((char *)update_tmpbuff)-2;
			
			printf("-----rx date len = %d---------\n", len);

			if(len == 17)
			{
				testcrc = crc_make(&update_tmpbuff[2], len, 0xFFFF);
			}else
			{
				testcrc = crc_make(&update_tmpbuff[2], GetFileLen - 2, 0xFFFF);
			}
			
			
			#if 1
			//memcpy(buff, update_tmpbuff, 200);
			printf("---------------------------\n"); 
			printf("%s\n", &update_tmpbuff[2]);
			printf("---------------------------\n"); 
			//printf("%s\n", buff);
			//printf("---------------------------\n"); 
			#endif
			uint16_t getCRC;
			memcpy(&getCRC, &update_tmpbuff[0], 2);
			
			printf("0x%x\n", testcrc);
			printf("---------------------------\n"); 	
			printf("0x%x\n", getCRC);
			printf("---------------------------\n"); 
			
			
			if (testcrc == getCRC)
			{
				flushData(&update_tmpbuff[2], GetFileLen - 2);
				memset(update_tmpbuff, 0, GetFileLen);
				return 0;
			}
		}
		return -1;
	}

	void flushData(unsigned char * buff,int sz)
	{
		ofstream writefile(testpath, ios::binary);
		printf("---------writefile to testpath---------\n");
		if (writefile.good()) {
			writefile.write((const char *)buff, sz);
		}
		
	}

private:
	unsigned char * update_tmpbuff;
	//const char * testpath = "/tmp/update";
	const char * testpath = "/mnt/jaffs/tmp/update";
	int GetFileLen;
	const int BlockSize = 200;
	int maxBlock;
};



struct ResetCom : public JDSN1UpdateBase {
	ResetCom(SN1Update_inf & ppp) : inSi(ppp) {}

	SN1Update_inf & inSi;
	virtual int getCode() final { return JD_CODE_RESET; }
	virtual int Work(unsigned char * buff, int len, unsigned char * retBuff) final
	{
		inSi.set_restart();
		printf("SN1 reset all\n");
		return 0;
	}
};

struct SetLenCom : public JDSN1UpdateBase {
	SetLenCom(SN1Update_inf & pp) : inSi(pp) {}

	SN1Update_inf & inSi;
	virtual int getCode() final { return JD_CODE_SET_FILE_LEN; }
	virtual int Work(unsigned char * buff, int len, unsigned char * retBuff) final
	{
		if (!inSi.startFlag) {
			return -1;
		}

		retBuff[0] = 0;
		int32_t getlen;
		memcpy(&getlen, &buff[0], 4);
		printf("SN1 get len = %d \n", getlen);
		inSi.SetLen(getlen);
		int32_t silen = inSi.GetLen();
		memcpy(&retBuff[0], &silen, 4);
		return 4;
	}
};




struct FileGetData : public JDSN1UpdateBase {
	FileGetData(SN1Update_inf & pp) : inSi(pp) {}
	SN1Update_inf & inSi;

	virtual int getCode() final { return JD_CODE_DOWNLOAD_DATA; }
	virtual int Work(unsigned char * buff, int len, unsigned char * retBuff) final
	{
		if (!inSi.startFlag) {
			return -1;
		}

		if (len >= (2 + 4)) {
			int16_t buffIndex;
			int32_t PackIndex;

			memcpy(&buffIndex, &buff[0], 2);
			memcpy(&PackIndex, &buff[0 + 2], 4);

			int dataLen = len - 6;
			printf("get buff index %d,pack index %d,len = %d\n"
				, (int)buffIndex, (int)PackIndex, dataLen);
#if 0
			fileMap[PackIndex].push_back(dataLen);

			if (fileMap.size() > 100) {
				cout << "init" << endl << endl;
				for (auto & p : fileMap) {
					cout << "pack index = " << p.first;

					for (auto & num : p.second) {
						cout << ",num = " << num;
					}
					cout << endl;
				}
			}
#endif

			if (0 == inSi.SetData(PackIndex, dataLen, &buff[2 + 4])) {
				memcpy(&retBuff[0], &buffIndex, 2);
				return 2;
			} else {
				printf("data set error \n");
			}
		} else {
			printf("len error = %d\n", len);
		}
		return 0;
	}
};


void child_handler(int num)
{
	//SIGCHLD的信号
	int status;
	int pid = waitpid(-1, &status, WNOHANG);
	if (WIFEXITED(status)) {
		printf("The child %d exit with code %d\n", pid, WEXITSTATUS(status));
	}
}


struct ExecFileUpdate : public JDSN1UpdateBase {
	ExecFileUpdate(SN1Update_inf & pp) :inSi(pp) {}
	SN1Update_inf & inSi;

	virtual int getCode() final { return JD_CODE_EXEC_UPDATE; }
	virtual int Work(unsigned char * buff, int len, unsigned char * retBuff) final
	{
		if (!inSi.startFlag) {
			return -1;
		}

		int ret = inSi.DataCheck();
		if (ret == 0) {
			//注册信号
			signal(SIGCHLD, child_handler);
			int pid = fork();
			if (pid == 0) {
				printf("chmod\n");
				chmod(inSi.getPath(), 0777);
				//printf("%s\n",inSi.getPath());
				execv(inSi.getPath(), nullptr);
				exit(0);
			}
			printf("report update ok\n");
		} else {
			printf("updata error = %d\n", ret);
		}

		retBuff[0] = ret;
		return 1;
	}
};




static SN1Update_inf si;
static ResetCom rc(si);
static SetLenCom slc(si);
static FileGetData fgd(si);
static ExecFileUpdate efu(si);

JDSN1UpdateBase *fileGroup[] = {
	&rc,
	&slc,
	&fgd,
	&efu,
};



int JD_file_service(JD_INFO & jif, JD_FRAME & jfr)
{
	//MDC_INFO & jit = (MDC_INFO &)jif;
	static unsigned char tmpbuff[256];
	if (jfr.jd_data_len >= 1) {
		for (auto * p : fileGroup) {
			if (p->getCode() == jfr.jd_data_buff[0]) {
				int retLen = p->Work(&jfr.jd_data_buff[1], jfr.jd_data_len - 1, &tmpbuff[1]);

				if (retLen >= 0) {
					tmpbuff[0] = p->getCode() | 0x80;
					jfr.jd_send_buff = tmpbuff;
					jfr.jd_data_len = 1 + retLen;
					jfr.jd_command |= 0x80;
					JD_send(jif, jfr);
				}
			}
		}
	}
	return JD_OK;
}
