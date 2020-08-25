
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


void RGB888_2_565(uint8_t * srcdata, uint8_t *dst, size_t pixCount);
int getJPEGfromFile(char * file, vector<unsigned char> &outData, int & w, int &h, int & imgType);
ERR_STA ImageProcessRGB(const char *saveName, shared_ptr<unsigned char> LoadImg, int inputSize, int width, int height, PROCESS_RESULT & res, int thres, float thresPer, bool BINjpgSaveFlag = true, unsigned int MinCntGrp = 50);
int processTest(int argc, char * argv[])
{


	PROCESS_RESULT  res;
	int thres = 200;
	float thresPer = 0.8;
	int BINjpgSaveFlag = 0;
	int MinCntGrp = 50;


	vector<uint8_t> dat;
	int getwidth = 0;
	int getheigth = 0;
	int imgtype = 0;
	int ret = getJPEGfromFile("1.jpeg", dat, getwidth, getheigth, imgtype);

	printf("ret = %d , w = %d , h = %d , type = %d ,len = %d\n"
		, ret, getwidth, getheigth, imgtype, dat.size());

	int imgSize = getwidth * getheigth * 2;
	shared_ptr<unsigned char >basemem(new unsigned char[imgSize], [](unsigned char * p) {delete[] p; });
	RGB888_2_565(&dat[0], &*basemem, getwidth * getheigth);

	ofstream logout("/timlog.txt");
	for (int i = 0; i < 10000000; i++) {
		auto start = std::chrono::system_clock::now();
		shared_ptr<unsigned char >ppp(new unsigned char[imgSize], [](unsigned char * p) {delete[] p; });
		memcpy(&*ppp, &*basemem, imgSize);
		//TimeInterval ppp2("test:");
		ImageProcessRGB("tp/test.jpg"
			, std::move(ppp)
			, imgSize, getwidth, getheigth
			, res, thres, thresPer
			, BINjpgSaveFlag, MinCntGrp);
		auto end = std::chrono::system_clock::now();
		logout << std::chrono::duration_cast<std::chrono::duration<float>>(end - start).count() << endl;
	}
	return 0;
}
void neon_test(uint8_t * srcdata, uint8_t *dst, size_t pixCount);


struct tesppp {
	int i;
	~tesppp()
	{
		printf("%d destruction\n", i);
	}
};

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

void testthread(shared_ptr<tesppp> p ,int index)
{
	p->i = index;
	printf("get index = %d\n", index);
}



int processTest2(int argc, char * argv[])
{
#define NUM (32)
	uint8_t test[NUM];
	uint8_t out[NUM];

	for (int in = 0; in < NUM; in++) {
		test[in] = in;
	}


	void neon_test(uint8_t * srcdata, uint8_t *dst, size_t pixCount);
	neon_test((uint8_t*)test, out, 1);

	for (size_t i = 0; i < NUM; i++) {
		printf("index %d = %d\n",i,out[i]);
	}
	boost::asio::thread_pool pool(10);
	
	for (size_t i = 0; i < 10; i++) {		
		auto p = make_shared<tesppp>();
		boost::asio::post(pool, bind(testthread, p, i));
		boost::asio::post(pool, bind(testthread, p, i+100));
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


ERR_STA ImageCap(const char * dstPath, int width, int height, PROCESS_RESULT & res, int thres, float thresPer
	, bool ORGjpgSaveFlag, bool BINjpgSaveFlag, unsigned int MinCntGrp, const unsigned int gain, const unsigned int expo
	, const int horflip, const int verFlip
);

int find_useful_pos(int hour, int min, int sec, vector<SUNPOS> & tab, SUNPOS & retcfg);
int createTable(int argc, char* argv[]);


static void make_rec_pack(unsigned char * rxbuf, int num, JD_FRAME & jfr)
{
	//recoder the head
	jfr.jd_frame_head = rxbuf;
	//jd command
	jfr.jd_command = rxbuf[2];
	jfr.seq = rxbuf[3];
	//aim address
	//maybe the device address
	jfr.jd_aim.byte_value.low_byte = rxbuf[4];
	jfr.jd_aim.byte_value.mlow_byte = rxbuf[5];
	jfr.jd_aim.byte_value.mhigh_byte = rxbuf[6];
	jfr.jd_aim.byte_value.high_byte = 0x00;
	//jd data load
	jfr.jd_data_len = rxbuf[7] - 10;
	if (jfr.jd_data_len > 0) {
		jfr.jd_data_buff = &rxbuf[8];
	} else {
		jfr.jd_data_buff = nullptr;
	}
}

shared_ptr< JD_FRAME> JD_pro_bare_buff(unsigned char * rxbuf, int num, JD_INFO & jif)
{
	for (int i = 0; i < num; i++) {
		int remainLen = num - i;
		if ((rxbuf[i] == 0XAA) && (rxbuf[i + 1] == 0XAA)) {
			if (remainLen > 7) {
				int recpackLen = rxbuf[i + 7];

				if (crc_check(recpackLen, &(*(rxbuf + i)), 0XFFFF) == 1) {
					if (jif.dbg_pri_chk_flag && jif.dbg_fp) fprintf(jif.dbg_fp, "crc ok\n");
					auto jfr = make_shared< JD_FRAME>();
					make_rec_pack(rxbuf + i, num - i, *jfr);

					return jfr;
				} else {
					if (jif.dbg_pri_chk_flag && jif.dbg_fp) fprintf(jif.dbg_fp, "crc error\n");
				}
			}
		}
	}
	return shared_ptr< JD_FRAME>();
}
static volatile float ctrl_deg1, ctrl_deg2;
static std::condition_variable_any enable_ctrl;
static std::timed_mutex mutex_ctrl;
void ctrl_thread(void)
{
	char namebuff[64];
	unsigned char buff[1024];
	strcpy(namebuff, "/dev/ttyUSB0");

	int fd = UARTX_Init(namebuff, 115200, 0, 8, 1, 0);	

	JD_INFO jif;
	jif.fd = fd;
	//jif.dbg_pri_snd_word = 1;
	JD_FRAME jfr;
	while (true) {
		unique_lock<timed_mutex> lck(mutex_ctrl);
		enable_ctrl.wait(lck);
		float deg1, deg2;
		//读取角度
		jfr.jd_aim.byte_value.low_byte = 0xff;
		jfr.jd_aim.byte_value.mlow_byte = 0xff;
		jfr.jd_aim.byte_value.mhigh_byte = 0xff;
		jfr.jd_command = 0x13;
		JD_send(jif, jfr);
		int rdsz = read(fd, buff, 1024);
		if (rdsz) {

			auto dat = JD_pro_bare_buff(buff, rdsz, jif);
			if (dat && dat->jd_data_len >= 6) {
				unsigned char tmpbuff[16];
				memcpy(tmpbuff, dat->jd_data_buff, 6);
				deg1 = Angle_Convert(&tmpbuff[0]);
				deg2 = Angle_Convert(&tmpbuff[3]);
				//printf("x = %f,%f\n", deg1, deg2);
			}
		}
		//设定新角度
		jfr.jd_aim.byte_value.low_byte = 0xff;
		jfr.jd_aim.byte_value.mlow_byte = 0xff;
		jfr.jd_aim.byte_value.mhigh_byte = 0xff;
		jfr.jd_command = 0x0B;
		
		unsigned char sndbuf[20];
		//if ((fabs(ctrl_deg1) < 0.00001f) || (fabs(ctrl_deg2) < 0.00001f)) {
		//	continue;
		//}

		float aimf1 = deg1 + ctrl_deg1;
		float aimf2 = deg2 + ctrl_deg2;
		printf("using deg %f,%f,%f,%f\n",deg1,deg2 , ctrl_deg1, ctrl_deg2);

		Angle_Convert_UShort(aimf1, sndbuf + 0);
		Angle_Convert_UShort(aimf2, sndbuf + 3);
		jfr.jd_send_buff = sndbuf;
		jfr.jd_data_len = 6;
		JD_send(jif, jfr);

		rdsz = read(fd, buff, 1024);
		ctrl_deg1 = 0;
		ctrl_deg2 = 0;
		if (rdsz) {

			auto dat = JD_pro_bare_buff(buff, rdsz, jif);
			if (dat) {
				printf("sendok\n");
			}
		}
	}
}


int tableGenerate3(int argc, char * argv[])
{
	cout << "table generate3" << endl;


	int ret;
	ERR_STA err;

	Tg_table tg_table;
	scanfAllTable(tg_table, Mask_All);

	logInit("aim", "./aim", google::GLOG_ERROR);
	int gain = 50;
	int expose = 100;


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
	createTable(argc, argv);
	auto tab = createTable(tg_table, year, mon, day);
	int lastsec = 0;
	int workflg = 1;

	thread t_ctrl(ctrl_thread);
	t_ctrl.detach();

	int sleepflg = 0;
	float x_pos = 0;
	float y_pos = 0;
	float r2 = 1;
	if (tab) {
		printf("size = %d\n", tab->size());
		while (workflg) {
			if (sleepflg) {
				sleep(10);
			}

			PROCESS_RESULT res;
			ERR_STA err = ImageCap(photoPath, 1920, 1080, res, 200, 0.8, true, false, 50, gain, expose, 0, 0);
			float x_diff = 0, y_diff = 0;
			if (err == err_ok) {
				x_diff = res.diff_x + 1920 / 2;
				y_diff = res.diff_y + 1080 / 2;
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
				

				if (x_pos_2 > 0) {
					r2 = -r2;
				}

				ConAlg(x_diff, y_diff, tabsun.ZR_u, tabsun.ZR_v, tabsun.ZR_At, tabsun.ZR_Az, 1, r2, tg_table.T6.SN1_P3, tg_table.T6.SN1_P4_x, tg_table.T6.SN1_P4_y
					, &zrat, &zraz, &speedat, &speedaz);


				if (speedaz == 0) {
					if (x_pos*x_pos + y_pos * y_pos > 50 * 50) {
						sleepflg = 1;
						zrat = zrat > 0 ? 5 : -5;
						zraz = zraz > 0 ? 5 : -5;
					} else {
						sleepflg = 0;
						zrat = zrat > 0 ? 1 : -1;
						zraz = zraz > 0 ? 1 : -1;
					}
				} else {
					sleepflg = 0;
				}

				ctrl_deg1 = zrat;
				ctrl_deg2 = zraz;

				SN1V2_ERR_LOG("input = %f,%f,%lf,%lf,%lf,%lf", x_diff, y_diff, x_diff - tabsun.ZR_u, y_diff - tabsun.ZR_v, tabsun.ZR_At, tabsun.ZR_Az);
				SN1V2_ERR_LOG("conalg = %f,%f,%d,%d\n", zrat, zraz, speedat, speedaz);
				enable_ctrl.notify_all();
			} else {
				printf("find fail\n");
			}
		}
	}
	return 0;
}


