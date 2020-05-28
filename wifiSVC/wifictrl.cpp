#include "SN1V2_rtConfig.h"
#include "wifi_ctrl.h"
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
#include <thread>
#include "errHandle/errHandle.h"
#include "mem_share.h"
#include "versions.h"
#include "jd_share.h"
#include "iostream"

using namespace std;

static int mdc_uart_init(int inrate, int argc, char ** argv)
{
	char * name = ChkCmdVal(argc, argv, "-s");
	cout << "name " << name << endl;
	if (!name) {
		name = (char *)"/dev/ttyS6";
	}

	int rate = inrate ? inrate : 230400;
	int fd = UARTX_Init(name, rate, 0, 8, 1, 0);
	return fd;
}


static void bus_handle(int signo)
{
	printf("%d , mdc error demon\n", signo);

	exit(-1);
}



static int set_bound_rate(int argc, char * argv[])
{
	char * rateVal = ChkCmdVal(argc, argv, "-r");

	int rate = 0;
	if (rateVal) {
		rate = atoi(rateVal);
	}

	if (rate) {
		return rate;
	}

	return 0;
}

SN1_SHM * get_shared_cfg()
{
	key_t key = getKey(SHARE_KEY_PATH, SHARE_KEY_INT);

	SN1_SHM * psn1 = (SN1_SHM *)getSHM(key, sizeof(SN1_SHM));
	psn1->mdc_flag = SN1_SHM::MDC_TIME_FALSE;
	psn1->max_time_out_second = MDC_MAX_TIME_OUT_SECOND;
	return psn1;
}

void setDbg(WIFI_INFO & wifi)
{
	wifi.dbg_pri_snd = 1;
	wifi.dbg_pri_rd_len = 1;
	wifi.dbg_pri_rd_word = 1;
}



int init_mdc_monitor_Service(int argc, char * argv[])
{
	signal(SIGBUS, bus_handle);

	prctl(PR_SET_NAME, "wifi service");
	SN1_SHM *psn1 = get_shared_cfg();
	if (nullptr == psn1) {
		exit(EXIT_FAILURE);
	}

	WIFI_INFO wifi;
	wifi.psn1 = psn1;

	int boundrate = set_bound_rate(argc, argv);

	int fd = mdc_uart_init(boundrate, argc, argv);
	if (fd < 0) {
		printf("wifi uart open failed\n");
		exit(EXIT_FAILURE);
	} else {
		wifi.uartFD = fd;
		printf("wifi init\n");
	}

	if (ChkifCMD(argc, argv, "-dbg"))
		setDbg(wifi);

	if (ChkifCMD(argc, argv, "-dwr")) {
		wifi.dbg_pri_snd = 1;
	}		

	if (ChkifCMD(argc, argv, "-drd")) {
		wifi.dbg_pri_rd_len = 1;
		wifi.dbg_pri_rd_word = 1;
	}

	if (ChkifCMD(argc, argv, "-duse")) {
		wifi.dbg_pri_useful = 1;
	}

	if (ChkifCMD(argc, argv, "-dctrl")) {
		wifi.dbg_pri_wifi_ctrl = 1;
	}

	//mdc poll will never return
	InitWIFI_svc(wifi);
	wifi_serivce(wifi);


	exit(0);
	printf("mdc com\n");

	return 0;
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















