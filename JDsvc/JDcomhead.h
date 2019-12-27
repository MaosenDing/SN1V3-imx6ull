#ifndef __JDCOMhead__h____
#define __JDCOMhead__h____


#include <mutex>
#include <sys/time.h>
#include <vector>
#include <map>
#include <stdio.h>
#include "SN1V2_rtConfig.h"
#include <condition_variable>
#include <sys/time.h>
#define JD_OK 0
#define JD_ERROR_HEAD (-1)
#define JD_LACK_HEAD (-2)
#define JD_LACK_TAIL (-3)
#define JD_ERROR_LEN (-4)
#define JD_ERROR_CRC (-5)
#define JD_UNKNOWN_COMMAND (-6)
#define JD_TIME_OUT (-7)
#define JD_CONTINUE (-8)
//every 0xff 0xff is a legal crc
//just for test
#define JD_CRC_FACK_TEST (!JD_OK)
//#define JD_COM_DISP 1

//force to close this message
#define JD_CLOSE_FRAME (-10)

//...
#define JD_UNKNOWN (-20)
#define JD_TEST__ (-21)



#define JD_COMMAND_STAT (1)
#define JD_COMMAND_GET_CRE (2)

typedef union
{
	struct
	{
		unsigned char low_byte;
		unsigned char mlow_byte;
		unsigned char mhigh_byte;
		unsigned char high_byte;
	}byte_value;
	struct
	{
		unsigned short low_byte;
		unsigned short high_byte;
	}short_value;
	unsigned int  value;
}JD_ADDR;


struct JD_FRAME
{
	JD_ADDR jd_aim;
	unsigned int seq;
	unsigned int jd_command;
	unsigned char * jd_frame_head;
	union
	{
		unsigned char * jd_data_buff;
		void * jd_send_buff;
	};	
	unsigned int jd_data_len;
};

//method to process jd command

enum JDTIME {
	JD_TIME_OK = 0,
	JD_TIME_UNSET = -1,
	JD_TIME_LOST_SYNC =-2,
};

//is fake check is jd_ok
//any 0xff 0xff is consider as correct crc


struct CTRL_BASE{
	int Max_retry = 3;
	int retry_num = 0;
	int cpl_flag = 1;
};

struct Man_CTRL :public CTRL_BASE{
	float manual_deg;
	void trig_set(float infloat)
	{
		manual_deg = infloat;
		retry_num = 0;
		cpl_flag = 0;
	}
};

struct STOP_CTRL :public CTRL_BASE {
	void trig_set()
	{
		retry_num = 0;
		cpl_flag = 0;
	}
};

enum {
	diff_init0 = 1 << 0,
	diff_init1 = 1 << 1,

	diff_max0 = 1 << 2,
	diff_max1 = 1 << 3,

	diff_phase0 = 1 << 4,
	diff_phase1 = 1 << 5,

	diff_period0 = 1 << 6,
	diff_period1 = 1 << 7,

	diff_currect0 = 1 << 8,
	diff_currect1 = 1 << 9,

	diff_ratio0 = 1 << 10,
	diff_ratio1 = 1 << 11,
};

struct Par_CTRL:public CTRL_BASE
{
	int initSpeed;
	int MaxSpeed;
	int Phase;
	int period;
	int current;
	int Ratio;

	int setflg;
	void trig_set_init(int inspeed)
	{
		setflg |= diff_init0;
		initSpeed = inspeed;
		retry_num = 0;
		cpl_flag = 0;
	}
	void trig_set_max(int inMax)
	{
		setflg |= diff_max0;
		MaxSpeed = inMax;
		retry_num = 0;
		cpl_flag = 0;
	}
	void trig_set_pha(int inPha)
	{
		setflg |= diff_phase0;
		Phase = inPha;
		retry_num = 0;
		cpl_flag = 0;
	}
	void trig_set_per(int inPer)
	{
		setflg |= diff_period0;
		period = inPer;
		retry_num = 0;
		cpl_flag = 0;
	}
	void trig_set_cur(int incur)
	{
		setflg |= diff_currect0;
		current = incur;
		retry_num = 0;
		cpl_flag = 0;
	}
	void trig_set_rat(int inRat)
	{
		setflg |= diff_ratio0;
		Ratio = inRat;
		retry_num = 0;
		cpl_flag = 0;
	}
};

struct Par_GET :public Par_CTRL
{
	int succ_flag = 0;

	void trig_get()
	{
		succ_flag = 0;
		retry_num = 0;
		cpl_flag = 0;
	}
};



struct MDC_STA :public CTRL_BASE {
	timeval last_tv;
	

	float deg;
	int temperature;
	int current;
	char status[4];

	int statusint;
	char runningflg = 0;

	int lost_time = 0;
	bool is_lost()
	{
		return lost_time > 6;
	}

	void trig_set_init()
	{
		Max_retry = 2;
		retry_num = 0;
		cpl_flag = 0;
	}
};




struct MDC_CTRL
{
	int addr;
	Man_CTRL manual;
	Man_CTRL correct;
	Par_CTRL par;
	Par_GET parget;
	MDC_STA sta;
	STOP_CTRL stop;

	void disconnect_pro()
	{
		parget.trig_get();
	}
};



enum _JD_MOD {
	mdc_mode_table = 0,
	mdc_mode_manual = 1,
	mdc_mode_off = 2,
};

struct JD_INFO 
{	
//base jd info
	JD_INFO();
	int fd;
	typedef int(*JD_PRO) (JD_INFO & jif, JD_FRAME & jfr);
	std::map<int, JD_PRO> jd_pro_method;
	std::mutex pro_mtd_mtx;//lock for jd_pro_method

	std::timed_mutex enable_mtx;
	std::condition_variable_any enable_cv;
	
	int wait_time_out_ms = 20;

	JDTIME timesetFlag;	
	JD_PRO default_err_cmd;
	int fake_check_flag;

	_JD_MOD JD_MOD;


	MDC_CTRL mdcCtrl[2];

//for debug
	FILE * dbg_fp = stdout;
	char dbg_pri_rd_len = 0;
	char dbg_pri_rd_word = 0;
	char dbg_pri_snd_len = 0;
	char dbg_pri_snd_word = 0;
	char dbg_pri_chk_flag = 0;
	char dbg_pri_error_cmd = 1;
//0 null,1for right,2 for fake
};

struct JD_INFO_TIM : public JD_INFO {
	SN1_SHM * psn1;
	char dbg_shm_ret_printf = 0;
	char dbg_tim_rec_printf = 0;
	char dbg_cre_printf = 0;
	char dbg_file_printf = 1;
	int rate = 19200;
	//auto return fixed error num
	int error_debug_flag = 0;
	//enable time diff && time set
	int time_diff_max = 20;
};






struct SCANF_DATA {
	_JD_MOD JD_MOD;

	int manual_flag = 0;
	float manual_deg[2];

	int correct_flag = 0;
	float correct[2];

	int set_flg = 0;
	int initSpeed[2];
	int MaxSpeed[2];
	int Phase[2];
	int period[2];
	int currect[2];
	int Ratio[2];

	int get_flg = 0;
};

SCANF_DATA real_scan_file(const char * fil);


struct JDPROSTRUCT
{
	unsigned int command;
	JD_INFO::JD_PRO pro;
};

//send jd data
int JD_send(JD_INFO & jif, JD_FRAME & jfr);

void disp_x_buff(FILE * fp,unsigned char * buff, int num);

int JD_run_poll(JD_INFO &, int timeoutMS);

//register or unregister method for jd callback
void JD_pro_ctl(JD_INFO & jif, int cmd, JD_INFO::JD_PRO profun, int ctl);

int JD_Name_transfer(const unsigned char * input, char * outbuff,int InMaxSz);

int UARTX_Init(char *, int baudrate, int fctl, int databit, int stopbit, int parity);

int regist_timer_auto_flush(SN1_SHM * psn1);

#define MDC_MODE_FILE ("/tmp/mdc_mode")

#define MDC_STATUS_FILE ("/tmp/mdc_status")

#endif
