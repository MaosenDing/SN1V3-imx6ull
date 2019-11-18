#ifndef __my_video_h____
#define __my_video_h____



#include <linux/videodev2.h>
#include <memory>


struct CAP_FRAME {
	unsigned char * startAddr;
	unsigned int length;
	unsigned int width;
	unsigned int heigth;

	struct v4l2_buffer buf;
	int useFlag = 0;
	int fd;
};


std::shared_ptr< CAP_FRAME> get_one_frame(int fd);





#endif

