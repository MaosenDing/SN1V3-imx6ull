#include "../wifi_svc.h"
#include "../wifi_ctrl.h"
#include <string.h>
#include "jd_share.h"
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace  std;
//单次接收发送模板

//重写read_pro_fun 接收处理函数 返回sta_need_write 启动 create_write_fun、
//重写mk_write_session_data,修改发送的数据
//重写virtual WIFI_BASE_FUNCTION * create_write_fun() = 0; 投递写入结构体
//重写virtual void destor_write_fun() = 0; 与上面对应 销毁写入结构体

struct WIFI_FUNCTION_DOWNLOAD_FILE :public WIFI_BASE_FUNCTION
{
	WIFI_FUNCTION_DOWNLOAD_FILE(WIFI_INFO & info) :WIFI_BASE_FUNCTION(info)
	{}

	int writebin(const char * path, const char * data, const int len)
	{
		int goodflg = 0;
		{
			ofstream writefile(path, ios::binary);
			if (writefile.good()) {
				writefile.write(pbuffer, len);
				goodflg = 1;
			}
		}
		sync();
		return goodflg;
	}

	virtual void contrl_read(WIFI_DATA_SUB_PROTOCOL & sub) = 0;
	virtual int first_data_frame(WIFI_BASE_SESSION & sec, uint16_t& outcrc, uint32_t &outlen) = 0;
	virtual int data_finish(char * data, int len) = 0;
	virtual int memcpy_write_fix_dat(unsigned char buff[],int maxlen) = 0;


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
					contrl_read(sub);
				}
			}
		}
		//数据信令
		if (PRO_MASK == WIFI_BASE_FUNCTION::MASK_SELF_DOWNLOAD) {
			if (sec.frame_index == 0) {
				uint16_t tmpcrc;
				uint32_t tmplen;
				if (0 == first_data_frame(sec, tmpcrc, tmplen)) {
					crc = tmpcrc;
					len = tmplen;

					usingindex++;
					if (pbuffer) {
						delete pbuffer;
					}
					pbuffer = new char[len];

					if (!pbuffer) {
						printf("error no enough memory\n");
						exit(-1);
					}
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
					data_finish(pbuffer, len);
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
		sec.data_len = 1;
		sec.data[0] = functionID;
		sec.frame_index = usingindex;

		int wrlen = memcpy_write_fix_dat(&sec.data[1], 128);

		sec.data_len += wrlen;
		if (info.dbg_pri_msg) {
			printf("try get frame index = %d\n"
				, usingindex
			);
		}
		return  WIFI_PRO_STATUS::WIFI_PRO_END;
	}

	virtual void destor_write_fun()
	{
		delete this;
	}

	virtual void DESTORY_FIRST(WIFI_INFO & info) final
	{
		delete this;
	}


	virtual void DESTORY_WRITE(WIFI_INFO & info) final
	{
		destor_write_fun();
	}
private:
	int fileindex = 0;
	uint16_t crc;
	uint32_t len;
	int usingindex = 0;

	char *pbuffer = 0;
};



