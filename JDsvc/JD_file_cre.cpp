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
#include <sys/inotify.h>
#include <sys/poll.h>

#include "mdc_ctrl.h"

using namespace std;


#if USING_FAKE_CRE
static double gotRand1()
{
	static bool srandinitFlag = false;

	if (srandinitFlag == false) {
		srandinitFlag = true;
		srand(time(0));
	}
	return (double)(rand() / (double)RAND_MAX);
}

static double gotRandFloat(double fmin, double fmax)
{
	double diff = fmax - fmin;

	return fmin + diff * gotRand1();
}

static int mk_fake_cre_message(char * buff)
{
	time_t now = time(0);
	tm nowtm;
	localtime_r(&now, &nowtm);

	int len = sprintf(buff, "%04d-%02d-%02d", nowtm.tm_year + 1900, nowtm.tm_mon + 1, nowtm.tm_mday);

	for (int i = 0; i < 8; i++) {
		len += sprintf(buff + len, ",%5f", (float)gotRandFloat(-5, 5));
	}

	len += sprintf(buff + len, ",SN1_300,2");

	return len;
}
#else
#include "time_interval.h"
static shared_ptr<string > GetFirstCreData(const char * creName)
{
	auto ret = make_shared<string >();

	std::ifstream t(creName);

	if (t.good()) {
		std::string loadbin((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());

		regex reg("\\d{4}-\\d{1,2}-\\d{1,2}((?:,-?\\d*(?:\\.\\d+)?){14}),SN1_\\d+,\\d+");
		smatch match;

		string::const_iterator star = loadbin.begin();
		string::const_iterator end = loadbin.end();

		if (regex_search(star, end, match, reg)) {
			return make_shared<string>(match[0].first, match[0].second);
		}
	}
	return make_shared<string>();
}
enum {
	MAX_CRE_LEN = 200,
};
static char creDataBuff[MAX_CRE_LEN];
static int creDataLen = 0;

static void FlushData(const char * CreName)
{
	cout << "flush data" << endl;
	auto ret = GetFirstCreData(CreName);
	int Len = (ret->length() + 1) > MAX_CRE_LEN ? MAX_CRE_LEN : ret->length();

	if (Len < 10) {
#if 0
		const char * orgCre = "2018-11-22,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,SN1_000000000,0";
		Len = strlen(orgCre);
#else
		time_t now = time(0);
		tm nowtm;
		localtime_r(&now, &nowtm);
		char orgCre[512];
		Len = sprintf(orgCre, "%d-%d-%d,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,SN1_000000000,0"
			,nowtm.tm_year + 1900, nowtm.tm_mon + 1, nowtm.tm_mday
		);
#endif
		//creDataBuff[Len++] = 0;
		creDataLen = Len;
		memcpy(creDataBuff, orgCre, creDataLen);
	} else {
		//creDataBuff[Len++] = 0;
		creDataLen = Len;
		memcpy(creDataBuff, ret->c_str(), creDataLen);
	}
	cout << "cre len  = "<< creDataLen << ",crebuff =" << creDataBuff << endl;
}



static void SetCreData(const char * CreName
	, void(*profun)(const char*) )
{
	int fd = inotify_init1(IN_NONBLOCK);

	if (fd < 0) {
		return;
	}

	char buff[64];
	sprintf(buff, "touch %s", CreName);
	system(buff);

	int wd = inotify_add_watch(fd, CreName, IN_MODIFY | IN_CREATE | IN_DELETE | IN_DELETE_SELF);

	if (wd < 0) {
		close(fd);
		return;
	}

	pollfd ipollfd;
	ipollfd.fd = fd;
	ipollfd.events = POLL_IN;
	ipollfd.revents = 0;
	int len;
	while (true) {
		if (0 >= poll(&ipollfd, 1, -1))
			continue;
		if (0 < (len = read(fd, buff, 128))) {
			inotify_event * ievent;
			for (char * ptr = buff; ptr < buff + len;
				ptr += sizeof(inotify_event) + ievent->len
				) {
				ievent = (inotify_event*)(ptr);
#if 1
				fprintf(stdout, "len = %d ,mask = %d,watch fd = %d,len = %d", len, ievent->mask, ievent->wd, ievent->len);
				if (ievent->len) {
					fprintf(stdout, ",name= %s", ievent->name);
				}
				fprintf(stdout, "\n");
#endif
				profun(CreName);
			}
		}
	}
}
#endif


void init_inotify(const char * CreName)
{
	static bool InitFlag = false;

	if (InitFlag == false) {
		printf("init inotify\n");
		InitFlag = true;
		FlushData(CreName);
		thread P(SetCreData, CreName, FlushData);
		P.detach();
	}
}


int JD_cre_response(JD_INFO & jif, JD_FRAME & jfr)
{
	MDC_INFO & jit = (MDC_INFO &)jif;
#if USING_FAKE_CRE
	char buff[MAX_CRE_LEN];
	int crelen = mk_fake_cre_message(buff);
#else
	init_inotify("PES.sn");
    int crelen = creDataLen;
	char(&buff)[MAX_CRE_LEN] = creDataBuff;
#endif

	if (jit.dbg_cre_printf)printf("send cre len = %d,message=%s\n", crelen, buff);

	jfr.jd_send_buff = buff;
	jfr.jd_data_len = crelen;
	jfr.jd_command |= 0x80;
	JD_send(jif, jfr);
	return JD_OK;
}

