#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include <stdio.h>
#include <algorithm>
#include <string.h>

#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "SN1V2_rtConfig.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/prctl.h>
#include <SN1V2_rtConfig.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include "errHandle/errHandle.h"
#include <fstream>
#include <sys/stat.h>
#include <memory>

#include "sn1v3cfg.h"
#include "tableWork.h"
#include "configOperator.h"

using namespace std;


void rebootService(int hour, int minute);



key_t getKey(const char *path, int num);
void * getSHM(key_t key, int sz);
int wait_for_mdc(int timeout);



bool NeedCapWork()
{
	key_t key = getKey(SHARE_KEY_PATH, SHARE_KEY_INT);

	SN1_SHM * psn1 = (SN1_SHM *)getSHM(key, sizeof(SN1_SHM));

	time_t now = time(0);
	tm t2;

	localtime_r(&now, &t2);

	const int year = t2.tm_year + 1900;
	const int mon = t2.tm_mon + 1;
	const int day = t2.tm_mday;
#if 0
	SN1V2_INF_LOG("sh tim : %d,%d,%d\n", psn1->year, psn1->mon, psn1->day);
	SN1V2_INF_LOG("local tim : %d,%d,%d\n", year, mon, day);
#endif

	if (year != psn1->year)
		return true;

	if (mon != psn1->mon)
		return true;

	if (day != psn1->day)
		return true;

	return false;
}

static bool getCMDClear(int argc, char * argv[])
{
	for (int i = 0; i < argc; i++) {
		if (!strcmp("-c", argv[i])) {
			return true;
		}
	}
	return false;
}

static void checkClear(int argc, char *argv[], SN1_SHM * psn1)
{
	if (getCMDClear(argc, argv) == true) {

		if (psn1->pid_aim) {
			kill(psn1->pid_aim, SIGINT);
		}
		if (psn1->pid_mdc) {
			kill(psn1->pid_aim, SIGINT);
		}
		memset(psn1, 0, sizeof(SN1_SHM));

		SN1V2_INF_LOG("set share memory clear \n");
	}
}


struct GUARD_SERVICE {
	GUARD_SERVICE(int & inpid) :shared_pid(inpid) {}
	int & shared_pid;
	virtual int init_pid(int argc, char * argv[]) = 0;
	virtual bool need_boot() = 0;
	virtual const char * get_name() = 0;
};

#if 0
struct svc_wifi : public GUARD_SERVICE {
	svc_wifi(SN1_SHM & sn1) :GUARD_SERVICE(sn1.pid_mdc) {}

	virtual const char * get_name() final
	{
		return "wifi service";
	}

	virtual int init_pid(int argc, char * argv[]) final
	{
		if (shared_pid > 0) {
			SN1V2_WARN_LOG("mdc ,kill pid =%d\n", shared_pid);
			kill(shared_pid, SIGINT);
			shared_pid = 0;
		}

		int pid = fork();
		if (pid == 0) {
			system("/root/io.sh jd3 1");
			prctl(PR_SET_NAME, "init:wifi");
			int ret = execv("./wifictrl.exe", argv);
			SN1V2_ERR_LOG("wifi finished itself,ret = %d\n", ret);
			SN1V2_ERR_LOG("errno = %d\n", errno);
			exit(0);
		} else if (pid > 0) {
			return pid;
		}
		SN1V2_ERR_LOG("fork error ###################1\n");
		return -1;
	}

	int last_connectted_min;

	virtual bool need_boot()final
	{
		//????????????
		time_t now = time(0);
		tm t2;

		localtime_r(&now, &t2);

		const int year = t2.tm_year + 1900;
		//const int mon = t2.tm_mon + 1;
		//const int day = t2.tm_mday;

		if (year < 2015) {
			//??????????????????  ???????????????
			return true;
		}
		Tg_table tg_table;
		scanfAllTable(tg_table, Mask_T4);
		//???????????????
		if (0 == tg_table.T4.CM_Cycle) {
			return true;
		}
		//???????????????
		system("/root/io.sh jd3 0");
		int thismin = t2.tm_min + t2.tm_hour * 60;

		int lastblock = last_connectted_min / tg_table.T4.CM_Cycle;
		int thisblock = thismin / tg_table.T4.CM_Cycle;

		//???????????????
		if (lastblock != thisblock) {
			//??????????????????
			int aimsecond = thisblock * tg_table.T4.CM_Cycle * 60
				+ tg_table.T4.CM_Group * 60 + tg_table.T4.CM_Times;
			//printf("next tim = %d;%d;%d\n", aimsecond / 3600, aimsecond / 60 % 60, aimsecond % 60);
			//????????????
			int nowsecond = thismin * 60 + t2.tm_sec;
			if (nowsecond > aimsecond) {
				last_connectted_min = thismin;
				return true;
			}
		}
		return false;
	}
};
#endif


struct svc_mdc : public GUARD_SERVICE {
	svc_mdc(SN1_SHM & sn1) :GUARD_SERVICE(sn1.pid_mdc) {}

	virtual const char * get_name() final
	{
		return "mdc service";
	}

	virtual int init_pid(int argc, char * argv[]) final
	{
		if (shared_pid > 0) {
			SN1V2_WARN_LOG("mdc ,kill pid =%d\n", shared_pid);
			kill(shared_pid, SIGINT);
			shared_pid = 0;
		}

		int pid = fork();
		if (pid == 0) {
			prctl(PR_SET_NAME, "init:mdc");
			int ret = execv("./mdc.exe", argv);
			SN1V2_ERR_LOG("mdc finished itself,ret = %d\n", ret);
			SN1V2_ERR_LOG("errno = %d\n", errno);
			exit(0);
		} else if (pid > 0) {
			return pid;
		}
		SN1V2_ERR_LOG("fork error ###################1\n");
		return -1;
	}
	virtual bool need_boot()final
	{
		return true;
	}
};


struct svc_cap : public GUARD_SERVICE {
	svc_cap(SN1_SHM & sn1) :GUARD_SERVICE(sn1.pid_aim) {}



	virtual const char * get_name() final
	{
		return "cap svc";
	}

	virtual int init_pid(int argc, char * argv[]) final
	{
		if (shared_pid) {
			SN1V2_WARN_LOG("cap ,kill pid =%d\n", shared_pid);
			kill(shared_pid, SIGINT);
			shared_pid = 0;
		}
		#if 0
			pid = fork();
			pid == 0) {
			prctl(PR_SET_NAME, "init:cap");
			while (true) {
				if (0 > wait_for_mdc(1000)) {
					SN1V2_WARN_LOG("reboot########################\n");
					system("reboot");
					exit(EXIT_FAILURE);
				}
				if (true == NeedCapWork()) {

					const char * daycfgFile = "day.cfg";

					auto fil = GetFile(daycfgFile);
					checkCont(*fil);

					execl("./aim.exe", "./aim.exe", "tableGen2", "SCG.txt", 0);
				}
			}
			exit(0);
			se if (pid > 0) {
			return pid;
			
		#else
			int pid = fork();
			if (pid == 0) {
				prctl(PR_SET_NAME, "init:cap");
				int ret = execl("./aim.exe", "./aim.exe", "tableGen2", "SCG.txt", 0);
				SN1V2_ERR_LOG("mdc finished itself,ret = %d\n", ret);
				SN1V2_ERR_LOG("errno = %d\n", errno);
				exit(0);
			} else if (pid > 0) {
				return pid;
			}
		#endif 

		SN1V2_ERR_LOG("fork error ###################2\n");
		return -1;
	}
	virtual bool need_boot()final
	{
		#if 0
		auto fil = GetFile("/tmp/snsta");
		if (sn_1 != checkSN_sta(*fil))
		{
			return false;
		}
		#endif
		
		return true;
	}
};







#if 0
enum file_sta{
	fil_ok = 0,//ok
	fil_no_file,//???????????????
	fil_reset,//??????reset??????
	fil_no_set,//??????set??????
	fil_day_error,//???????????????
};

static shared_ptr<string> GetFile(const char * fileName)
{
	auto st = make_shared<string>();
	int ret;
	struct stat tmpStat;
	if (0 > (ret = stat(fileName, &tmpStat))) {
		return st;
	}

	ifstream ifs(fileName);

	st->resize(tmpStat.st_size);
	if (ifs.good()) {
		ifs.read((char *)st->c_str(), tmpStat.st_size);
	}
	return st;
}
#endif


int main(int argc, char *argv[])
{
	logInit("daemon", "./daemon", google::GLOG_WARNING);

	prctl(PR_SET_NAME, "daemon.exe");

	key_t key = getKey(SHARE_KEY_PATH, SHARE_KEY_INT);

	SN1_SHM * psn1 = (SN1_SHM *)getSHM(key, sizeof(SN1_SHM));

	checkClear(argc, argv, psn1);

	svc_mdc svc_mdc0(*psn1);
	svc_cap svc_cap0(*psn1);
	//svc_wifi svc_wifi0(*psn1);

	GUARD_SERVICE *group[] = {
		&svc_mdc0 ,
		&svc_cap0 ,
		//&svc_wifi0,
	};

	for (GUARD_SERVICE * p : group) {
		int pid = p->init_pid(argc, argv);
		p->shared_pid = pid;
		SN1V2_INF_LOG("%s,pid = %d\n", p->get_name(), p->shared_pid);
	}

	while (true) {
		for (GUARD_SERVICE * p : group) {
			sleep(3);

			bool bootFLG = false;
			//check dead
			if (p->shared_pid) {
				int status = 0;
				int retpid = waitpid(p->shared_pid, &status, WNOHANG);

				if (retpid == 0) {
					continue;
				}
				//check program fail count
				p->shared_pid = 0;
				SN1V2_WARN_LOG("svc = %s ,ret aim ret pid = %d,status = %#x\n"
					, p->get_name(), retpid, status);

				if (WIFEXITED(status)) {
					SN1V2_WARN_LOG("exited, status=%d\n", WEXITSTATUS(status));
				}
				if (WIFSIGNALED(status)) {
					SN1V2_WARN_LOG("killed by signal %d\n", WTERMSIG(status));
				}
				if (WIFSTOPPED(status)) {
					SN1V2_WARN_LOG("stopped by signal %d\n", WSTOPSIG(status));
				}
				if (WIFCONTINUED(status)) {
					SN1V2_WARN_LOG("continued\n");
				}
			}
			//check boot
			if (p->need_boot() == true) {
				bootFLG = true;
			}
			//boot
			if (bootFLG) {
				int newpid = p->init_pid(argc, argv);
				p->shared_pid = newpid;
			}
		}
	}
}


int wait_for_mdc(int timeout)
{
	key_t key = getKey(SHARE_KEY_PATH, SHARE_KEY_INT);

	SN1_SHM * psn1 = (SN1_SHM *)getSHM(key, sizeof(SN1_SHM));

	while (timeout--) {
		sleep(1);

		if (psn1->mdc_flag == SN1_SHM::MDC_TIME_OK) {
			return 0;
		}
	}
	return -1;
}


key_t getKey(const char *path, int num)
{
	key_t key;
	if (-1 == (key = ftok(path, num))) {
		return -1;
	}
	return key;
}

void * getSHM(key_t key, int sz)
{
	int sid;

	if (-1 == (sid = shmget(key, sz, IPC_CREAT | IPC_EXCL | 0666))) {
		if (errno == EEXIST) {
			if (0 == (sid = shmget(key, sz, 0666))) {
				goto succ;
			}
		}
		return nullptr;
	}
succ:
	void *p = shmat(sid, nullptr, 0);

	if ((long int)p != -1) {
		return p;
	}
	return nullptr;
}




