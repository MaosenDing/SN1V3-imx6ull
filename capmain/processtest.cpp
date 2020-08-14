
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

#include <syslog.h>










