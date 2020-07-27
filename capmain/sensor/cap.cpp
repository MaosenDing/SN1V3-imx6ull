
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

ERR_STA loop_cap2JPG(const unsigned int gain, const unsigned int expo
	, const int horizenFlip, const int VeriFlip
)
{
	my_cap_init(gain, expo, horizenFlip, VeriFlip);

	ERR_STA err;

	if (video_fd < 0) {
		SN1V2_ERROR_CODE_RET(err_sensor_open);
	}
	int ret = set_gain_expose(video_fd, gain, expo);

	if (ret < 0) {
		SN1V2_ERROR_CODE_RET(err_sensor_set);
	}
	int i = 0;
	while (true) {
		shared_ptr< CAP_FRAME> fram = get_one_frame(video_fd);

		char name[64];

		snprintf(name, 64, "/home/cap/test%d.jpg", i++);

		if (fram && fram->useFlag) {
			SaveRGB565Jpg(name, fram->startAddr, 1600, 1200);
		}
	}
}

static inline void convert_yuv_to_rgb_pixel(int y, int u, int v,uint16_t * rgb565)
{
	int r = y + v + ((103 * v) >> 8);
	int g = y - ((u * 88) >> 8) - ((v * 183) >> 8);
	int b = y + u + (u * 198) >> 8;
	*rgb565 = (((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3));
}

int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
	unsigned int in, out = 0;
	unsigned int pixel_16;
	unsigned char pixel_24[3];
	unsigned int pixel32;
	int y0, u, y1, v;

	width = width & (~127);

	for (in = 0; in < width * height * 2; in += 4) {

		y0 = yuv[in + 0];
		u = yuv[in + 1];
		y1 = yuv[in + 2];
		v = yuv[in + 3];


		convert_yuv_to_rgb_pixel(y0, u, v, (uint16_t*)&rgb[in]);

		convert_yuv_to_rgb_pixel(y1, u, v, (uint16_t*)&rgb[in + 2]);

	}
	return 0;
}

void YUV422ToRGB565(const void* inbuf, void* outbuf, int width, int height);


void RGB888_2_565(uint8_t * srcdata, uint8_t *dst, size_t pixCount);

ERR_STA cap_once(unsigned char * rgb565buff, int &insize, const unsigned int gain, const unsigned int expo
	, const int horizenFlip, const int VeriFlip
)
{
	ERR_STA err;

	if (video_fd < 0) {
		SN1V2_ERROR_CODE_RET(err_sensor_open);
	}
	int ret = set_gain_expose(video_fd, gain, expo);

	if (ret < 0) {
		SN1V2_ERROR_CODE_RET(err_sensor_set);
	}

	{
		//auto p0 = get_one_frame(video_fd);
		//auto p1 = get_one_frame(video_fd);
	}
	
	shared_ptr< CAP_FRAME> fram = get_one_frame(video_fd);

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



