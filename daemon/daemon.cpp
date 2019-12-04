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

enum sn_sta {
	sn_stop = 0,
	sn_1 = 1,
	sn_2 = 2,
	sn_3 = 3,
};



static sn_sta checkSN_sta(string & st)
{
	struct _GRP {
		sn_sta sta;
		const char * str;
	};

	_GRP ppp[] = {
		{sn_stop,"stop"},
		{sn_1,"sn1"},
		{sn_2,"sn2"},
		{sn_3,"sn3"},
	};

	if (st.size() > 0) {
		for (auto one : ppp) {
			char * findst = strstr((char *)st.c_str(), one.str);

			if (findst) {
				return one.sta;
			}
		}
	}
	return sn_1;
}





struct GUARD_SERVICE {
	GUARD_SERVICE(int & inpid) :shared_pid(inpid) {}
	int & shared_pid;
	virtual int init_pid(int argc, char * argv[]) = 0;
	virtual bool need_boot() = 0;
	virtual const char * get_name() = 0;
};

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

enum file_sta{
	fil_ok = 0,//ok
	fil_no_file,//无配置文件
	fil_reset,//启用reset字段
	fil_no_set,//没有set信息
	fil_day_error,//时间不达标
};


static file_sta checkCont(string & st)
{
	char * buff = (char *)st.c_str();

	char * findRESET = strstr(buff, "reset");

	if (findRESET) {
		return fil_reset;
	}

	char * findSETTIME = strstr(buff, "set@");

	if (findSETTIME) {
		int year, mon, day;
		if (3 == sscanf(buff, "set@%d-%d-%d", &year, &mon, &day)) {
			tm thisday;
			time_t thistim = time(nullptr);
			localtime_r(&thistim, &thisday);
			//fix tim
			thisday.tm_year += 1900;
			thisday.tm_mon += 1;

			if (thisday.tm_year < year) {
				return fil_day_error;
			}

			if (thisday.tm_mon < mon) {
				return fil_day_error;
			}

			if (thisday.tm_mday <= day) {
				return fil_day_error;
			}
			return fil_ok;
		}
	}
	return fil_no_set;
}


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

		int pid = fork();
		if (pid == 0) {
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
		} else if (pid > 0) {
			return pid;
		}
		SN1V2_ERR_LOG("fork error ###################2\n");
		return -1;
	}
	virtual bool need_boot()final
	{
		auto fil = GetFile("/tmp/snsta");
		if (sn_1 != checkSN_sta(*fil))
		{
			return false;
		}
		return true;
	}
};

#define MAX_FAIL_TIME 100
static int error_time = 0;
static void chk_reboot()
{
	if (error_time++ > MAX_FAIL_TIME) {
		SN1V2_WARN_LOG("over max error time,reboot\n");
		system("reboot");
	}
}


int main(int argc, char *argv[])
{
	logInit("daemon", "./daemon", google::GLOG_WARNING);

	prctl(PR_SET_NAME, "daemon svc");

	key_t key = getKey(SHARE_KEY_PATH, SHARE_KEY_INT);

	SN1_SHM * psn1 = (SN1_SHM *)getSHM(key, sizeof(SN1_SHM));

	checkClear(argc, argv, psn1);

	svc_mdc svc_mdc0(*psn1);
	svc_cap svc_cap0(*psn1);

	GUARD_SERVICE *group[] = {
		&svc_mdc0 ,
		&svc_cap0 ,
	};

	for (GUARD_SERVICE * p : group) {
		int pid = p->init_pid(argc, argv);
		p->shared_pid = pid;
		SN1V2_INF_LOG("%s,pid = %d\n", p->get_name(), p->shared_pid);
	}

	while (true) {
		for (GUARD_SERVICE * p : group) {
			sleep(1);

			bool bootFLG = false;
			//check dead
			if (p->shared_pid) {
				int status = 0;
				int retpid = waitpid(p->shared_pid, &status, WNOHANG);

				if (retpid == 0) {
					continue;
				}
				//check program fail count
				chk_reboot();
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

	if ((int)p != -1) {
		return p;
	}
	return nullptr;
}




