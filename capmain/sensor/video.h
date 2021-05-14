#ifndef __my_video_h____
#define __my_video_h____

#include <iostream>
#include <linux/videodev2.h>
#include <memory>

/********************/
#include <linux/types.h>
#include <sys/types.h>
#include <stdbool.h>
#define BUFFER_COUNT 2

/********************/

struct CAP_FRAME {
	unsigned char * startAddr;
	unsigned int length;
	unsigned int width;
	unsigned int heigth;

	struct v4l2_buffer buf;
	int useFlag = 0;
	int fd;
	//int reset();
};

std::shared_ptr<CAP_FRAME> get_one_frame(int fd);
//CAP_FRAME *get_one_frame(int fd);

typedef struct{
	void *start;
	size_t length;
}framebuf_t; 

typedef struct {
	char *devName;      	//摄像头设备名
	char *jpgName;
	unsigned int fd;
	int image_mode;			//图片尺寸类型
	int image_overturn;     //图片翻转
	unsigned short width;	//宽度
	unsigned short height;	//高度
	unsigned int gain;   	//增益
	unsigned int exposure;	//曝光

	unsigned char *graybuff;

	framebuf_t framebuf[BUFFER_COUNT];
	unsigned int frameindex;
	
	bool use_flag;
	
}video_context_t;




//#include "context.h"
//extern int video_capture(struct video_context_t* ctx);
//extern void video_free(struct video_context_t* ctx);
//extern void video_list_modes();
//extern void video_match_mode(struct video_context_t* ctx,const char* name);

//extern void take_picture(struct video_context_t* ctx);



#endif

