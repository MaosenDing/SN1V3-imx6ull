#ifndef __MDC_CTRL__h____
#define __MDC_CTRL__h____


#include "JDcomhead.h"
#include "SN1V2_rtConfig.h"


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


struct CLEAN_ALARM :public CTRL_BASE {
	void trig()
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
	
	int dir;
	float last_deg;
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
	CLEAN_ALARM alarm;

	void disconnect_pro()
	{
		parget.trig_get();
	}
};


//mdc ?????????????????????????????????????????????????????????
enum MDC_WORK_MOD {
	mdc_mode_table = 0,
	mdc_mode_manual = 1,
	mdc_mode_off = 2,
};
//mdc ????????????????????????????????????sn2????????????
enum MDC_RUNNING_MOD {
	mdc_run_mod_normal = 0,
	mdc_run_mod_sn1 = 1,
};

typedef struct {
	int mod;
	float manual0;
	float manual1;
	float correct0;
	float correct1;
	int	initspeed0;
	int	initspeed1;
	int	maxspeed0;
	int	maxspeed1;
	int	phase0;
	int	phase1;
	int	period0;
	int	period1;
	int	current0;
	int	current1;
	int	ratio0;
	int	ratio1;
	int	getpar0;
	int	getpar1;
	int	cleanalarm0;
	int	cleanalarm1;
}SCANF_DATA;


struct MDC_INFO :public JD_INFO_TIM {
	MDC_INFO()
	{
		mdcCtrl[0].addr = 0xaaaa << 8;
		mdcCtrl[1].addr = 0xbbbb << 8;

		mdcCtrl[0].parget.cpl_flag = 0;
		mdcCtrl[1].parget.cpl_flag = 0;
	}
	SN1_SHM * psn1;
	MDC_WORK_MOD work_mod;
	MDC_CTRL mdcCtrl[2];

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





SCANF_DATA real_scan_file(const char * fil);

int regist_timer_auto_flush(SN1_SHM * psn1);

#define MDC_MODE_FILE ("/tmp/mdc_mode")
#define MDC_STATUS_FILE ("/tmp/mdc_status")

#endif