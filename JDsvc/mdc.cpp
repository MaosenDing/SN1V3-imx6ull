#include "SN1V2_rtConfig.h"
#include "JDcomhead.h"
#include <clockd_def.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/prctl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include "JDcomhead.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <fstream>
#include <regex>
#include <sys/wait.h>
#include "errHandle/errHandle.h"
#include "mem_share.h"
#include "versions.h"
#include "jd_share.h"
#include "iostream"

using namespace std;





static int mdc_uart_init(JD_INFO_TIM & jit, int argc, char ** argv)
{
	char * name = ChkCmdVal(argc, argv, "-s");
	cout << "name " << name << endl;
	if (!name) {
		name = (char *)"/dev/ttyS6";
	}

	int rate = jit.rate ? jit.rate : 115200;
	int fd = UARTX_Init(name, rate, 0, 8, 1, 0);
	return fd;
}

int JD_time_rec(JD_INFO & jif, JD_FRAME & jfr);






static void bus_handle(int signo)
{
	printf("%d , mdc error demon\n", signo);

	exit(-1);
}

static void jif_dbg_set(JD_INFO_TIM &jif)
{
	jif.dbg_pri_snd_len = 1;
	jif.dbg_pri_snd_word = 1;
	jif.dbg_pri_rd_len = 1;
	jif.dbg_pri_rd_word = 1;
	jif.dbg_shm_ret_printf = 1;

	jif.fake_check_flag = 1;
	jif.dbg_pri_chk_flag = 2;
	jif.dbg_pri_error_cmd = 1;
}

static void jif_dbgtim_set(JD_INFO_TIM &jif)
{
	jif.dbg_pri_snd_len = 0;
	jif.dbg_pri_snd_word = 0;
	jif.dbg_pri_rd_len = 0;
	jif.dbg_pri_rd_word = 0;
	jif.dbg_shm_ret_printf = 0;

	jif.fake_check_flag = 0;
	jif.dbg_pri_chk_flag = 2;
	jif.dbg_pri_error_cmd = 1;
	jif.dbg_tim_rec_printf = 1;
}


static void jif_normal_set(JD_INFO_TIM &jif)
{
	jif.dbg_pri_snd_len = 0;
	jif.dbg_pri_snd_word = 0;
	jif.dbg_pri_rd_len = 0;
	jif.dbg_pri_rd_word = 0;
	jif.dbg_shm_ret_printf = 0;

	jif.fake_check_flag = 0;
	jif.dbg_pri_chk_flag = 0;
	jif.dbg_pri_error_cmd = 0;
	jif.dbg_tim_rec_printf = 0;
}

static void set_bound_rate(JD_INFO_TIM &jif, int argc, char * argv[])
{
	char * rateVal = ChkCmdVal(argc, argv, "-r");

	int rate = 0;
	if (rateVal) {
		rate = atoi(rateVal);
	}

	if (rate) {
		jif.rate = rate;
	}
}

SN1_SHM * get_shared_cfg()
{
	key_t key = getKey(SHARE_KEY_PATH, SHARE_KEY_INT);

	SN1_SHM * psn1 = (SN1_SHM *)getSHM(key, sizeof(SN1_SHM));
	psn1->mdc_flag = SN1_SHM::MDC_TIME_FALSE;
	psn1->max_time_out_second = MDC_MAX_TIME_OUT_SECOND;
	return psn1;
}

int register_master_svc(JD_INFO& jif);
int init_mdc_monitor_Service(int argc, char * argv[])
{
	signal(SIGBUS, bus_handle);

	prctl(PR_SET_NAME, "mdc service");
	SN1_SHM *psn1 = get_shared_cfg();
	if (nullptr == psn1) {
		exit(EXIT_FAILURE);
	}

	JD_INFO_TIM jif;

	set_bound_rate(jif, argc, argv);

	int fd = mdc_uart_init(jif, argc, argv);
	if (fd < 0) {
		printf("mdc uart open failed\n");
		exit(EXIT_FAILURE);
	} else {
		printf("mdc init\n");
	}

	jif.fd = fd;
	jif.dbg_tim_rec_printf = 1;


	jif.timesetFlag = JD_TIME_UNSET;

	jif.fake_check_flag = JD_CRC_FACK_TEST;

	jif.psn1 = psn1;
	if (ChkifCMD(argc, argv, "-dbg")) {
		jif_dbg_set(jif);

	} else if (ChkifCMD(argc, argv, "-dbgtim")) {
		jif_dbgtim_set(jif);
	} else {
		jif_normal_set(jif);
	}
	register_master_svc(jif);
	//regist_timer_auto_flush(psn1);

	//mdc poll will never return
	int ret = JD_run_poll(jif, -1);
	switch (ret) {
	case JD_TIME_OUT:
		printf("mdc timeout\n");
		exit(0);
		break;
	case JD_CLOSE_FRAME:
		printf("mdc com ok\n");
		exit(0);
		break;
	default:
		break;
	}
	exit(0);
	printf("mdc com\n");

	return ret;
}


int main(int argc, char **argv)
{
	for (int i = 0; i < argc; i++) {
		if (!strcmp("-v", argv[i])) {
			printf("%s\n", getVersion());
			printf("compile data = %s,%s\n", __DATE__, __TIME__);
			return 0;
		}
	}
	logInit("mdc", "./mdc", google::GLOG_WARNING);
	return init_mdc_monitor_Service(argc, argv);
}















