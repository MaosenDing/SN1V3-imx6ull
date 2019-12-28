#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include "SN1V2_com.h"
#include <iostream>
#include <video.h>

using namespace std;
#define UXGA_WIDTH (1600)
#define UXGA_HEIGHT (1200)

#define VGA_WIDTH (640)
#define VGA_HEIGHT (480)

#if 0
#define WIDTH_INIT VGA_WIDTH
#define HEIGTH_INIT VGA_HEIGHT
#else
#define WIDTH_INIT UXGA_WIDTH
#define HEIGTH_INIT UXGA_HEIGHT
#endif

struct buffer {
	void *start;
	unsigned int length;
};
buffer buffers[3];

int init_cap(const char * videoName)
{
	int fd = open(videoName, O_RDWR);
	printf("TK------->>>fd is %d\n", fd);
	//////
	struct v4l2_capability cap;
	ioctl(fd, VIDIOC_QUERYCAP, &cap);
	printf("TK---------->>>>>Driver Name:%s\nCard Name:%s\nBus info:%s\n", cap.driver, cap.card, cap.bus_info);

	printf("Driver Name : %s\nCard Name : %s\nBus info : %s\nDriver Version : %u.%u.%u\n", cap.driver, cap.card, cap.bus_info, (cap.version >> 16) & 0XFF, (cap.version >> 8) & 0XFF, cap.version & 0XFF);
	//////
	struct v4l2_fmtdesc fmtdesc;
	fmtdesc.index = 0; 
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1) {
		printf("TK-------->>>>>fmtdesc.description is %s\n", fmtdesc.description);
		fmtdesc.index++;
	}
	//////
	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(fd, VIDIOC_G_FMT, &fmt);
	printf("get fmt.fmt.width is %d\nfmt.fmt.pix.height is %d\nfmt.fmt.pix.colorspace is %d\n", fmt.fmt.pix.width, fmt.fmt.pix.height, fmt.fmt.pix.colorspace);

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = WIDTH_INIT;
	fmt.fmt.pix.height = HEIGTH_INIT;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
	if (-1 == ioctl(fd, VIDIOC_S_FMT, &fmt)) {
		printf("VIDIOC_S_FMT set error \n");
	}

	//////
	struct v4l2_requestbuffers req;
	req.count = 3;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if (-1 == ioctl(fd, VIDIOC_REQBUFS, &req)) {
		printf("VIDIOC_REQBUFS set error \n");
	}

	if (req.count < 2) {
		printf("Insufficient buffer memory \n");
		exit(EXIT_FAILURE);
	}

	unsigned int n_buffers = 0;
	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;
		if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
			printf("TK---------_>>>>>>error\n");
			close(fd);
			exit(-1);
		}
		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
		if (MAP_FAILED == buffers[n_buffers].start) {
			printf("TK--------__>>>>>error 2\n");
			close(fd);
			exit(-1);
		}
	}
	////
	unsigned int i;
	enum v4l2_buf_type type;
	for (i = 0; i < 3; i++) {
		struct v4l2_buffer buf;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) {
			printf("VIDIOC_QBUF set error = %d \n", i);
		}
	}
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == ioctl(fd, VIDIOC_STREAMON, &type)) {
		printf("VIDIOC_STREAMON set error \n");
	}

	return fd;
}






int set_gain_expose(int fd, int gain, int expose)
{
	static int old_gain, old_expose;

	if (gain == old_gain && expose == old_expose) {
		return 0;
	}
	old_expose = expose;
	old_gain = gain;
	printf("set gain = %d ,expose = %d \n", gain, expose);
	struct v4l2_control  Setting;

	Setting.id = V4L2_CID_EXPOSURE_AUTO;
	Setting.value = V4L2_EXPOSURE_MANUAL;
	int ret = ioctl(fd, VIDIOC_S_CTRL, &Setting);


	Setting.id = V4L2_CID_AUTOGAIN;
	Setting.value = 0;
	ret = ioctl(fd, VIDIOC_S_CTRL, &Setting);


	Setting.id = V4L2_CID_GAIN;
	Setting.value = gain;
	ret = ioctl(fd, VIDIOC_S_CTRL, &Setting);
	if (ret < 0) {
		printf("V4L2_CID_GAIN ret = %d \n", ret);
		return -1;
	}

	Setting.id = V4L2_CID_EXPOSURE;
	Setting.value = expose;
	ret = ioctl(fd, VIDIOC_S_CTRL, &Setting);
	if (ret < 0) {
		printf("V4L2_CID_EXPOSURE ret = %d \n", ret);
		return -2;
	}
	//remove three frame
	get_one_frame(fd);
	get_one_frame(fd);
	get_one_frame(fd);
}




void cap_deinit(CAP_FRAME * pcap)
{
	if (pcap) {
		if (pcap->useFlag) {
			if (-1 == ioctl(pcap->fd, VIDIOC_QBUF, &pcap->buf))
				printf("VIDIOC_QBUF error\n");
		}
		delete pcap;
	}
}



shared_ptr< CAP_FRAME> get_one_frame(int fd)
{
	shared_ptr< CAP_FRAME> ret(new CAP_FRAME, cap_deinit);

	struct v4l2_buffer &buf = ret->buf;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 == ioctl(fd, VIDIOC_DQBUF, &buf)) {
		printf("VIDIOC_DQBUF set error \n");
		return ret;
	}

	ret->fd = fd;
	ret->heigth = HEIGTH_INIT;
	ret->width = WIDTH_INIT;
	ret->startAddr = (unsigned char*)buffers[buf.index].start;
	ret->length = buf.length;
	ret->useFlag = 1;

	return ret;
}



#if 0 
int main()
{
	int fd = init_cap("/dev/video0");

	if (fd < 0) {
		cerr << "set video error" << endl;
		return -1;
	}
	set_gain_expose(fd, 10, 1000);

	for (int i = 0; i < 10; i++) {
		shared_ptr< CAP_FRAME> fram = get_one_frame(fd);

		if (fram && fram->useFlag) {
			char path[20];
			snprintf(path, sizeof(path), "test%d.jpg", i);
			SaveRGB565Jpg(path, fram->startAddr, fram->width, fram->heigth);
		}
	}
	////
	close(fd);
	return 0;
}
#endif