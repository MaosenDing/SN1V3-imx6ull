#include <iostream>
#include <fstream>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "main.h"
using namespace std;

#define BINSIZE(bin) _binary_##bin##_size
#define BINSTAET(bin) _binary_##bin##_start
#define BINEND(bin) _binary_##bin##_end

#define CLEAN_ALL_LOG 1
#define CLEAN_CRE 0


void writebin(const char * filename, const char buff[], int size)
{
	char tmpppp[64];
	sprintf(tmpppp, "rm -f %s", filename);
	printf("%s \n", tmpppp);
	sleep(1);
	ofstream outtest(filename);
	outtest.write(buff, size);
}

void copyOBJ(const char * start, const char * end, int len, const char * aimpath)
{
	fprintf(stdout, "start = %p,len =%d,end = %p ,file = %s\n"
		, start, len, end, aimpath);

	writebin(aimpath, start, len);

	fprintf(stdout, "bin write ok\n");

	chmod(aimpath, 0777);
}

#ifdef EXE
void cplibjd()
{
	extern char BINEND(libcomlib_so)[];
	extern char BINSIZE(libcomlib_so)[];
	extern char BINSTAET(libcomlib_so)[];
	copyOBJ(BINSTAET(libcomlib_so),
		BINEND(libcomlib_so),
		(int)BINSIZE(libcomlib_so),
		"/mnt/jaffs/lib/libcomlib.so"
	);
}
ADD_JD_SERVICE(cplibjd);

void cpmdc()
{
	extern char BINEND(mdc_exe)[];
	extern char BINSIZE(mdc_exe)[];
	extern char BINSTAET(mdc_exe)[];
	copyOBJ(BINSTAET(mdc_exe),
		BINEND(mdc_exe),
		(int)BINSIZE(mdc_exe),
		"/mnt/jaffs/user/mdc.exe"
	);
}
ADD_JD_SERVICE(cpmdc);

void cpdaemon()
{
	extern char BINEND(daemon_exe)[];
	extern char BINSIZE(daemon_exe)[];
	extern char BINSTAET(daemon_exe)[];
	copyOBJ(BINSTAET(daemon_exe),
		BINEND(daemon_exe),
		(int)BINSIZE(daemon_exe),
		"/mnt/jaffs/user/daemon.exe"
	);
}
ADD_JD_SERVICE(cpdaemon);


void cpaim()
{
	extern char BINEND(aim_exe)[];
	extern char BINSIZE(aim_exe)[];
	extern char BINSTAET(aim_exe)[];
	copyOBJ(BINSTAET(aim_exe),
		BINEND(aim_exe),
		(int)BINSIZE(aim_exe),
		"/mnt/jaffs/user/aim.exe"
	);
}
ADD_JD_SERVICE(cpaim);

void cpwifictrl()
{
	extern char BINEND(wifictrl_exe)[];
	extern char BINSIZE(wifictrl_exe)[];
	extern char BINSTAET(wifictrl_exe)[];
	copyOBJ(BINSTAET(wifictrl_exe),
		BINEND(wifictrl_exe),
		(int)BINSIZE(wifictrl_exe),
		"/mnt/jaffs/user/wifictrl.exe"
	);
}
ADD_JD_SERVICE(cpwifictrl);

#endif



#ifdef SCG
void cpscg()
{
	extern char BINEND(T1_txt)[];
	extern char BINSIZE(T1_txt)[];
	extern char BINSTAET(T1_txt)[];
	copyOBJ(BINSTAET(T1_txt),
		BINEND(T1_txt),
		(int)BINSIZE(T1_txt),
		"/mnt/jaffs/user/T1.txt"
	);

	extern char BINEND(T2_txt)[];
	extern char BINSIZE(T2_txt)[];
	extern char BINSTAET(T2_txt)[];
	copyOBJ(BINSTAET(T2_txt),
		BINEND(T2_txt),
		(int)BINSIZE(T2_txt),
		"/mnt/jaffs/user/T2.txt"
	);


	extern char BINEND(T3_txt)[];
	extern char BINSIZE(T3_txt)[];
	extern char BINSTAET(T3_txt)[];
	copyOBJ(BINSTAET(T3_txt),
		BINEND(T3_txt),
		(int)BINSIZE(T3_txt),
		"/mnt/jaffs/user/T3.txt"
	);


	extern char BINEND(T4_txt)[];
	extern char BINSIZE(T4_txt)[];
	extern char BINSTAET(T4_txt)[];
	copyOBJ(BINSTAET(T4_txt),
		BINEND(T4_txt),
		(int)BINSIZE(T4_txt),
		"/mnt/jaffs/user/T4.txt"
	);


	extern char BINEND(T6_txt)[];
	extern char BINSIZE(T6_txt)[];
	extern char BINSTAET(T6_txt)[];
	copyOBJ(BINSTAET(T6_txt),
		BINEND(T6_txt),
		(int)BINSIZE(T6_txt),
		"/mnt/jaffs/user/T6.txt"
	);
}
ADD_JD_SERVICE(cpscg);
#endif

#ifdef USR
void cpinit()
{
	extern char BINEND(user_sh)[];
	extern char BINSIZE(user_sh)[];
	extern char BINSTAET(user_sh)[];
	copyOBJ(BINSTAET(user_sh),
		BINEND(user_sh),
		(int)BINSIZE(user_sh),
		"/mnt/jaffs/user.sh"
	);

	system("chmod 777 /mnt/jaffs/user.sh");
	printf("chmod 777 /mnt/jaffs/user.sh");
}
#endif


int main()
{
	int pid = fork();
	if (pid < 0)
		printf("First fork failed!");
	if (pid > 0)
		exit(EXIT_SUCCESS);// father exit

	if (setsid() == -1)
		printf("setsid failed!");
	pid = fork();
	if (pid < 0)
		printf("Second fork failed!");

	if (pid > 0)// father exit
		exit(EXIT_SUCCESS);

	cout << "main ppp " << endl;
#if CLEAN_CRE > 0
	//clean pes.sn
	system("echo '' > /mnt/jaffs/user/PES.sn");
#endif

#if CLEAN_ALL_LOG > 0
	//clean all log
	system("rm /mnt/jaffs/user/aim -rf");
	system("rm /mnt/jaffs/user/daemon -rf");
	system("rm /mnt/jaffs/user/mdc -rf");
	system("rm /mnt/jaffs/user/wifi -rf");
	system("rm /mnt/jaffs/user/20* -rf\n");
	system("rm /mnt/jaffs/user/19* -rf\n");
#endif
	for (int i = 0; i < 3; i++) {
		system("killall daemon.exe");
		system("killall aim.exe");
		system("killall wifictrl.exe");
		system("killall mdc.exe");

		system("killall init:cap");
		system("killall init:mdc");
		system("killall init:wifi");
		system("killall init:mdc");
		sleep(1);
	}

	system("ps");

#ifdef EXE
	cplibjd();
	cpmdc();
	cpdaemon();
	cpaim();
	cpwifictrl();
#warning "using all .exe file"
#endif


#ifdef SCG
	cpscg();
#warning "using scg file"
#endif


#ifdef USR
	cpinit();
#warning "using user.sh file"
#endif
	fprintf(stdout, "update over\n");

	for (int i = 0; i < 4; i++) {
		printf("sync\n");
		sync();
	}
	system("/root/io jd2 0");
	system("/root/io jd3 0");

	printf("umount\n");
	system("umount /mnt/jaffs/");
	system("df -h");

	sync();
	system("reboot");

}
