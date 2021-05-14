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

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/ioctl.h>
//#include <sys/mman.h>
//#include <fcntl.h>
//#include <errno.h>
//#include <linux/videodev2.h>
//#include "video.h"


#define SET_GAIN_EXPOSURE_IN_MODE 1


using namespace std;
#define UXGA_WIDTH (2592)
#define UXGA_HEIGHT (1944)

#define VGA_WIDTH (640)
#define VGA_HEIGHT (480)

#if 0
	#define WIDTH_INIT VGA_WIDTH
	#define HEIGTH_INIT VGA_HEIGHT
#else
	#define WIDTH_INIT UXGA_WIDTH
	#define HEIGTH_INIT UXGA_HEIGHT
#endif

/****************************************************/

#define CAMERA_DEVICE "/dev/video1"
#define CAMERA_PATH "user/camera"
#define JPEG_PATH "test.jpg"

static struct image_mode_t {
	const char* name;
	unsigned short width;
	unsigned short height;
	unsigned char  mode;
} smap[] = {
	{"QCIF", 176, 144, 7},
	{"QVGA", 320, 240, 1},
	{"VGA",  640, 480, 0},
	{"NTSC", 720, 480, 2},
	{"PAL",  720, 576, 3},
	{"XGA",  1024,768, 8},
	{"720P", 1280,720, 4},
	{"1080P",1920,1080,5},
	{"QSXGA",2592,1944,6},
};

video_context_t g_video_context;


/****************************************************/


struct buffer {
	void *start;
	unsigned int length;
};
#define REQ_BUFF_NUM (2)
buffer buffers[REQ_BUFF_NUM];

int init_cap(const char * videoName)
{
	/********打开设备**********/
	int fd = open(videoName, O_RDWR);
	printf("TK------->>>fd is %d\n", fd);
	/********获取驱动信息**********/
	struct v4l2_capability cap;
	ioctl(fd, VIDIOC_QUERYCAP, &cap);
	printf("TK---------->>>>>Driver Name:%s\nCard Name:%s\nBus info:%s\n", cap.driver, cap.card, cap.bus_info);

	printf("Driver Name : %s\nCard Name : %s\nBus info : %s\nDriver Version : %u.%u.%u\n", cap.driver, cap.card, cap.bus_info, (cap.version >> 16) & 0XFF, (cap.version >> 8) & 0XFF, cap.version & 0XFF);
	/********获取当前设备支持的视频格式**********/
	struct v4l2_fmtdesc fmtdesc;
	memset(&fmtdesc, 0, sizeof(fmtdesc));
	fmtdesc.index = 0; 
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1) {
		printf("pixelformat is '%c%c%c%c', description is '%s' \n", fmtdesc.pixelformat & 0xFF,
			(fmtdesc.pixelformat >> 8) & 0xFF, (fmtdesc.pixelformat >> 16) & 0xFF,
			(fmtdesc.pixelformat >> 24) & 0xFF, fmtdesc.description);
		fmtdesc.index++;
	}
	/********设置视频/图片格式**********/
	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(0 == ioctl(fd, VIDIOC_G_FMT, &fmt))
	printf("get fmt.fmt.width is %d\nfmt.fmt.pix.height is %d\nfmt.fmt.pix.colorspace is %d\n", fmt.fmt.pix.width, fmt.fmt.pix.height, fmt.fmt.pix.colorspace);
	else {
		printf("VIDIOC_G_FMT V4L2_BUF_TYPE_VIDEO_CAPTURE error=%d\n",errno);
	}


	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = WIDTH_INIT;
	fmt.fmt.pix.height = HEIGTH_INIT;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	//fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
	if (-1 == ioctl(fd, VIDIOC_S_FMT, &fmt)) {
		printf("VIDIOC_S_FMT set error \n");
	}

	/********请求分配内存**********/
	struct v4l2_requestbuffers req;
	req.count = REQ_BUFF_NUM;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if (-1 == ioctl(fd, VIDIOC_REQBUFS, &req)) {
		printf("VIDIOC_REQBUFS set error \n");
	}

	//if (req.count < 2) {
	//	printf("Insufficient buffer memory \n");
	//	exit(EXIT_FAILURE);
	//}
	/********获取空间，并将其映射到用户空间，然后投放到视频输入队列**********/
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
	/********投放一个空的视频缓冲区到视频缓冲区输入队列中**********/
	unsigned int i;
	enum v4l2_buf_type type;
	for (i = 0; i < REQ_BUFF_NUM; i++) {
		struct v4l2_buffer buf;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) {
			printf("VIDIOC_QBUF set error = %d \n", i);
		}
	}
	/********开始录制*********/
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == ioctl(fd, VIDIOC_STREAMON, &type)) {
		printf("VIDIOC_STREAMON set error \n");
	}

	return fd;
}


int set_gain_expose(int fd, unsigned int gain, unsigned int expose)
{
	static unsigned int old_gain, old_expose;

	if (gain == old_gain && expose == old_expose) {
		return 0;
	}
	old_expose = expose;
	old_gain = gain;
	printf("set gain = %d ,expose = %d \n", gain, expose);

/**********
	struct v4l2_control  Setting;

	Setting.id = V4L2_CID_EXPOSURE_AUTO;
	Setting.value = V4L2_EXPOSURE_MANUAL;
	if (0 > ioctl(fd, VIDIOC_S_CTRL, &Setting)) {
		printf("V4L2_CID_EXPOSURE_AUTO error = %d \n", errno);
	}

	Setting.id = V4L2_CID_EXPOSURE_ABSOLUTE;
	Setting.value = expose;
	if (0 > ioctl(fd, VIDIOC_S_CTRL, &Setting)) {
		printf("V4L2_CID_EXPOSURE error = %d \n", errno);
	}

	Setting.id = V4L2_CID_GAIN;
	Setting.value = gain;
	if (0 > ioctl(fd, VIDIOC_S_CTRL, &Setting)) {
		printf("V4L2_CID_GAIN error = %d \n", errno);
	}
******/
	//摄像头重新初始化并设置增益和曝光
//	camera_init(gain, expose);

	//remove three frame
#if 0
	get_one_frame(fd);
	get_one_frame(fd);
	get_one_frame(fd);
#endif
	return 0;
}



void cap_deinit(CAP_FRAME * pcap)
{
	if (pcap) {
//		if (pcap->useFlag) {
//			//if (-1 == ioctl(pcap->fd, VIDIOC_QBUF, &pcap->buf))
//			if (-1 == ioctl(g_video_context.fd, VIDIOC_QBUF, &pcap->buf))
//				printf("VIDIOC_QBUF error\n");
//		}
		delete pcap;
	}
}


/***从视频缓冲区的输出队列中取得一个已经保存有一帧视频数据的视频缓冲区***/
/************控制命令VIDIOC_DQBUF*****************/
struct v4l2_buffer buf;
shared_ptr< CAP_FRAME> get_one_frame(int fd)
//CAP_FRAME get_one_frame(int fd)
{
	shared_ptr< CAP_FRAME> ret(new CAP_FRAME, cap_deinit);

	//struct v4l2_buffer &buf = ret->buf;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	#if 1
//	if(ret->useFlag)
	//printf("get one frame\n");
	if(g_video_context.use_flag)
	{
		if(-1 == ioctl(g_video_context.fd, VIDIOC_QBUF, &buf))
			printf("VIDIOC_QBUF error \n");
	}
	#endif	
	
	if (-1 == ioctl(g_video_context.fd, VIDIOC_DQBUF, &buf)) {
		printf("VIDIOC_DQBUF set error \n");
		return ret;
	}else
	{
		printf("v4l2 DQBUF process\n");
	}
	g_video_context.frameindex = buf.index;
	g_video_context.use_flag = true;
	//g_video_context.frameindex = 0;
	printf("frameindex = %d \n", g_video_context.frameindex);

	#if 1
	ret->fd = fd;
	ret->heigth = HEIGTH_INIT;
	ret->width = WIDTH_INIT;
	//ret->startAddr = (unsigned char*)buffers[buf.index].start;
	ret->startAddr = (unsigned char*)g_video_context.framebuf[g_video_context.frameindex].start;
	//ret->length = buf.length;
	ret->length = buf.bytesused;
	ret->useFlag = 1;
	#endif

	/************同步g_video_context的参数变量****************/

	/****************************/
	
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
		//CAP_FRAME fram = get_one_frame(fd);
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

/********************OV5640内容*********************/
//int my_cap_init(unsigned int gain, unsigned int expo, int isHorFlip, int isVerFlip)
//int camera_init(unsigned int gain, unsigned int expo);
static void video_match_mode(void);
static int v4l2_init_DVP(void);

static int t_video_fd = -1;
int camera_init(unsigned int gain, unsigned int expo, unsigned int overturn)
{
	printf("camera init\n");

 	g_video_context.devName = strdup(CAMERA_DEVICE);	
	g_video_context.jpgName = strdup(JPEG_PATH);	
	
	//#define WIDTH_INIT UXGA_WIDTH
	//#define HEIGTH_INIT UXGA_HEIGHT

	//g_video_context.width = tg_table.T6.SN1_P2; //1920
	//g_video_context.height = tg_table.T6.SN1_P1; //1080
	g_video_context.width = WIDTH_INIT; //2592
	g_video_context.height = HEIGTH_INIT; //1944
	g_video_context.fd = 0;
	g_video_context.image_mode = -1;
	g_video_context.image_overturn = overturn;

	g_video_context.gain = gain;
	g_video_context.exposure = expo;

	g_video_context.use_flag = false;

	printf("gain = %d, exposure=%d\n",g_video_context.gain, g_video_context.exposure);

	//YUV422转化为Gray保存数据的地址（下面是为该地址保存的数据开辟空间）
	g_video_context.graybuff = (unsigned char*)malloc(g_video_context.height*g_video_context.width);

	if(g_video_context.graybuff == NULL)
	{
		printf("video_context alloc gray buff err\n");
		exit(1);
	}

	printf("camera type 1\n");
	video_match_mode();
	
	//设备上使用的是DVP接口的摄像头
	t_video_fd = v4l2_init_DVP();

	if (t_video_fd < 0) {
		cerr << "set video error" << endl;
		return -1;
	}
	//set_gain_expose(video_fd, gain, expo);

	if (t_video_fd >= 0) {
		return 0;
	}
	return -1;
}

static void video_match_mode(void)
{
	//if(g_video_context)
	//{
		for(std::size_t i = 0; i<sizeof(smap)/sizeof(smap[0]); i++)
		{
			if(smap[i].width == g_video_context.width && smap[i].height == g_video_context.height)
			{
				g_video_context.image_mode = smap[i].mode;
				printf("image mode %s, image mode %d \n", smap[i].name, g_video_context.image_mode);
				break;
			}
		}
	//	}
}

static int v4l2_init_DVP(void)
{
	struct v4l2_capability cap;
	struct v4l2_fmtdesc fmtdesc;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers reqbuf;
	struct v4l2_buffer buf;
	struct v4l2_streamparm parms; 
	enum v4l2_buf_type type ;
	int r;

	//if(&video_context) return -1;

	// check the capture size; 
	if(g_video_context.width < 1 || g_video_context.height < 1) {
		fprintf(stderr,"invalid video context with width=%d height=%d\n",g_video_context.width,g_video_context.height);
		return -1;
	}

	if ((g_video_context.fd = open(g_video_context.devName, O_RDWR, 0)) < 0) {
		printf("Open %s failed\n", g_video_context.devName);
		return -1;
	}

	if((r = ioctl(g_video_context.fd, VIDIOC_QUERYCAP, &cap)) < 0) {
		printf("VIDIOC_QUERYCAP failed (%d)\n", r);
		return r;
	}

	// Print capability infomations
	printf("------------VIDIOC_QUERYCAP-----------\n");
	printf("Capability Informations:\n");
	printf(" driver: %s\n", cap.driver);
	printf(" card: %s\n", cap.card);
	printf(" bus_info: %s\n", cap.bus_info);
	printf(" version: %08X\n", cap.version);
	printf(" capabilities: %08X\n\n", cap.capabilities);

	// check the video format supported;
	printf("get the format what the device support\n");
	memset(&fmtdesc,0,sizeof(struct v4l2_fmtdesc)); 
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	for(fmtdesc.index = 0;;fmtdesc.index++) {
		if(ioctl(g_video_context.fd, VIDIOC_ENUM_FMT, &fmtdesc) < 0) break; 

		printf("[%d] { pixelformat = ''%c%c%c%c'', description = ''%s'' }\n", fmtdesc.index,
				fmtdesc.pixelformat & 0xFF, (fmtdesc.pixelformat >> 8) & 0xFF, (fmtdesc.pixelformat >> 16) & 0xFF, (fmtdesc.pixelformat >> 24) & 0xFF, 
				fmtdesc.description);

		switch(fmtdesc.pixelformat) {
			case V4L2_PIX_FMT_YUYV:
				break;
			default:
				printf("The video format is not supported by camera!\n");
				return -1;
		}
	}

	//设置增益和曝光
	struct v4l2_control ctrl;
	// set the gain param; it must be done after the VIDIOC_S_FMT;
	ctrl.id = V4L2_CID_EXPOSURE_AUTO;
	ctrl.value = V4L2_EXPOSURE_MANUAL;
	if ((r = ioctl(g_video_context.fd, VIDIOC_S_CTRL, &ctrl)) < 0) {
		printf("VIDIOC_S_CTRL - V4L2_CID_EXPOSURE_AUTO failed (%d)\n",r);
		return r; 
	}

	ctrl.id = V4L2_CID_EXPOSURE;
	ctrl.value = g_video_context.exposure; 	//曝光
	if ((r = ioctl(g_video_context.fd, VIDIOC_S_CTRL, &ctrl)) < 0) {
		printf("VIDIOC_S_CTRL - V4L2_CID_EXPOSURE failed (%d) \n", r);
		return r;
	}
	fprintf(stderr,"Set the exposure to %d \n",g_video_context.exposure); 

	ctrl.id= V4L2_CID_GAIN;
	ctrl.value = g_video_context.gain;		//增益
	if((r = ioctl(g_video_context.fd,VIDIOC_S_CTRL,&ctrl)) < 0) {
		printf("VIDIOC_S_CTRL failed (%d)\n", r);
		return r;
	}
	fprintf(stderr,"Set the gain to %d \n",g_video_context.gain); 

	// set the capture stream params; /********设置视频/图片格式**********/
	memset(&parms,0,sizeof(struct v4l2_streamparm));
	parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	parms.parm.capture.capturemode = g_video_context.image_mode;
	parms.parm.capture.reserved[0] = g_video_context.image_overturn;
	parms.parm.capture.timeperframe.denominator = 15;
	parms.parm.capture.timeperframe.numerator = 1;
	if((r = ioctl(g_video_context.fd, VIDIOC_S_PARM,&parms)) < 0) {
		printf("VIDIOC_S_PARM failed (%d)\n", r);
		return r;
	}

	// set the capture video format; 
	memset(&fmt,0,sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = g_video_context.width;
	fmt.fmt.pix.height = g_video_context.height;
	//fmt.fmt.pix.pixelformat = fmtdesc.pixelformat;//V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;//V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
	if((r = ioctl(g_video_context.fd, VIDIOC_S_FMT, &fmt)) < 0) {
		printf("VIDIOC_S_FMT failed (%d)\n", r);
		return r;
	}

	// re-get and check it;
	memset(&fmt,0,sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if((r = ioctl(g_video_context.fd, VIDIOC_G_FMT, &fmt)) < 0) {
		printf("VIDIOC_G_FMT failed (%d)\n", r);
		return r;
	}
	// Print Stream Format
	printf("------------VIDIOC_S_FMT---------------\n");
	printf("Stream Format Informations:\n");
	printf(" type: %d\n", fmt.type);
	printf(" width: %d\n", fmt.fmt.pix.width);
	printf(" height: %d\n", fmt.fmt.pix.height);

	printf(" pixelformat: 0x%x\n", fmt.fmt.pix.pixelformat);
	printf(" field: %d\n", fmt.fmt.pix.field);
	printf(" bytesperline: %d\n", fmt.fmt.pix.bytesperline);
	printf(" sizeimage: %d\n", fmt.fmt.pix.sizeimage);
	printf(" colorspace: %d\n", fmt.fmt.pix.colorspace);
	printf(" priv: %d\n", fmt.fmt.pix.priv);
	//printf(" raw_date: 0x%x\n", fmt.fmt.raw_data);
    
	/********请求分配内存**********/
	// request the buffer; 
	memset(&reqbuf,0,sizeof(struct v4l2_requestbuffers));
	reqbuf.count = BUFFER_COUNT;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf.memory = V4L2_MEMORY_MMAP;

	if((r = ioctl(g_video_context.fd, VIDIOC_REQBUFS, &reqbuf)) < 0) {
		printf("VIDIOC_REQBUFS failed (%d)\n", r);
		return r;
	}
	printf("the buffer has been assigned successfully!\n");
	/********获取空间，并将其映射到用户空间，然后投放到视频输入队列**********/
	for (std::size_t i = 0; i < reqbuf.count && i < BUFFER_COUNT; i++)
	{
		buf.index = i;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		if((r = ioctl(g_video_context.fd, VIDIOC_QUERYBUF, &buf)) < 0) {//buf取得内存缓冲区的信息
			printf("VIDIOC_QUERYBUF (%d) failed (%d)\n", i, r);
			return r;
		}

		// mmap buffer/********投放一个空的视频缓冲区到视频缓冲区输入队列中**/
		g_video_context.framebuf[i].length = buf.length;
		g_video_context.framebuf[i].start = (char*)mmap(0, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, g_video_context.fd, buf.m.offset);
		if (g_video_context.framebuf[i].start == MAP_FAILED) {
			printf("mmap (%d) failed: %s\n", i, strerror(errno));
			return -1;
		}
		#if 1
		// Queen buffer
		if((r= ioctl(g_video_context.fd, VIDIOC_QBUF, &buf)) < 0) {
			printf("VIDIOC_QBUF (%d) failed (%d)\n", i, r);
			return r;
		}
		#endif
		printf("Frame buffer %d: address=0x%x, length=%d\n", i, (unsigned int)g_video_context.framebuf[i].start,g_video_context.framebuf[i].length);
	}

	// start the capture/********开始录制*********/
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if((r = ioctl(g_video_context.fd, VIDIOC_STREAMON, &type)) < 0) {
		printf("VIDIOC_STREAMON failed (%d)\n", r);
		return r;
	}
#if 0
	// Get frame
	if((r = ioctl(g_video_context.fd, VIDIOC_DQBUF, &buf)) < 0) {//VIDIOC_DQBUF命令结果, 使从队列删除的缓冲帧信息传给了此buf
		printf("VIDIOC_DQBUF failed (%d)\n", r);
		return r;
	}
	g_video_context.frameindex = buf.index;
#endif
	return g_video_context.fd;
}
