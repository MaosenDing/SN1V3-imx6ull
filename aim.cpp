
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include "SN1V2_com.h"
#include "timSet.h"
#include <algorithm>
#include "jpgInc/jpeglib.h"
#include "time_interval.h"
#include <sys/stat.h>
#include "errHandle/errHandle.h"
#include "tableWork.h"
#include <thread>
#include <unistd.h>
#include <chrono>
#include <regex>
#include <sys/prctl.h>
#include "camera.h"
//#include "communicate.h"
#include "errHandle/errHandle.h"

#include <iostream>
using namespace std;


struct MAIN_CMD {
	const char * cmd_name;
	int(*cmd_fun)(int, char *argv[]);
};

int rtf_test(int argc, char * argv[])
{
	clog << "RTF test" << endl;
	cout << "RTF test" << endl;
	return 0;
}

int cppReg(int argc, char * argv[])
{
	cout << "c++ reg test" << endl;
	if (argc >= 3) {
		string aim(argv[2]);
		cout << "input =" << aim << endl;
		regex reg("(\\d{4})-(\\d{1,2})-(\\d{1,2})_(\\d{1,2}):(\\d{1,2}):(\\d{1,2})");
		smatch match;

		string::const_iterator star = aim.begin();
		string::const_iterator end = aim.end();
		while (regex_search(star, end, match, reg)) {
			cout << "reg count = " << match.size() << endl;
			for (auto & p : match) {
				cout << p << endl;
			}
			star = match[0].second;
		}

		return 0;
	} else {
		cout << "par error" << endl;
	}
	return -1;
}



static int coredump_sub(int * p, int num)
{
	*p = num;
	return 0;
}


static int coredump(int argc, char * argv[])
{
	cout << "core dump test" << endl;

	if (argc >= 2) {
		logInit("once", "./aim", google::GLOG_ERROR);
		
		SN1V2_ERR_LOG("test error");

		coredump_sub(0, 0);
	}
	return 0;
}




static int capOnce(int argc, char * argv[])
{
	cout << "table generate" << endl;

	if (argc >= 5) {
		logInit("once", "./aim", google::GLOG_ERROR);

		ERR_STA err;

		char * configName = argv[2];

		cout << "ConfName = " << configName << endl;

		SN1_CFG cfg;
		if ((err = getConf(configName, &cfg, 0)) != err_ok)
			return err;

		cfg.gain = atoi(argv[3]);
		cfg.expo = atoi(argv[4]);

		my_cap_init(cfg.gain, cfg.expo, cfg.isHorisFlip, cfg.isVeriFlip);

		fprintf(stdout, "gain is %d , expo is %d\n", cfg.gain, cfg.expo);

		int sysret = system("mkdir -p capOnce");

		if (sysret < 0) {
			return sysret;
		}

		//get shard memory
		//key_t key = getKey(SHARE_KEY_PATH, SHARE_KEY_INT);
		//SN1_SHM * psn1 = (SN1_SHM *)getSHM(key, sizeof(SN1_SHM));

		char capOnce[32] = "capOnce";
		char testCSC[] = "test.csv";

		if (strlen(cfg.ForceSavePath)) {
			sprintf(capOnce, "%s/capOnce", cfg.ForceSavePath);
			mkdir(capOnce, 0777);
			cfg.FLAG_SAVE_BIN = 1;
			cfg.FLAG_SAVE_ORG = 1;
		}
		capWork dw(capOnce, cfg, nullptr, testCSC);
		dw.test_for_cap_once = 1;

		//sleep(5);

		timTableSet ts;
		ts.tt = time(0);
		dw.work(ts, 0);
	}
	return 0;
}

int creSaveTest(int argc, char * argv[])
{
	TimeInterval ppp2("cre:");
	cout << "test cre" << endl;
	if (argc >= 8) {
		int year, mon, day, SID;
		double sfl, sps;
		year = atoi(argv[2]);
		mon = atoi(argv[3]);
		day = atoi(argv[4]);
		sps = atof(argv[5]);
		sfl = atof(argv[6]);
		SID = atoi(argv[7]);
		fprintf(stdout, "%d-%d-%d,%lf,%lf,id = %d\n", year, mon, day, sfl, sps, SID);

		char buff[64];
		snprintf(buff, 64, "date %d.%d.%d-%d:%d", year, mon, day, 9, 58);

		system(buff);

		CREOBJ credata;
		for (auto & p : credata.errData) {
			p = 0;
		}
		const char * creName = "PES.sn";
		const char * resName = "sr.txt";

		shared_ptr< vector< CREOBJ >> creDataGrp = make_shared<vector<CREOBJ>>();
		CREOBJ  creData = GetNewCre(*creDataGrp);
		SN1_CFG cfg;

		int err;
		int ret;

		char configName[] = "SCG.txt";
		if ((err = getConf(configName, &cfg)) != err_ok)
			return err;


		if ((ret = res_filter(resName, &cfg, creData.extra)) != err_ok) {
			SN1V2_ERR_LOG("filter ret = %d\n", ret);
			//保存数据
		} else 	if ((ret = CalCre(resName, creData, cfg.SPS, cfg.SFL)) == err_ok) {
			//添加成功的cre数据
			creDataGrp->push_back(creData);
			//导出文本
			save_cre(creName, *creDataGrp, cfg.SID);
		} else {
			SN1V2_WARN_LOG("CalCre ret = %d\n", ret);
		}
	} else {
		cout << "err par" << endl;
	}
	return 0;
}

int ipchange(const char * inFileName, const char * outFileName)


int ipchange_pp(int argc, char * argv[])
{
	cout << "ipchange" << endl;

	cout << "name " << argv[2] << endl;
	ipchange(argv[2],"test.txt");
	return 0;
}


MAIN_CMD cmd_group[] = {
	{"RTF",rtf_test},
	{"CPPREG",cppReg},
	{ "capOnce" ,capOnce},
	{"coredump",coredump},
	{ "CREsave" ,creSaveTest},
	{"ip",ipchange_pp},
};





int main(int argc, char *argv[])
{
	prctl(PR_SET_NAME, "main service");

	if (argc < 2) {
		cout << "test par error" << endl;
		return -1;
	}

	for (auto & p : cmd_group) {
		if (!strcmp(argv[1], p.cmd_name)) {
			return p.cmd_fun(argc, argv);
		}
	}

	cout << "no command" << endl;

	return 0;

}