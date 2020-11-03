
#include <iostream>
#include <stdio.h>
#include <thread>
#include <condition_variable>
//#include "camera.h"
#include <vector>
#include <SN1V2_com.h>
#include <SN1V2_error.h> 
#include <errHandle/errHandle.h>
#include "time_interval.h"
#include <stdlib.h>
#include <chrono>
#include <fcntl.h>
#include "video.h"
#include <sys/prctl.h>
#include <fstream>



int init_cap(const char * videoName);
int set_gain_expose(int fd, int gain, int expose);
int my_cap_init(unsigned int gain, unsigned int expo, int isHorFlip, int isVerFlip);
using namespace std;


static int video_fd = -1;


//RGB565保存jpeg
int rgb565_to_jpeg(unsigned char * rgbst, int pwidth, int pheigth, int fname)
{
	ERR_STA err;

	time_t now = time(0);
	tm t2;
	localtime_r(&now, &t2);

	int year = t2.tm_year + 1900;
	int mon = t2.tm_mon + 1;
	int day = t2.tm_mday;

	char jpegName[128] = { 0 };       //图像保存名称

	sprintf(jpegName, "jpeg_%04d_%02d_%02d/%d.jpeg", year, mon, day, fname);
	printf("jpeg image %d\n", fname);
	fflush(stdout);        //清空输出缓冲区

	if ((err = SaveRGB565Jpg(jpegName, rgbst, pwidth, pheigth)) != err_ok) {
		cout << "save error = " << (int)err << endl;
	} else {
		cout << "save ok" << endl;
	}

	return 0;
}
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <boost/pool/pool.hpp>

boost::pool<> mempl2M(2 * 1920 * 1080);

ERR_STA SaveyuyvJpg(char * fName, unsigned char * yuyv, int width, int heigth);
void YUV422ToRGB565(const void* inbuf, void* outbuf, int width, int height);
void compress(void * p, int i)
{
	//TimeInterval tim("compress:");
	char name[64];
	snprintf(name, 64, "/tmp/cap/test%d.jpg", i % 10);
	SaveyuyvJpg(name, (unsigned char *)p, 1920, 1080);
	mempl2M.free(p);
}




ERR_STA loop_cap2JPG(const unsigned int gain, const unsigned int expo
	, const int horizenFlip, const int VeriFlip
)
{
	my_cap_init(gain, expo, horizenFlip, VeriFlip);


	if (video_fd < 0) {
		SN1V2_ERROR_CODE_RET(err_sensor_open);
	}
	int ret = set_gain_expose(video_fd, gain, expo);

	if (ret < 0) {
		SN1V2_ERROR_CODE_RET(err_sensor_set);
	}
	boost::asio::thread_pool pool(10);

	for (int i = 0; i < 10000000000; i++) {
		TimeInterval tim("once:");
		shared_ptr< CAP_FRAME> fram = get_one_frame(video_fd);
		if (fram && fram->useFlag) {
			TimeInterval tim("pp:");
			void *p = mempl2M.malloc();
			if (p) {
				if (fram && fram->useFlag) {
					TimeInterval tim("pp2:");
					memcpy(p, fram->startAddr, fram->length);
					fram.reset();
					boost::asio::post(pool, bind(compress, p, i));
				}
			}
		}
	}
}




void RGB888_2_565(uint8_t * srcdata, uint8_t *dst, size_t pixCount);

ERR_STA cap_once(unsigned char * rgb565buff, int &insize, const unsigned int gain, const unsigned int expo
	, const int horizenFlip, const int VeriFlip
)
{
	if (video_fd < 0) {
		SN1V2_ERROR_CODE_RET(err_sensor_open);
	}
	int ret = set_gain_expose(video_fd, gain, expo);

	if (ret < 0) {
		SN1V2_ERROR_CODE_RET(err_sensor_set);
	}

	shared_ptr< CAP_FRAME> fram;
	{
		TimeInterval ppp2("CAP_FRAME:");
		fram = get_one_frame(video_fd);
	}


	if (fram && fram->useFlag) {
		TimeInterval ppp2("yuv:");
		YUV422ToRGB565(fram->startAddr, rgb565buff, 1920, 1080);
		return err_ok;
	}

	SN1V2_ERROR_CODE_RET(err_sensor_catch);
}




int my_cap_init(unsigned int gain, unsigned int expo, int isHorFlip, int isVerFlip)
{
	video_fd = init_cap("/dev/video1");

	if (video_fd < 0) {
		cerr << "set video error" << endl;
		return -1;
	}
	set_gain_expose(video_fd, gain, expo);

	if (video_fd >= 0) {
		return 0;
	}
	return -1;
}

void YUV422ToGray(const void* inbuf, void* outbuf, int width, int height, int flg);
ERR_STA SaveGRAYJpg(char * fName, unsigned char * regImg, int width, int heigth);


void saveThread(unsigned char * srcbuff, string name)
{
	static int index = 0;
	static char savename[64];

	if (0 == strcmp(savename, name.c_str())) {
		printf("skip %s\n", name);
		return;
	}

	unsigned char * buff = (unsigned char *)mempl2M.malloc();
	YUV422ToRGB565(srcbuff, buff, 1920, 1080);

	strcpy(savename, name.c_str());
	printf("save %s\n", savename);

	SaveRGB565Jpg(savename, buff, 1920, 1080);
	mempl2M.free(buff);

	char tmpsavename[64];
	snprintf(tmpsavename, 64, "cp %s /tmp/cap/%d.jpg", savename, index);
	system(tmpsavename);
}

static void saveJPG(string savename, void * srcbuff)
{
#if 0
	boost::asio::thread_pool pool(10);
	boost::asio::post(pool, bind(saveThread, (unsigned char *)srcbuff, savename));
#else
	thread p(bind(saveThread, (unsigned char *)srcbuff, savename));
	p.detach();
#endif
}




ERR_STA cap_once_gray(unsigned char * graybuff, int &insize, const unsigned int gain, const unsigned int expo
	, const int horizenFlip, const int VeriFlip, char * savename
)
{
	if (video_fd < 0) {
		SN1V2_ERROR_CODE_RET(err_sensor_open);
	}
	int ret = set_gain_expose(video_fd, gain, expo);

	if (ret < 0) {
		SN1V2_ERROR_CODE_RET(err_sensor_set);
	}

	shared_ptr< CAP_FRAME> fram;
	{
		TimeInterval ppp2("CAP_FRAME:");
		fram = get_one_frame(video_fd);
	}


	if (fram && fram->useFlag) {
#if 1
		TimeInterval ppp2("gray:");
		if (savename) {
			saveJPG(savename, fram->startAddr);
		}
#endif
		{
			TimeInterval ppp2("gray22222:");
			YUV422ToGray(fram->startAddr, graybuff, 1920, 1080, 3);
		}
		return err_ok;
	}

	SN1V2_ERROR_CODE_RET(err_sensor_catch);
}

