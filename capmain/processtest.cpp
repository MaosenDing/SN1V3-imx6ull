
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
#include <fstream>
using namespace std;

void RGB888_2_565(uint8_t *srcdata, uint8_t *dst, size_t pixCount);
int getJPEGfromFile(const char *file, vector<unsigned char> &outData, int &w, int &h, int &imgType);
ERR_STA ImageProcessRGB(const char *saveName, shared_ptr<unsigned char> LoadImg, int inputSize, int width, int height, PROCESS_RESULT &res, int thres, float thresPer, bool BINjpgSaveFlag = true, unsigned int MinCntGrp = 50);
//ERR_STA ImageCapRGB(const char * dstPath, int width, int height, PROCESS_RESULT & res, int thres, float thresPer
//	, bool ORGjpgSaveFlag, bool BINjpgSaveFlag, unsigned int MinCntCrp, const unsigned int gain, const unsigned int expo
//	, const int horFlip, const int VerFlip
//);
ERR_STA ImageCap(const char * dstPath, int width, int height, PROCESS_RESULT & res, int thres, float thresPer
	, bool ORGjpgSaveFlag, bool BINjpgSaveFlag, unsigned int MinCntCrp, const unsigned int gain, const unsigned int expo
	, const int horFlip, const int VerFlip
);


int processTest(int argc, char *argv[])
{
	PROCESS_RESULT res;
	int thres = 200;
	float thresPer = 0.8;
	int BINjpgSaveFlag = 0;
	int MinCntGrp = 50;

	vector<uint8_t> dat;
	int getwidth = 0;
	int getheigth = 0;
	int imgtype = 0;
	int ret = getJPEGfromFile("1.jpeg", dat, getwidth, getheigth, imgtype);

	printf("ret = %d , w = %d , h = %d , type = %d ,len = %d\n", ret, getwidth, getheigth, imgtype, dat.size());

	int imgSize = getwidth * getheigth * 2;
	shared_ptr<unsigned char> basemem(new unsigned char[imgSize], [](unsigned char *p) { delete[] p; });
	RGB888_2_565(&dat[0], &*basemem, getwidth * getheigth);

	ofstream logout("/timlog.txt");
	for (int i = 0; i < 10000000; i++) {
		auto start = std::chrono::system_clock::now();
		shared_ptr<unsigned char> ppp(new unsigned char[imgSize], [](unsigned char *p) { delete[] p; });
		memcpy(&*ppp, &*basemem, imgSize);
		//TimeInterval ppp2("test:");
		ImageProcessRGB("tp/test.jpg", std::move(ppp), imgSize, getwidth, getheigth, res, thres, thresPer, BINjpgSaveFlag, MinCntGrp);
		auto end = std::chrono::system_clock::now();
		logout << std::chrono::duration_cast<std::chrono::duration<float> >(end - start).count() << endl;
	}
	return 0;
}
void neon_test(uint8_t *srcdata, uint8_t *dst, size_t pixCount);

struct tesppp {
	int i;
	~tesppp()
	{
		printf("%d destruction\n", i);
	}
};

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

void testthread(shared_ptr<tesppp> p, int index)
{
	p->i = index;
	printf("get index = %d\n", index);
}

int processTest2(int argc, char *argv[])
{
#define NUM (32)
	uint8_t test[NUM];
	uint8_t out[NUM];

	for (int in = 0; in < NUM; in++) {
		test[in] = in;
	}

	void neon_test(uint8_t * srcdata, uint8_t * dst, size_t pixCount);
	neon_test((uint8_t *)test, out, 1);

	for (size_t i = 0; i < NUM; i++) {
		printf("index %d = %d\n", i, out[i]);
	}
	boost::asio::thread_pool pool(10);

	for (size_t i = 0; i < 10; i++) {
		auto p = make_shared<tesppp>();
		boost::asio::post(pool, bind(testthread, p, i));
		boost::asio::post(pool, bind(testthread, p, i + 100));
		sleep(1);
	}

	sleep(10);

	return 0;
}

#include "SunPosTable.h"
#include "sn1v3cfg.h"
#include "tableWork.h"
#include "configOperator.h"
#include "JDcomhead.h"
#include "jd_share.h"
#include "SN1V2_error.h"
#include "errHandle.h"



int createTable(int argc, char *argv[]);

void init_ctrl_thread(void);
void set_deg(int flg, float deg1, float deg2);

int tableGenerate3(int argc, char *argv[])
{
	cout << "table generate3" << endl;
	int nomdcflg = ChkifCMD(argc, argv, "-nomdc");

	if (nomdcflg) {
		printf("no mdc ctrl\n");
	}

	Tg_table tg_table;
	scanfAllTable(tg_table, Mask_All);

	logInit("aim", "./aim", google::GLOG_ERROR);
	int gain = tg_table.T6.gain;
	int expose = tg_table.T6.expo;

	int SGT = tg_table.T6.SGT;
	float SBT = tg_table.T6.SBT;
	int MCP = tg_table.T6.MCP;

	printf("SGT = %d , SBT = %f , MCP = %d\n", SGT, SBT, MCP);

	my_cap_init(gain, expose, 0, 0);
	time_t now = time(0);
	tm t2;

	localtime_r(&now, &t2);

	const int year = t2.tm_year + 1900;
	const int mon = t2.tm_mon + 1;
	const int day = t2.tm_mday;

	//创建拍摄暂存目录
	char storPath[24];
	sprintf(storPath, "%04d_%02d_%02d", year, mon, day);
	mkdir(storPath, 0777);

	//照片存储目录
	char photoPath[24];
#if 0
	if (strlen(cfg.ForceSavePath)) {
		SN1V2_ERR_LOG("force save jpeg in %s\n", cfg.ForceSavePath);
		cfg.FLAG_SAVE_BIN = 1;
		cfg.FLAG_SAVE_ORG = 1;

		sprintf(photoPath, "%s/%04d_%02d_%02d", cfg.ForceSavePath, year, mon, day);
		mkdir(photoPath, 0777);
	} else
#endif
	{
		strcpy(photoPath, storPath);
	}
	auto tab = createTable(tg_table, year, mon, day);
	char tableName[36];
	snprintf(tableName, 36, "%d-%d-%d-table.txt", year, mon, day);
	saveSunTable(tab, tableName);

	int workflg = 1;

	if (!nomdcflg) {
		init_ctrl_thread();
	}

	int sleepflg = 0;
	float x_pos = 0;
	float y_pos = 0;
	if (tab) {
		printf("size = %d\n", tab->size());
		while (workflg) {
			if (nomdcflg) {
				sleep(1);
			} else if (sleepflg == 1) {
				sleep(10);
			} else if (sleepflg == 2) {
				sleep(5); //与轨迹表周期一样长
			} else {
				sleep(1);
			}

			PROCESS_RESULT res;
			ERR_STA err = ImageCap(photoPath, 2592, 1944, res, SGT, SBT, true, false, MCP, gain, expose, 0, 0);
			float x_diff = 0, y_diff = 0;
			if (err == err_ok) {
				x_diff = res.diff_x + 2592 / 2;
				y_diff = res.diff_y + 1944 / 2;
				printf("pos = %f,%f\n", x_diff, y_diff);
			} else {
				x_diff = 4000;
				y_diff = 4000;
				printf("pos = null\n");
			}

			time_t timGetImg = time(nullptr);
			tm reftime;
			localtime_r(&timGetImg, &reftime);

			SUNPOS tabsun;
			if (0 == find_useful_pos(reftime.tm_hour, reftime.tm_min, reftime.tm_sec, *tab, tabsun)) {
				float zrat = 0, zraz = 0;
				int speedat = 0, speedaz = 0;

				float x_pos_2 = abs(x_diff - tabsun.ZR_u) - abs(x_pos);
				float y_pos_2 = abs(y_diff - tabsun.ZR_v) - abs(y_pos);

				x_pos = abs(x_diff - tabsun.ZR_u);
				y_pos = abs(y_diff - tabsun.ZR_v);

				ConAlg(x_diff, y_diff, tabsun.ZR_u, tabsun.ZR_v, tabsun.ZR_At,
				       tabsun.ZR_Az, 1, 1, tg_table.T6.SN1_P3, tg_table.T6.SN1_P4_x,
				       tg_table.T6.SN1_P4_y, x_pos_2, y_pos_2, &zrat, &zraz,
				       &speedat, &speedaz, &sleepflg);

				set_deg(0, zrat, zraz);

				SN1V2_ERR_LOG("input = %f,%f,%lf,%lf,%lf,%lf,%lf,%lf", y_diff, x_diff, y_diff - tabsun.ZR_v, x_diff - tabsun.ZR_u, tabsun.test[1],tabsun.test[0],tabsun.ZR_At, tabsun.ZR_Az);
				SN1V2_ERR_LOG("conalg = %f,%f,%d,%d\n", zrat, zraz, speedat, speedaz);
				SN1V2_ERR_LOG("ideal_UV = %f,%f\n", tabsun.ZR_u,tabsun.ZR_v);

			} else {
				printf("find fail\n");
			}
		}
	}
	return 0;
}
