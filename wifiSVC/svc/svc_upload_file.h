#ifndef __SVC_UPLOAD_FILE_H___
#define __SVC_UPLOAD_FILE_H___
#include "../wifi_svc.h"
#include "../wifi_ctrl.h"
#include "svc_once_read.h"
#include <mutex>
using namespace  std;

struct WIFI_FUNCTION_UPLOADFILE_FILE_HEAD :public WIFI_FUNCTION_ONCE_READ
{
	WIFI_FUNCTION_UPLOADFILE_FILE_HEAD(WIFI_INFO & info) :WIFI_FUNCTION_ONCE_READ(info)
	{
	}

	virtual void read_pro_fun(WIFI_BASE_SESSION & sec) final
	{
		WIFI_DATA_SUB_PROTOCOL sub;
		mk_WIFI_DATA_SUB_PROTOCOL(sec, sub);
		//控制信令
		if (sec.frame_index == -1) {
			if (info.dbg_pri_msg) {
				printf("len = %d ,fil id = %d,tim = %d:%d - %d:%d\n"
					, sub.datalen
					, sub.function_data[0]
					, sub.function_data[1], sub.function_data[2]
					, sub.function_data[3], sub.function_data[4]
				);
			}
			contrl_read(sub);
		}
	}
	virtual void contrl_read(WIFI_DATA_SUB_PROTOCOL & sub) = 0;
};


struct WIFI_FUNCTION_UPLOADFILE_FILE_DAT :public WIFI_BASE_FUNCTION
{
	WIFI_FUNCTION_UPLOADFILE_FILE_DAT(WIFI_INFO & info) :WIFI_BASE_FUNCTION(info)
	{
		SetProMask(WIFI_BASE_FUNCTION::MASK_SELF_UPLOAD);
	}
	virtual void load_data(vector<uint8_t> &dat) = 0;
	virtual int fil_first_frame_head(unsigned char * dat, int maxlen) = 0;

	//数据部分需要添加的特殊数据
	virtual int data_frame_fil_head(unsigned char * dat, int maxlen) { return 0; }
	virtual int data_frame_fix_len() { return 0; }
#define HEAD_LEN (3)
	virtual WIFI_PRO_STATUS wifi_read(WIFI_BASE_SESSION & sec) final
	{
		WIFI_DATA_SUB_PROTOCOL sub;
		mk_WIFI_DATA_SUB_PROTOCOL(sec, sub);

		//数据信令
		if (GetProMask() == WIFI_BASE_FUNCTION::MASK_SELF_UPLOAD) {
			if (info.dbg_pri_msg) printf("MASK_SELF_UPLOAD get frame index = %d,using index = %d \n", sec.frame_index, usingindex);
			if (sec.frame_index == usingindex) {
				usingindex++;
				if (usingindex > MaxIndex) {
					return WIFI_PRO_STATUS::WIFI_PRO_END;
				}
			} else {
				if (info.dbg_pri_msg) printf("MASK_SELF_UPLOAD get frame index = %d \n", sec.frame_index);
			}
		}
		return WIFI_PRO_STATUS::WIFI_PRO_NEED_WRITE;
	}

	size_t fil_data(unsigned char * buff, size_t packlen, size_t packindex)
	{
		size_t sz = dat.size();
		if (info.dbg_pri_msg) printf("out index = %d,", packindex);
		const unsigned char * srcdat = &dat[0];
		if (packindex > MaxIndex || packindex <= 0) {
			if (info.dbg_pri_msg) printf("end frame\n");
			return 0;
		}
		size_t srcpos = (packindex - 1) * packlen;
		int len = ((sz - srcpos) > packlen) ? packlen : (sz - srcpos);
		if (info.dbg_pri_msg) printf("cp pos = %d,len = %d\n", srcpos, len);
		memcpy(buff, &srcdat[srcpos], len);
		return len;
	}


	virtual WIFI_PRO_STATUS wifi_write(WIFI_BASE_SESSION & sec) final
	{
		int sigpack = MAX_PACK_SZ - MIN_PACK_SZ - HEAD_LEN - data_frame_fix_len();
		if (usingindex == 0) {
			load_data(dat);

			unsigned int calcrc = crc_make((unsigned char *)&dat[0], dat.size(), 0xffff);

			if (dat.size()) {
				MaxIndex =
					dat.size() / sigpack //满包 
					+ 1//空包 或者 半满包				
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

			int len = fil_first_frame_head(&sec.data[sec.data_len], 100);

			sec.data_len += len;

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

			sec.data_len += data_frame_fil_head(&sec.data[sec.data_len], 100);

			int len = fil_data(&sec.data[sec.data_len], sigpack, usingindex);

			sec.data_len = HEAD_LEN + data_frame_fix_len() + len;

			sec.frame_index = usingindex;
		}
		return  WIFI_PRO_STATUS::WIFI_PRO_END;
	}

	virtual void DESTORY_FIRST(WIFI_INFO & info) final
	{
		delete this;
	}

	virtual void DESTORY_WRITE(WIFI_INFO & info) final
	{
		delete this;
	}
private:
	int usingindex = 0;
	int MaxIndex = 0;

	int msgid = 0;

	vector<uint8_t> dat;
};
#endif


