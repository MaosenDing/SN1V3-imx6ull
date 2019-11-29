
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
#include <JDcomhead.h>
#include <stdio.h>
#include <thread>
#include <string.h>
#include <string>
#include <fstream>
#include <regex>
#include <sys/wait.h>
#include "errHandle/errHandle.h"
#include "mem_share.h"
#include "versions.h"
#include "jd_share.h"

#include <thread>
using namespace std;

int master_svc_thread(JD_INFO * pjif)
{
	while (true) {
		sleep(1);

		JD_FRAME jfr;
		
		unsigned char testbuff[] = "send test";

		jfr.jd_send_buff = &testbuff;
		jfr.jd_data_len = sizeof(testbuff);
		jfr.jd_command = 0x34;

		JD_send(*pjif, jfr);
	}
}




int register_master_svc(JD_INFO& jif)
{
	thread p(master_svc_thread, &jif);
	p.detach();
	return 0;
}












