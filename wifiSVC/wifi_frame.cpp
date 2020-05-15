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
using namespace std;




int wifi_open(WIFI_INFO & wifi)
{
	//启动接收服务
	init_rec_pro(&wifi);
	//上电

	//对时

	//其他
	
	return 0;
}


int wifi_close(WIFI_INFO & wifi)
{
	//
	close_rec_pro(&wifi);
	//关闭电源


	return 0;
}

shared_ptr < vector <unsigned int> > read_num(WIFI_INFO & wifi)
{
	auto ret = make_shared<vector <unsigned int>>();

	WIFI_BASE_SESSION sec;

	mk_read_num_session(wifi, sec);



	return ret;
}


int wifi_serivce(WIFI_INFO & wifi)
{

	wifi_open(wifi);



	return 0;
}
