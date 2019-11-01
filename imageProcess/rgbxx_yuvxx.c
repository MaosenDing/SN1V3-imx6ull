
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
//#include"./camera.h"

//常见YUV数据格式定义
#define FMT420P_YU12   1    /*YUV420P YU12 -- YUV*/
#define FMT420P_YV12   2    /*YUV420P YV12 -- YVU*/
#define FMT420SP_NV12  3    /*YUV420SP NV12 -- Y-UV*/
#define FMT420SP_NV21  4    /*YUV420SP NV21 -- Y-VU*/
#define bool int
#define TRUE  0
#define FALSE -1
/*---------------------------内部调用-----------------------------*/
static void init_RGB_table(void);
static unsigned int RGB565ToRGB888(char pixrc, unsigned short n565Color);

//static bool ccvt_RGB888_to_YUV420p(char yuv_type, unsigned char* rgbbuffer, unsigned char* yuvbuffer, int width, int height, unsigned long int *len);
/*-----------------------------------------------------------------*/

/**
* 名称：RGB565ToRGB888(char pixrc, unsigned short n565Color)
* 功能： RGB565 数据转RGB888
* 入口参数： pixrc: 像素补偿   1：开启  0：关闭
*			 n565Color：RGB565像素值
* 出口参数： void
*           
* 返回值：n888Color像素值
*/

#define RGB565_RED      0xf800
#define RGB565_GREEN    0x07e0
#define RGB565_BLUE     0x001f


static inline unsigned int RGB565ToRGB888(char pixrc, unsigned short n565Color)
{
	unsigned int n888Color = 0;
	unsigned char bcRed = 0, bcGreen = 0, bcBlue = 0;   //补充使用
	// 获取RGB单色，并填充低位
	unsigned char cRed = (n565Color & RGB565_RED) >> 8;
	unsigned char cGreen = (n565Color & RGB565_GREEN) >> 3;
	unsigned char cBlue = (n565Color & RGB565_BLUE) << 3;
#if 1
//R&B交换
	unsigned char tmp = cRed;
	cRed = cBlue;
	cBlue = tmp;
#endif
	if (pixrc) {
		/*----------低位补充----------*/
		bcRed = cRed | ((cRed & 0x38) >> 3);
		bcGreen = cGreen | ((cGreen & 0x0c) >> 2);
		bcBlue = cBlue | ((cBlue & 0x38) >> 3);
		n888Color = (bcRed << 16) + (bcGreen << 8) + (bcBlue << 0);   //565——888 低位填充补偿
	}
	n888Color = (cRed << 16) + (cGreen << 8) + (cBlue << 0);   //565——888 低位填充补偿

	return n888Color;
}

//RGB888-->YUV420

#define COLORSIZE 256

static unsigned short Y_R[COLORSIZE];
static unsigned short Y_G[COLORSIZE];
static unsigned short Y_B[COLORSIZE];

static unsigned short U_R[COLORSIZE];
static unsigned short U_G[COLORSIZE];
static unsigned short U_B[COLORSIZE];

static unsigned short V_G[COLORSIZE];
static unsigned short V_B[COLORSIZE];

typedef struct RGB
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}RGB;


//RGB表
static void init_RGB_table(void)
{
	int i;

	for (i = 0; i < COLORSIZE; i++)
	{
		Y_R[i] = (i * 1224) >> 12; //Y对应的查表数组0.2988
		Y_G[i] = (i * 2404) >> 12;  //0.5869
		Y_B[i] = (i * 469) >> 12; //0.1162
		U_R[i] = (i * 692) >> 12; //U对应的查表数组0.1688
		U_G[i] = (i * 1356) >> 12;  //0.3312
		U_B[i] = i /*(* 2048) */ >> 1; //0.5
									   //   V_R[i] = (i * 2048) >> 12; ///V对应的查表数组
		V_G[i] = (i * 1731) >> 12;  //0.4184
		V_B[i] = (i * 334) >> 12; //0.0816
	}
}

bool ccvt_RGB888_to_YUV420p(char yuv_type, unsigned char* rgbbuffer, unsigned char* yuvbuffer, int width, int height, unsigned long *len)
{
	int x, y;
	int pix = 0, pixP4 = 0;

	RGB *rgbBufIn = (RGB *)rgbbuffer;

	unsigned long int IMGSIZE = width * height;   //像素大小

	if (NULL == rgbbuffer) return -1;   //输入数据错误

	init_RGB_table();

	for (y = 0; y < height; y++)   //line
	{
		for (x = 0;x < width; x++) //pixf
		{

			RGB rgbByte = rgbBufIn[pix]; //
//			RGB rgbByte = rgbBufIn[(height - y - 1)*width + x]; //取得垂直方向上镜像的位置即可解决倒立问题

			//首先执行颜色互换 -- 没有这个的话  得到的YUV图像颜色不对
			unsigned char temp = rgbByte.r; //顺序调整
			rgbByte.r = rgbByte.b;
			rgbByte.b = temp;

			int i = Y_R[rgbByte.r] + Y_G[rgbByte.g] + Y_B[rgbByte.b];
			yuvbuffer[pix] = i; //YUV输出

			if ((x % 2 == 1) && (y % 2 == 1))
			{
				pixP4 = (width >> 1) *(y >> 1) + (x >> 1);
				i = U_B[rgbByte.b] - U_R[rgbByte.r] - U_G[rgbByte.g] + 128;

				yuvbuffer[pixP4 + IMGSIZE] = i;
				/*+  U_B[in[pix+1].b] - U_R[in[pix+1].r] - U_G[in[pix+1].g]
				+U_B[in[pix+XSIZE].b] - U_R[in[pix+XSIZE].r] - U_G[in[pix+XSIZE].g]
				+U_B[in[pix+1+XSIZE].b] - U_R[in[pix+1+XSIZE].r] - U_G[in[pix+1+XSIZE].g] )/4*/
				//U
				i = U_B[rgbByte.r] - V_G[rgbByte.g] - V_B[rgbByte.b] + 128;

				yuvbuffer[pixP4 + 5 * IMGSIZE / 4] = i;
				/*+U_B[in[pix+1].r] - V_G[in[pix+1].g] - V_B[in[pix+1].b]
				+U_B[in[pix+XSIZE].r] - V_G[in[pix+XSIZE].g] - V_B[in[pix+XSIZE].b]
				+U_B[in[pix+1+XSIZE].r] - V_G[in[pix+1+XSIZE].g] - V_B[in[pix+1+XSIZE].b])/4*/
				//V
			}
			pix++;
		}
	}
	*len = width * height + (width * height) / 2;
	return TRUE;
}

typedef union
{
	struct
	{
		unsigned char low_byte;
		unsigned char mlow_byte;
		unsigned char mhigh_byte;
		unsigned char high_byte;
	}byte_value;
	struct
	{
		unsigned short low_byte;
		unsigned short high_byte;
	}short_value;
	unsigned int  value;
}MY_UINT;
//RGB565转rgb888
bool rgb565_to_rgb888(unsigned char * psrc, unsigned char * pdst, int width, int height, unsigned long *len)
{
	unsigned long int loop = width * height;     //像素个数

	MY_UINT rgb888buf;

	unsigned long int i;
	unsigned short *prgb565 = (unsigned short *)psrc;
	rgb888buf.value = 0;

	for (i = 0; i < loop; i++)
	{
		rgb888buf.value = RGB565ToRGB888(0, prgb565[i]);

		*pdst++ = rgb888buf.byte_value.low_byte;
		*pdst++ = rgb888buf.byte_value.mlow_byte;
		*pdst++ = rgb888buf.byte_value.mhigh_byte;
	}

	*len = width * height * 3;    //rgb888

	return TRUE;
}

#if 0

#define TUNE(r) ( r < 0 ? 0 : (r > 255 ? 255 : r) )

static  int YUVY_R[256];
static  int YUVY_G[256];
static  int YUVY_B[256];

static  int YUVU_R[256];
static  int YUVU_G[256];
static  int YUVU_B[256];

static int YUVV_R[256];
static  int YUVV_G[256];
static  int YUVV_B[256];

bool RGB888_YUV422(unsigned char *pRGB, unsigned char *pYUV, int width, int height)
{
	unsigned char r, g, b, u, v, u1, v1, r1, g1, b1;
	int loop = size / 2;
	int i;

	for (i = 0; i<loop; i++)
	{
		r = *pRGB; pRGB++;
		g = *pRGB; pRGB++;
		b = *pRGB; pRGB++;
		r1 = *pRGB; pRGB++;
		g1 = *pRGB; pRGB++;
		b1 = *pRGB; pRGB++;

		//new method ---  right 
		int y = ((YUVY_R[r] + YUVY_G[g] + YUVY_B[b] + 128) >> 8) + 16;
		u = ((YUVU_R[r] + YUVU_G[g] + YUVU_B[b] + 128) >> 8) + 128;
		v = ((YUVV_R[r] + YUVV_G[g] + YUVV_B[b] + 128) >> 8) + 128;

		int y1 = ((YUVY_R[r1] + YUVY_G[g1] + YUVY_B[b1] + 128) >> 8) + 16;
		u1 = ((YUVU_R[r1] + YUVU_G[g1] + YUVU_B[b1] + 128) >> 8) + 128;
		v1 = ((YUVV_R[r1] + YUVV_G[g1] + YUVV_B[b1] + 128) >> 8) + 128;

		*pYUV++ = TUNE(y);
		*pYUV++ = (TUNE(u) + TUNE(u1)) >> 1;
		*pYUV++ = TUNE(y1);
		*pYUV++ = TUNE(v);
	}
	return 0;
}
#endif