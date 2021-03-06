#ifndef __JDCOMhead__h____
#define __JDCOMhead__h____





#include <mutex>
#include <sys/time.h>
#include <vector>
#include <map>
#include <stdio.h>
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

enum PROTOCOL_TYPE {
	protocol_JD_motor = 0,
	protocol_JD_WIFI = 1,
	protocol_JD_tmp_motor_version = 2,
};

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
	/*
	char dbg_shm_ret_printf = 0;
	char dbg_tim_rec_printf = 0;
	char dbg_cre_printf = 0;
	char dbg_file_printf = 1;
	int rate = 19200;
	//auto return fixed error num
	int error_debug_flag = 0;
	//enable time diff && time set
	int time_diff_max = 20;
	*/
};


struct JDPROSTRUCT
{
	unsigned int command;
	JD_INFO::JD_PRO pro;
};

//send jd data
int JD_send(JD_INFO & jif, JD_FRAME & jfr);

void disp_x_buff(FILE * fp,unsigned char * buff, int num);
void disp_x_buff(unsigned char * buff, int num);
//int JD_run_poll(JD_INFO& jif, int TimeOutMS, PROTOCOL_TYPE typ);
int JD_run_poll(JD_INFO& jif, int TimeOutMS);

//register or unregister method for jd callback
void JD_pro_ctl(JD_INFO & jif, int cmd, JD_INFO::JD_PRO profun, int ctl);

int JD_Name_transfer(const unsigned char * input, char * outbuff,int InMaxSz);

int UARTX_Init(char *, int baudrate, int fctl, int databit, int stopbit, int parity);




#endif
