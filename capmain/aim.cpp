
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
#include <jd_share.h>
#include <iostream>
#include <math.h>
#include "timeTableV2.h"
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

ERR_STA ImageCapRGB(const char * dstPath, int width, int height, PROCESS_RESULT & res, int thres, float thresPer
	, bool ORGjpgSaveFlag, bool BINjpgSaveFlag, unsigned int MinCntCrp, const unsigned int gain, const unsigned int expo
	, const int horFlip, const int VerFlip
);


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
		capWork dw(capOnce, cfg, nullptr, testCSC, &ImageCapRGB);
		dw.test_for_cap_once = 1;

		//sleep(5);
		int logflg = argc >= 6 ? 1 : 0;

		do {
			timTableSet ts;
			memset(&ts, 0, sizeof(timTableSet));
			ts.tt = time(0);
			dw.work(ts, 0);
		} while (logflg);
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


int ipchange_pp(int argc, char * argv[])
{
	int ipchange(const char * inFileName, const char * outFileName);
	cout << "ipchange" << endl;

	cout << "name " << argv[2] << endl;
	ipchange(argv[2],"test.txt");
	return 0;
}


bool checkNoMdc(int argc, char * argv[])
{
	for (int i = 0; i < argc; i++) {
		if (!strcmp("-nomdc", argv[i])) {
			printf("run without mdc\n");
			return true;
		}
	}
	return false;
}


int tableGenerate2(int argc, char * argv[])
{
	cout << "table generate" << endl;

	bool NoMdcFlag = checkNoMdc(argc, argv);

	if (argc >= 3) {
		int ret;
		ERR_STA err;

		char * configName = argv[2];

		cout << "ConfName = " << configName << endl;

		SN1_CFG cfg;
		if ((err = getConf(configName, &cfg)) != err_ok)
			return err;

		logInit("aim", "./aim", google::GLOG_ERROR);

		my_cap_init(cfg.gain, cfg.expo, cfg.isHorisFlip, cfg.isVeriFlip);
		time_t now = time(0);
		tm t2;

		localtime_r(&now, &t2);
		{
#if 0
			char buff[64];
			sprintf(buff, "tim = %d,%d,%d,%d,%d,%d", t2.tm_year + 1900, t2.tm_mon + 1, t2.tm_mday, t2.tm_hour, t2.tm_min, t2.tm_sec);
			cerr << buff << endl;
			cout << buff << endl;
#endif
		}

		const int year = t2.tm_year + 1900;
		const int mon = t2.tm_mon + 1;
		const int day = t2.tm_mday;

		//创建拍摄暂存目录
		char storPath[24];
		sprintf(storPath, "%04d_%02d_%02d", year, mon, day);
		mkdir(storPath, 0777);

		//照片存储目录

		char photoPath[24];
		if (strlen(cfg.ForceSavePath)) {
			SN1V2_ERR_LOG("force save jpeg in %s\n", cfg.ForceSavePath);
			cfg.FLAG_SAVE_BIN = 1;
			cfg.FLAG_SAVE_ORG = 1;

			sprintf(photoPath, "%s/%04d_%02d_%02d", cfg.ForceSavePath, year, mon, day);
			mkdir(photoPath, 0777);
		} else {
			strcpy(photoPath, storPath);
		}

		//获取cre
		const char * crePath = "PES.sn";
		auto creDataGrp = initCre(crePath);
		if (false == ChkTodayCre(*creDataGrp)) {
			CREOBJ  creData = GetNewCre(*creDataGrp);

			vector	<timTableSet> timeset;
			char TableName[64];
			sprintf(TableName, "%s/sh.txt", storPath);
#if 0
			cout << "TableName = " << TableName << endl;
#endif

			//获取时间表
			if ((ret = GetTableSet(TableName, creData
				, year, mon, day, cfg.Lon, cfg.lati, timeset
				, cfg.SSA, cfg.SHT, cfg.SPT, cfg.TDT, cfg.SCH, cfg.TEE, cfg.TEP, cfg.TET
				, cfg.SaveTimeTable
			)) != err_ok)
				return ret;
			//get shard memory
			key_t key = getKey(SHARE_KEY_PATH, SHARE_KEY_INT);

			SN1_SHM * psn1 = nullptr;
			if (NoMdcFlag == false) {
				psn1 = (SN1_SHM *)getSHM(key, sizeof(SN1_SHM));
				cfg.SID = psn1->mdc_id_num;
			}
			//修剪时间表
			RmTimeTableForTimeNotSuit(timeset, cfg.limitHourBeforeNoon, cfg.limitHourAfterNoon);

			char resName[64];
			snprintf(resName, 64, "%s/sr.txt", storPath);
			//get plan cap
			cfg.bf_size = 0;
			cfg.af_size = 0;
			for (auto & p : timeset) {
				if (p.tm_hour < 12)
					cfg.bf_size++;
				else
					cfg.af_size++;
			}
			//拍摄
			capWork dw(photoPath, cfg, psn1, resName, &ImageCapRGB);
			__attribute__((unused)) int sz = timTableWorkWithReserveTime(timeset, dw, cfg.max_reserve_time);

			//过滤res数据

			if (cfg.IsSaveCre == 0) {
				SN1V2_ERR_LOG("debug : not generate CRE process\n");
			} else
				if ((ret = res_filter(resName, &cfg, creData.extra)) != err_ok) {
					SN1V2_ERR_LOG("filter ret = %d\n", ret);
					//保存数据
				} else 	if ((ret = CalCre(resName, creData, cfg.SPS, cfg.SFL)) == err_ok) {
					//添加成功的cre数据
					creDataGrp->push_back(creData);
					//导出文本
					save_cre(crePath, *creDataGrp, cfg.SID);
				} else {
					SN1V2_WARN_LOG("CalCre ret = %d\n", ret);
				}
		}

		if (cfg.CleanLastDate != 0) {
			char _rmpath[20];
#if 0
			sprintf(_rmpath, "rm %s -rf\n", storPath);
			SN1V2_INF_LOG("rm %s -rf\n", storPath);
			system(_rmpath);
#else
			sprintf(_rmpath, "rm 20* -rf");
			SN1V2_INF_LOG("rm 20* -rf\n");
			system(_rmpath);

			sprintf(_rmpath, "rm 19* -rf");
			SN1V2_INF_LOG("rm 19* -rf\n");
			system(_rmpath);
#endif
		}
		SN1V2_INF_LOG("set work finished\n");
		if (NoMdcFlag == false) {
			shm_set_dayFlag(true);
		}
	}
	return 0;
}

int test_converter(int argc, char * argv[])
{
	float aimdeg = 82.24;
	unsigned int tmpdeg = Angle_Convert_UShort(aimdeg);

	char databuff[3];
	databuff[0] = (tmpdeg >> (0 * 8)) & 0xff;
	databuff[1] = (tmpdeg >> (1 * 8)) & 0xff;
	databuff[2] = (tmpdeg >> (2 * 8)) & 0xff;
	databuff[3] = (tmpdeg >> (3 * 8)) & 0xff;

	printf("from %f to %02x,%02x,%02x,%02x,,,%#08x\n", aimdeg, databuff[0], databuff[1], databuff[2],databuff[3],tmpdeg);
	
	printf("%d\n", tmpdeg);
	
	//printf("from %f to %02x,%02x,%02x\n", &tmpdeg);
	return 0;
}

#define PI 3.1415926535898
static double getdeg(double len)
{
	return acos((222929.1433 - (len + 85)*(len + 85)) / 221102.5584)*180.0f/ PI - 24.7421;
}


int degtest(int argc, char * argv[])
{
	double degpos = 0, diffdeg = 0;
	double difflen = 0.02;
	int sum = 0;
	{
		TimeInterval ppp2("degtest:");
		for (double pos = 0.0f; pos < 800; pos += 0.01) {
			sum++;
			double tmpdiff = fabs(getdeg(pos) - getdeg(pos + difflen));

			if (tmpdiff > diffdeg) {
				diffdeg = tmpdiff;
				degpos = pos;
			}
		}
	}
	printf("sum = %d , deg = %lf , diff = %lf\n", sum, degpos, diffdeg);
	return 0;
}


#include "sn1v3cfg.h"
void testpro();
int scanftest(int argc, char * argv[])
{
	testpro();
	return 0;
}


typedef struct {
	const char * name;
	const int value;
}TEST;

#define ppptest(val) {#val,val}
static TEST ttt[]{
	ppptest(1),
	ppptest(2),
	ppptest(3),
	ppptest(4),
	ppptest(5),
};

static const TEST * findttt(const char * name)
{
	auto p = find_if(begin(ttt), end(ttt)
		, [name](TEST & p) {return !strcmp(name, p.name); });
	if (p != end(ttt))
	{
		return p;
	}
	return NULL;
}



static int testfind(int argc, char * argv[])
{
	printf("1111111\n");
	const TEST * p = findttt("8");
	if (p) {
		printf("p %d\n", p->value);
	} else {
		printf("p find error \n");
	}
	return 0;
}

ERR_STA loop_cap2JPG(const unsigned int gain, const unsigned int expo
	, const int horizenFlip, const int VeriFlip
);

static int loopcap(int argc, char * argv[])
{
	int gain = 20;
	int expose = 100;
	if (argc >= 3) {
		gain = atoi(argv[2]);
		expose = atoi(argv[3]);
	}
	loop_cap2JPG(gain, expose, 0, 0);

	return 0;
}

int processTest(int argc, char * argv[]);
int processTest2(int argc, char * argv[]);
int testsunpos(int argc, char* argv[]);
int createTable(int argc, char* argv[]);
MAIN_CMD cmd_group[] = {
	{"RTF",rtf_test},
	{"CPPREG",cppReg},
	{ "capOnce" ,capOnce},
	{"coredump",coredump},
	{ "CREsave" ,creSaveTest},
	{"ip",ipchange_pp},
	{ "tableGen2" ,tableGenerate2 },
	{ "tv" , test_converter},
	{ "deg" , degtest},
	{"scanf",scanftest},
	{"testfind",testfind},
	{"testTimeTableV2",testTimeTableV2},
	{"loopcap",loopcap},
	{"pro",processTest},
	{"pro2",processTest2},
	{"pos",testsunpos},
	{"table",createTable},
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