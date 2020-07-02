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
using namespace std;


struct WIFI_FUNCTION_UPLOAD_FILE :public WIFI_BASE_FUNCTION
{
	WIFI_FUNCTION_UPLOAD_FILE(WIFI_INFO & info) :WIFI_BASE_FUNCTION(info)
	{
		PRO_MASK = WIFI_BASE_FUNCTION::MASK_READ;
		functionID = 0x86;
	}

	WIFI_FUNCTION_UPLOAD_FILE(WIFI_INFO & info, int downloadindex, unsigned char *intim) :WIFI_BASE_FUNCTION(info)
	{
		PRO_MASK = WIFI_BASE_FUNCTION::MASK_SELF_UPLOAD;
		functionID = 0x86;
		fileindex = downloadindex;
		memcpy(tim, intim, 4);
	}
	int fileindex = 0;
	unsigned char tim[4];

	int usingindex = 0;
	int MaxIndex = 0;

	int msgid = 0;

	vector<uint8_t> dat;
#define HEAD_LEN (4)
	virtual WIFI_PRO_STATUS wifi_read(WIFI_BASE_SESSION & sec) final
	{
		WIFI_DATA_SUB_PROTOCOL sub;
		mk_WIFI_DATA_SUB_PROTOCOL(sec, sub);
		//控制信令
		if (PRO_MASK == WIFI_BASE_FUNCTION::MASK_READ) {
			if (sec.frame_index == -1) {
				if (info.dbg_pri_msg) {
					printf("len = %d ,fil id = %d,tim = %d:%d - %d:%d\n"
						, sub.datalen
						, sub.function_data[0]
						, sub.function_data[1], sub.function_data[2]
						, sub.function_data[3], sub.function_data[4]
					);
				}
				if (sub.datalen == 5) {
					ADD_FUN(new WIFI_FUNCTION_UPLOAD_FILE(
						info, sub.function_data[0], &sub.function_data[1]));
				}
			}
			return WIFI_PRO_STATUS::WIFI_PRO_END;
		}

		//数据信令
		if (PRO_MASK == WIFI_BASE_FUNCTION::MASK_SELF_UPLOAD) {
			printf("MASK_SELF_UPLOAD get frame index = %d,using index = %d \n", sec.frame_index, usingindex);
			if (sec.frame_index == usingindex) {
				usingindex++;
				if (usingindex > MaxIndex) {
					return WIFI_PRO_STATUS::WIFI_PRO_END;
				}
			} else {
				printf("MASK_SELF_UPLOAD get frame index = %d \n", sec.frame_index);
			}
		}
		return WIFI_PRO_STATUS::WIFI_PRO_NEED_WRITE;
	}

	size_t fil_data(unsigned char * buff, size_t packlen, size_t packindex)
	{
		size_t sz = dat.size();
		printf("out index = %d,",packindex);
		const unsigned char * srcdat = &dat[0];
		if (packindex >= MaxIndex || packindex <= 0) {
			printf("end frame\n");
			return 0;
		}
		size_t srcpos = (packindex - 1) * packlen;		
		int len = ((sz - srcpos) > packlen) ? packlen : (sz - srcpos);
		printf("cp pos = %d,len = %d\n", srcpos, len);
		memcpy(buff, &srcdat[srcpos], len);
		return len;
	}


	virtual WIFI_PRO_STATUS wifi_write(WIFI_BASE_SESSION & sec) final
	{
		int sigpack = MAX_PACK_SZ - MIN_PACK_SZ - HEAD_LEN;
		if (usingindex == 0) {
			loadFile("./1.txt", dat);

			unsigned int calcrc = crc_make((unsigned char *)&dat[0], dat.size(), 0xffff);

			if (dat.size()) {
				MaxIndex =
					dat.size() / sigpack //满包 
					+ ((dat.size() % sigpack) ? 1 : 0)//半满包 
					+ 1//空包					
					;
			} else {
				MaxIndex = 0;
			}
			printf("sigpack = %d , maxindex = %d \n", sigpack, MaxIndex);

			sec.data_len = 0;

#if HEAD_LEN >= 3
			sec.data[sec.data_len++] = msgid;
			sec.data[sec.data_len++] = msgid >> 8;
#endif
			sec.data[sec.data_len++] = functionID;

			sec.data[sec.data_len++] = fileindex;

			memcpy(&sec.data[sec.data_len], tim, 4);
			sec.data_len += 4;

			sec.data[sec.data_len++] = calcrc;
			sec.data[sec.data_len++] = calcrc >> 8;

			int32_t sz = dat.size();
			memcpy(&sec.data[sec.data_len], &sz, 4);
			sec.data_len += 4;

			sec.frame_index = 0;

			if (info.dbg_pri_msg) {
				printf("snd fil,len = %d,crc = %x\n"
					, sz, calcrc
				);
			}
		} else {
			sec.data_len = 0;
#if HEAD_LEN >= 3
			sec.data[sec.data_len++] = msgid;
			sec.data[sec.data_len++] = msgid >> 8;
#endif
			sec.data[sec.data_len++] = functionID;
#if HEAD_LEN >= 3
			sec.data[sec.data_len++] = fileindex;
#endif
			int len = fil_data(&sec.data[sec.data_len], sigpack, usingindex);

			sec.data_len = HEAD_LEN + len;

			sec.frame_index = usingindex;
		}
		return  WIFI_PRO_STATUS::WIFI_PRO_END;
	}

	virtual void destor_write_fun()final
	{
		delete this;
	}

	virtual const char * FUNCTION_NAME() final
	{
		return "upload log file";
	}

	virtual void DESTORY_FIRST(WIFI_INFO & info) final
	{
		delete this;
	}

	virtual void DESTORY_WRITE(WIFI_INFO & info) final
	{
		destor_write_fun();
	}

};


WIFI_BASE_FUNCTION * Getuploadupatefile(WIFI_INFO & wifi)
{
	return new WIFI_FUNCTION_UPLOAD_FILE(wifi);
}



