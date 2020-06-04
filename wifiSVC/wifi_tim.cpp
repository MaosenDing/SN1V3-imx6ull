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
#include <memory>
#include <vector>
#include "wifi_snd.h"
#include <string.h>
#include <chrono>
using namespace std;


shared_ptr<WIFI_BASE_SESSION> exec_wifi_tim(WIFI_INFO & wifi)
{
	WIFI_BASE_SESSION sec;

	sec.code_num = CODE_INIT;

	sec.frame_index = 0;
	sec.seq_num = 0;

	for (int i = 0; i < MAX_RETRY_EXEC_CTRL; i++) {
		transmit_session(wifi, sec);
		auto ret = wait_rec_session(wifi, [](WIFI_BASE_SESSION & session) -> bool {return (session.data[0] | 0x80) && session.code_num == (CODE_INIT | 0x80); }, wifi.max_delay_ms_ctrl);
		//if (ret && (ret->data[0] == (code | 0x80))) {
		//	return ret;
		//}
	}
	//返回失败
	return shared_ptr<WIFI_BASE_SESSION>();
}


int get_wifi_tim(WIFI_INFO & wifi)
{


	return 0;
}




