#ifndef __wifi_ctrl_h____
#define __wifi_ctrl_h____

#include "SN1V2_rtConfig.h"
#include <vector>
#include <list>
#include <memory>
#include <mutex>
#include <sys/time.h>
#include <vector>
#include <map>
#include <stdio.h>
#include <condition_variable>
#include <sys/time.h>
#include "sn1v3cfg.h"
enum WIFI_ERR {
	wifi_ok = 0,
	wifi_disconnect = -1,
	wifi_send_error = -2,
	wifi_read_error = -3,
	wifi_pack_error = -4,
};


enum CODE_ID {
	CODE_CTRL = 0,
	CODE_INIT = 1,
	CODE_READ_NUM = 2,
	CODE_READ = 3,
	CODE_WRITE = 4,
	CODE_SELF_DOWNLOAD = 5,
#if 0
	CODE_INIT_MASK = 1 << (CODE_INIT + 1),
	CODE_READ_NUM_MASK = 1 << (CODE_READ_NUM + 1),
	CODE_READ_MASK = 1 << (CODE_READ + 1),
	CODE_WRITE_MASK = 1 << (CODE_WRITE + 1),
	CODE_SELF_WRITE_MASK = 1 << (CODE_SELF_WRITE + 1),
#endif
	CODE_ERR = -1,
};

enum WIFI_PRO_STATUS {
	WIFI_PRO_ERR = -1,
	//处理结束
	WIFI_PRO_END = 0,
	//继续处理
	WIFI_PRO_NEED_WRITE = 1,
	//保留到下一轮继续处理
	WIFI_PRO_NEXT_SHIFT_TO_NEXT_ROUND = 2,
};

enum
{
	MIN_PACK_SZ = 18,//最小包长度
	MAX_PACK_SZ = 1024,//最大包长度
};

struct WIFI_BASE_SESSION {
	timeval tv;
	int code_num = CODE_ERR;//命令编码 3 4 5
	int seq_num = 0;//帧顺序码 防止重复
	int16_t frame_index;//帧编号

	int pack_len = 0;
	int data_len = 0;
	unsigned char data[1024];
};

struct WIFI_DATA_SUB_PROTOCOL {
	uint16_t message_id = 0;
	unsigned char function_id = 0;
	unsigned char *function_data;
	int datalen = 0;
};



enum wifi_run_flg {
	wifi_run_null = 0,
	wifi_run_stop = -1,
	wifi_run_try_stop = -2,
	wifi_runing = 1,
};

struct WIFI_INFO;

struct WIFI_BASE_FUNCTION {
	WIFI_BASE_FUNCTION(WIFI_INFO &INinfo) :info(INinfo) {}
	virtual WIFI_PRO_STATUS wifi_read(WIFI_BASE_SESSION & sec) = 0;
	virtual WIFI_PRO_STATUS wifi_write(WIFI_BASE_SESSION & sec) = 0;

	virtual const char * FUNCTION_NAME() = 0;

	virtual ~WIFI_BASE_FUNCTION() {}
	enum {
		MASK_READ_NUM = 1 << 2,
		MASK_READ = 1 << 3,
		MASK_SELF_UPLOAD = 1 << 4,
		MASK_SELF_DOWNLOAD = 1 << 5,
	};


	int GetFunctionID() { return functionID; }
	int GetProMask() { return PRO_MASK; }

	void ADD_FUN(WIFI_BASE_FUNCTION *);

	static uint16_t static_msg_id;
protected:
	void SetProMask(uint32_t inmask) { PRO_MASK = inmask; }
	int functionID = -1;
	WIFI_INFO &info;
private:
	uint32_t PRO_MASK = 0;

};

struct WIFI_INFO {
	WIFI_INFO()
	{}
	int uartFD = -1;
	wifi_run_flg recRunFlg = wifi_run_null;
	//wifi mcu 通讯超时
	const int max_delay_ms_muc_response = 100;
	//数据传输服务超时
	const int max_delay_ms_session_response = 20 * 1000;
	//连接服务器超时
	const int max_delay_ms_connecting = 60 * 1000;
	//服务器ID 固定
	const unsigned char server_id[4] = { 0x62,0x27,0x21,0x55 };

	SN1_SHM * psn1;

	WIFI_BASE_FUNCTION** svcGrp = nullptr;
	int svcCnt = 0;

	std::list<std::shared_ptr <WIFI_BASE_SESSION> > rec_session_list;
	std::timed_mutex mtx_using_list;
	std::condition_variable_any enable_cv;


	std::list<WIFI_BASE_FUNCTION *> write_fun_list;
	std::mutex mtx_write_fun_list;

	//using in transmit
	std::mutex send_mtx;
	unsigned char sndbuf[1024];
	unsigned char send_seq = 0;
	//using retry

	//sn1v3 cfg file
	Tg_table cfg;
#define MAX_RETRY_EXEC_CTRL (3)
	//debug using
	int fake_check_flag = 0;
	int dbg_pri_chk_flag = 0;
	int dbg_pri_rd_len = 0;
	int dbg_pri_rd_word = 0;
	int dbg_pri_snd = 0;
	int dbg_pri_useful = 0;
	int dbg_pri_wifi_ctrl = 0;//wifi 控制事务
	int dbg_pri_wifi_data = 0;//wifi 数据事务

	int dbg_pri_msg = 0;//上层事务
	void delete_svc();

	~WIFI_INFO()
	{
		delete_svc();
	}
};








//开、关接收服务
void init_rec_pro(WIFI_INFO * pwifi);
int close_rec_pro(WIFI_INFO * pwifi);


int wifi_serivce(WIFI_INFO & wifi);
void InitWIFI_svc(WIFI_INFO & wifi);
WIFI_BASE_FUNCTION * FindFunction(WIFI_INFO & wifi, int funMask, int funid);



std::shared_ptr<WIFI_BASE_SESSION> wait_rec_session(WIFI_INFO & wifi
	, bool(*ChkSession)(WIFI_BASE_SESSION &),int milliseconds);

std::shared_ptr<WIFI_BASE_SESSION> wait_rec_session(WIFI_INFO & wifi
	, bool(*ChkSession)(WIFI_BASE_SESSION &, void * p), void * priv, int milliseconds);

int set_wifi_module(WIFI_INFO & wifi);
int get_wifi_tim(WIFI_INFO & wifi);
int get_cache(WIFI_INFO & wifi, int * buffsta);
int set_connect(WIFI_INFO & wifi);
int set_disconnect(WIFI_INFO & wifi);
//重置wifi buffer
int wifi_reset_buff_status(WIFI_INFO & wifi);


int mk_WIFI_DATA_SUB_PROTOCOL(WIFI_BASE_SESSION & sesssion, WIFI_DATA_SUB_PROTOCOL & pro);

#endif
