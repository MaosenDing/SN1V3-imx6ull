#include "SN1V2_com.h"
#include <algorithm>
#include <stdint.h>
#include <iostream>
#include "time_interval.h"
#include "errHandle/errHandle.h"
#include <arm_neon.h>
using namespace std;



ERR_STA RGB565GRAY(unsigned char * LoadImg, int inputSize, shared_ptr<vector<uint8_t> > & outdata)
{
	int pixCount = inputSize / 2;
	try
	{
		outdata->resize(pixCount);
	}
	catch (std::bad_alloc & testbad)
	{
		SN1V2_ERROR_CODE_RET(err_out_of_memory);
	}

	{
		TIME_INTERVAL_SCOPE("rgb gray operator:");
		RGB565GRAY((uint16_t*)LoadImg, &(*outdata)[0], pixCount);
	}
	return err_ok;
}

void RGB565GRAY(unsigned char * LoadImg, int inputSize)
{
	int pixCount = inputSize / 2;
	TIME_INTERVAL_SCOPE("rgb gray operator:");
	RGB565GRAY((uint16_t*)LoadImg, LoadImg, pixCount);
}


void RGB888_2_565(uint8_t * srcdata, uint8_t *dst, size_t pixCount)
{
	const unsigned int RGB565_RED = 0xf800;
	const unsigned int RGB565_GREEN = 0x07e0;
	const unsigned int RGB565_BLUE = 0x001f;

	uint16_t *dst16 = (uint16_t*)dst;
	pixCount = pixCount & (~127);
	while (pixCount--) {
		*dst16 = ((srcdata[0] >> 3) & RGB565_BLUE) |
			((srcdata[1] << 3) & RGB565_GREEN) |
			((srcdata[2] << 8) & RGB565_RED);
		
		dst16 += 1;
		srcdata += 3;
	}
}


//#ifndef CORTEX
#if 0
void RGB565GRAY(uint16_t * srcdata, uint8_t *dst, size_t pixCount)
{
	const unsigned int RGB565_RED = 0xf800;
	const unsigned int RGB565_GREEN = 0x07e0;
	const unsigned int RGB565_BLUE = 0x001f;
	const unsigned int ROUND_NUM = 50;
	if (pixCount % 4 == 0)
	{
		printf("8888888----------------------------\n");
		uint16_t * endp = &srcdata[pixCount];
		
		while(srcdata < endp)
		{
			uint8_t *dst_1 = dst;
			uint8_t *dst_2 = dst + 1;
			uint8_t *dst_3 = dst + 2;
			uint8_t *dst_4 = dst + 3;



			uint16_t rgb565_1 = srcdata[0];
			int cRed_1 = ((rgb565_1 & RGB565_RED) >> 8);
			int cGreen_1 = ((rgb565_1 & RGB565_GREEN) >> 3);
			int cBlue_1 = ((rgb565_1 & RGB565_BLUE) << 3);
			*dst_1 = (cRed_1 * 299 + cGreen_1 * 587 + cBlue_1 * 114 + ROUND_NUM) / 1000;

			uint16_t rgb565_2 = srcdata[1];
			int cRed_2 = ((rgb565_2 & RGB565_RED) >> 8);
			int cGreen_2 = ((rgb565_2 & RGB565_GREEN) >> 3);
			int cBlue_2 = ((rgb565_2 & RGB565_BLUE) << 3);
			*dst_2 = (cRed_2 * 299 + cGreen_2 * 587 + cBlue_2 * 114 + ROUND_NUM) / 1000;


			uint16_t rgb565_3 = srcdata[2];
			int cRed_3 = ((rgb565_3 & RGB565_RED) >> 8);
			int cGreen_3 = ((rgb565_3 & RGB565_GREEN) >> 3);
			int cBlue_3 = ((rgb565_3 & RGB565_BLUE) << 3);
			*dst_3 = (cRed_3 * 299 + cGreen_3 * 587 + cBlue_3 * 114 + ROUND_NUM) / 1000;


			uint16_t rgb565_4 = srcdata[3];
			int cRed_4 = ((rgb565_4 & RGB565_RED) >> 8);
			int cGreen_4 = ((rgb565_4 & RGB565_GREEN) >> 3);
			int cBlue_4 = ((rgb565_4 & RGB565_BLUE) << 3);
			*dst_4 = (cRed_4 * 299 + cGreen_4 * 587 + cBlue_4 * 114 + ROUND_NUM) / 1000;



			srcdata += 4;
			dst += 4;
		}
	}
	else
	{
		for (size_t pos = 0; pos < pixCount; pos++)
		{
			uint16_t rgb565 = srcdata[pos];

			int cRed = ((rgb565 & RGB565_RED) >> 8);
			int cGreen = ((rgb565 & RGB565_GREEN) >> 3);
			int cBlue = ((rgb565 & RGB565_BLUE) << 3);

			dst[pos] = (cRed * 299 + cGreen * 587 + cBlue * 114 + 50) / 1000;
		}
	}
}
#else

#if 0
void RGB565GRAY(uint16_t * srcdata, uint8_t *dst, size_t pixCount)
{
	//pixCount = 8;
	const uint16x8_t RGB565_RED = vdupq_n_u16(0xf800);
	const uint16x8_t RGB565_GREEN = vdupq_n_u16(0x07e0);
	const uint16x8_t RGB565_BLUE = vdupq_n_u16(0x001f);
	const uint16x8_t ROUND_NUM = vdupq_n_u16(50 / 8);

	const uint16x8_t const_299 = vdupq_n_u16(299/8);
	const uint16x8_t const_587 = vdupq_n_u16(587/8);
	const uint16x8_t const_114 = vdupq_n_u16(114/8);

	uint16_t * endp = &srcdata[pixCount];

	while (srcdata < endp) {
		__asm__ __volatile__(
			"pld [%0,#32] \n"
			:
		: "r" (srcdata));
		uint16x8_t rgb565_0 = vld1q_u16(srcdata);
		uint16x8_t tmpr0 = vandq_u16(rgb565_0, RGB565_RED);
		uint16x8_t tmpg0 = vandq_u16(rgb565_0, RGB565_GREEN);
		uint16x8_t tmpb0 = vandq_u16(rgb565_0, RGB565_BLUE);

		tmpr0 = vshrq_n_u16(tmpr0, 8);
		tmpg0 = vshrq_n_u16(tmpg0, 3);
		tmpb0 = vshlq_n_u16(tmpb0, 3);

		uint16x8_t rgb565_1 = vld1q_u16(srcdata + 8);
		uint16x8_t tmpr1 = vandq_u16(rgb565_1, RGB565_RED);
		uint16x8_t tmpg1 = vandq_u16(rgb565_1, RGB565_GREEN);
		uint16x8_t tmpb1 = vandq_u16(rgb565_1, RGB565_BLUE);

		tmpr1 = vshrq_n_u16(tmpr1, 8);
		tmpg1 = vshrq_n_u16(tmpg1, 3);
		tmpb1 = vshlq_n_u16(tmpb1, 3);


		uint16x8_t ans0 = vmulq_u16(tmpr0, const_299);
		ans0 = vmlaq_u16(ans0, tmpg0, const_587);
		ans0 = vmlaq_u16(ans0, tmpb0, const_114);
		ans0 = vaddq_u16(ans0, ROUND_NUM);
		uint8x8_t ret0 = vshrn_n_u16(ans0, 10 - 3);
		vst1_u8(dst, ret0);


		uint16x8_t ans1 = vmulq_u16(tmpr1, const_299);
		ans1 = vmlaq_u16(ans1, tmpg1, const_587);
		ans1 = vmlaq_u16(ans1, tmpb1, const_114);
		ans1 = vaddq_u16(ans1, ROUND_NUM);
		uint8x8_t ret1 = vshrn_n_u16(ans1, 10 - 3);
		vst1_u8(dst + 8, ret1);

		dst += 16;
		srcdata += 16;
	}
}
#else
void RGB565GRAY(uint16_t * srcdata, uint8_t *dst, size_t pixCount)
{
	for (size_t i = 0; i < (pixCount & ~127); i += 8) {
#define COLOR 3//r g b

#if COLOR == 1
		const uint8x8_t mask_3 = vdup_n_u8(~(8 - 1));
		uint8x8x2_t  rgb565 = vld2_u8((uint8_t *)(srcdata + i));	
		uint8x8_t btun = vand_u8(rgb565.val[1], mask_3);
		vst1_u8((dst + i), btun);
#elif COLOR == 2
		const uint8x8_t mask_2 = vdup_n_u8(~(4 - 1));
		uint16x8_t  rgb565 = vld1q_u16((uint16_t *)(srcdata + i));
		uint8x8_t btun = vshrn_n_u16(rgb565, 3);
		btun = vand_u8(btun, mask_2);
		vst1_u8((dst + i), btun);
#elif COLOR == 3
		uint8x8x2_t  rgb565 = vld2_u8((uint8_t *)(srcdata + i));
		uint8x8_t btun = vshl_n_u8(rgb565.val[0], 3);
		vst1_u8((dst + i), btun);
#endif
	}
}

#endif

#endif

void neon_test(uint8_t * srcdata, uint8_t *dst, size_t pixCount)
{
	uint8x8x2_t dat = vld2_u8((uint8_t *)srcdata);

	uint8x8x2_t ret = vtrn_u8(dat.val[0], dat.val[1]);
	
	vst2_u8(dst, ret);
}


//#ifndef CORTEX
#if 0
void fastbinaryzation(unsigned char * src, int thres, int insize)
{
	if (insize % 4 == 0)
	{
		//std::cout << "test 4" << std::endl;
		unsigned char * endp = src + insize;
		while (src < endp)
		{
			unsigned char * p1 = &src[0];
			unsigned char * p2 = &src[1];
			unsigned char * p3 = &src[2];
			unsigned char * p4 = &src[3];


			if (*p1 > thres)
			{
				*p1 = 255;
			}
			else
			{
				*p1 = 0;
			}

			if (*p2 > thres)
			{
				*p2 = 255;
			}
			else
			{
				*p2 = 0;
			}

			if (*p3 > thres)
			{
				*p3 = 255;
			}
			else
			{
				*p3 = 0;
			}

			if (*p4 > thres)
			{
				*p4 = 255;
			}
			else
			{
				*p4 = 0;
			}
			src += 4;
		}
	}
	else
	{
		//std::cout << "4 size error" << std::endl;
		unsigned char * endp = src + insize;
		while (src < endp)
		{
			unsigned char * p1 = &src[0];

			if (*p1 > thres)
			{
				*p1 = 255;
			}
			else
			{
				*p1 = 0;
			}
			src += 1;
		}
	}
}
#else
void fastbinaryzation(unsigned char * src, int thres, int insize)
{
#if 1
	uint8x16_t comp = vdupq_n_u8(thres);
	insize /= 32;
	while (insize--) {

		uint8x16_t dat = vld1q_u8(src);
		uint8x16_t dat2 = vld1q_u8(src + 16);

		dat = vcgtq_u8(dat, comp);
		dat2 = vcgtq_u8(dat2, comp);

		vst1q_u8(src, dat);
		vst1q_u8(src + 16, dat2);

		src += 32;
	}
#else
	insize /= 32;
	asm  volatile(
		"vdup.u8 q0,%1 \n \t"

		"myloop: \n \t"
		"pld [%0,#32] \n \t"
		"vld1.8 {d4,d5,d6,d7} ,[%0] \n \t "

		"vcgt.u8 q2,q2,q0 \n \t"
		"vcgt.u8 q3,q3,q0 \n \t"

		"vst1.8 {d4,d5,d6,d7},[%0] \n \t"

		"add %0,%0,#32 \n\t"
		"sub %2,%2,#1 \n\t"
		"cmp %2,#0 \n\t"
		"bne myloop \n\t"
		:
	: "r"(src), "r"(thres), "r"(insize)
		: "q0" ,"q2" , "q3"
		);
#endif
}
#endif


//#ifndef CORTEX
#if 0
unsigned int getMaxVal(unsigned char * src,size_t sz)
{
	unsigned char * max;
	{
		TIME_INTERVAL_SCOPE("get max :");
		max = max_element(src, src + sz);
	}
#if 0
	cout << "get max gray =" << (int)*max << endl;
#endif
	return *max;
}


#else
unsigned int getMaxVal(unsigned char * src, size_t num)
{
	TIME_INTERVAL_SCOPE("get max :");

	unsigned char max = 0;
#if 1
	for (size_t i = 0; i < (num & (~(256 - 1))); i++) {
		if (src[i] > max) {
			max = src[i];
		}
	}
#else
	num /= 64;
	uint8x16_t dat0 = vld1q_u8(src);
	uint8x16_t dat1 = vld1q_u8(src+16);
	uint8x16_t dat2 = vld1q_u8(src+32);
	uint8x16_t dat3 = vld1q_u8(src+48);
	num--;
	src += 64;

	while (num--) {
		__asm__ __volatile__(
			"pld [%0,#64] \n"
			:
		: "r" (src));


		uint8x16_t dat4 = vld1q_u8(src);
		uint8x16_t dat5 = vld1q_u8(src + 16);
		uint8x16_t dat6 = vld1q_u8(src + 32);
		uint8x16_t dat7 = vld1q_u8(src + 48);

		dat0 = vmaxq_u8(dat0, dat4);
		dat1 = vmaxq_u8(dat1, dat5);
		dat2 = vmaxq_u8(dat2, dat6);
		dat3 = vmaxq_u8(dat3, dat7);

		src += 64;
	}

	unsigned char out[64];
	vst1q_u8(out, dat0);
	vst1q_u8(out + 16, dat1);
	vst1q_u8(out + 32, dat2);
	vst1q_u8(out + 48, dat3);


	for (int i = 0; i < 64; i++) {
		if (out[i] > max)
		{
			max = out[i];
		}			
	}
#endif
#if 0
	printf("max val = %d\r\n", max);
#endif
	return max;
}
#endif
unsigned int getMaxVal(vector<uint8_t> &testArray)
{
	return getMaxVal(&testArray[0], testArray.size());
}





#define RANGE_LIMIT(x) (x > 255 ? 255 : (x < 0 ? 0 : x))

//#ifndef CORTEX
#if 0
void YUV422ToRGB565(const void* inbuf, void* outbuf, int width, int height)
{
	int rows, cols;
	int y0, y1, u, v, r, g, b;
	unsigned char *yuv_buf;
	unsigned short *rgb_buf;

	yuv_buf = (unsigned char *)inbuf;
	rgb_buf = (unsigned short *)outbuf;

	for (int pos = 0; pos < ((height * width /2) &(~127)) ;pos++) {

		y0 = yuv_buf[0];
		u = yuv_buf[1] - 128;
		y1 = yuv_buf[2];
		v = yuv_buf[3] - 128;		

		r = RANGE_LIMIT(y0 + v + ((v * 103) >> 8));
		g = RANGE_LIMIT(y0 - ((u * 88) >> 8) - ((v * 183) >> 8));
		b = RANGE_LIMIT(y0 + u + ((u * 198) >> 8));
		*rgb_buf++ = (((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3));

		r = RANGE_LIMIT(y1 + v + ((v * 103) >> 8));
		g = RANGE_LIMIT(y1 - ((u * 88) >> 8) - ((v * 183) >> 8));
		b = RANGE_LIMIT(y1 + u + ((u * 198) >> 8));
		*rgb_buf++ = (((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3));

		yuv_buf += 4;
	}
}
#else
void YUV422ToRGB565(const void* inbuf, void* outbuf, int width, int height)
{
	unsigned char *yuv_buf;
	unsigned char *rgb_buf;
	
	//printf("--------------------in YUV422ToRGB565  1-------------------\n");
	yuv_buf = (unsigned char *)inbuf;
	rgb_buf = (unsigned char *)outbuf;

	const int16x8_t const_128 = vdupq_n_s16(128);

	// const uint8x8_t const_8 = vdup_n_u8(8);

	int16_t dat[4] = { 88,198,103,183 };

	int16x4_t const_dat = vld1_s16(dat);
	//printf("--------------------in YUV422ToRGB565  2-------------------\n");
	for (int pos = 0; pos < ((height * width / 2 /8) &(~127)); pos++) {

		uint8x8x4_t yuv = vld4_u8(yuv_buf);

		int16x8_t y0 = vreinterpretq_s16_u16(vmovl_u8(yuv.val[0]));
		int16x8_t y1 = vreinterpretq_s16_u16(vmovl_u8(yuv.val[2]));

		int16x8_t u = vsubq_s16(vreinterpretq_s16_u16(vmovl_u8(yuv.val[1])), const_128);
		int16x8_t v = vsubq_s16(vreinterpretq_s16_u16(vmovl_u8(yuv.val[3])), const_128);

		int16x8_t rcfg = vshrq_n_s16(vmulq_lane_s16(v, const_dat,2), 8);
		rcfg = vaddq_s16(rcfg, v);

		int16x8_t gtmp0 = vmulq_lane_s16(u, const_dat, 0);
		int16x8_t gtmp1 = vmulq_lane_s16(v, const_dat, 3);

		int16x8_t gcfg = vshrq_n_s16(vaddq_s16(gtmp0, gtmp1),8);

		int16x8_t bcfg = vshrq_n_s16(vmulq_n_s16(u, 198), 8);
		bcfg = vaddq_s16(bcfg, u);

		int16x8_t rtmp00 = vaddq_s16(y0, rcfg);
		uint8x8_t r0 = vqmovun_s16(rtmp00);
		int16x8_t gtmp00 = vsubq_s16(y0, gcfg);
		uint8x8_t g0 = vqmovun_s16(gtmp00);
		int16x8_t btmp00 = vaddq_s16(y0, bcfg);
		uint8x8_t b0 = vqmovun_s16(btmp00);

		uint8x8x4_t ret;
		ret.val[1] = vsri_n_u8(r0, g0, 5);
		g0 = vshl_n_u8(g0, 3);
		ret.val[0] = vsri_n_u8(g0, b0, 3);

		int16x8_t rtmp10 = vaddq_s16(y1, rcfg);
		uint8x8_t r1 = vqmovun_s16(rtmp10);
		int16x8_t gtmp10 = vsubq_s16(y1, gcfg);
		uint8x8_t g1 = vqmovun_s16(gtmp10);
		int16x8_t btmp10 = vaddq_s16(y1, bcfg);
		uint8x8_t b1 = vqmovun_s16(btmp10);

		ret.val[3] = vsri_n_u8(r1, g1, 5);
		g1 = vshl_n_u8(g1, 3);
		ret.val[2] = vsri_n_u8(g1, b1, 3);
		vst4_u8(rgb_buf , ret);

		yuv_buf += 32;
		rgb_buf += 32;
	}

	//printf("--------------------out YUV422ToRGB565-------------------\n");
}

void YUV422ToRGB888(const void* inbuf, void* outbuf, int width, int height)
{
	unsigned char *yuv_buf;
	unsigned char *rgb_buf;

	yuv_buf = (unsigned char *)inbuf;
	rgb_buf = (unsigned char *)outbuf;

	const int16x8_t const_128 = vdupq_n_s16(128);

	// const uint8x8_t const_8 = vdup_n_u8(8);

	int16_t dat[4] = { 88,198,103,183 };

	int16x4_t const_dat = vld1_s16(dat);

	for (int pos = 0; pos < ((height * width / 2 / 8) &(~127)); pos++) {
		uint8x8x4_t yuv = vld4_u8(yuv_buf);

		int16x8_t y0 = vreinterpretq_s16_u16(vmovl_u8(yuv.val[0]));
		int16x8_t y1 = vreinterpretq_s16_u16(vmovl_u8(yuv.val[2]));

		int16x8_t u = vsubq_s16(vreinterpretq_s16_u16(vmovl_u8(yuv.val[1])), const_128);
		int16x8_t v = vsubq_s16(vreinterpretq_s16_u16(vmovl_u8(yuv.val[3])), const_128);

		int16x8_t rcfg = vshrq_n_s16(vmulq_lane_s16(v, const_dat, 2), 8);
		rcfg = vaddq_s16(rcfg, v);

		int16x8_t gtmp0 = vmulq_lane_s16(u, const_dat, 0);
		int16x8_t gtmp1 = vmulq_lane_s16(v, const_dat, 3);

		int16x8_t gcfg = vshrq_n_s16(vaddq_s16(gtmp0, gtmp1), 8);

		int16x8_t bcfg = vshrq_n_s16(vmulq_n_s16(u, 198), 8);
		bcfg = vaddq_s16(bcfg, u);

		int16x8_t rtmp00 = vaddq_s16(y0, rcfg);
		uint8x8_t r0 = vqmovun_s16(rtmp00);
		int16x8_t gtmp00 = vsubq_s16(y0, gcfg);
		uint8x8_t g0 = vqmovun_s16(gtmp00);
		int16x8_t btmp00 = vaddq_s16(y0, bcfg);
		uint8x8_t b0 = vqmovun_s16(btmp00);


		int16x8_t rtmp10 = vaddq_s16(y1, rcfg);
		uint8x8_t r1 = vqmovun_s16(rtmp10);
		int16x8_t gtmp10 = vsubq_s16(y1, gcfg);
		uint8x8_t g1 = vqmovun_s16(gtmp10);
		int16x8_t btmp10 = vaddq_s16(y1, bcfg);
		uint8x8_t b1 = vqmovun_s16(btmp10);
		//交叉字节
		uint8x8x2_t p0 = vzip_u8(r0, r1);
		uint8x8x2_t p1 = vzip_u8(g0, g1);
		uint8x8x2_t p2 = vzip_u8(b0, b1);

		uint8x8x3_t ret0;
		uint8x8x3_t ret1;

		ret0.val[0] = p0.val[0];
		ret0.val[1] = p1.val[0];
		ret0.val[2] = p2.val[0];
		ret1.val[0] = p0.val[1];
		ret1.val[1] = p1.val[1];
		ret1.val[2] = p2.val[1];

		vst3_u8(rgb_buf, ret0);
		vst3_u8(rgb_buf + 24, ret1);

		yuv_buf += 32;
		rgb_buf += 48;
	}
}



void YUV422ToGray_B(const void* inbuf, void* outbuf, int width, int height)
{
	unsigned char *yuv_buf;
	unsigned char *rgb_buf;

	yuv_buf = (unsigned char *)inbuf;
	rgb_buf = (unsigned char *)outbuf;

	const int16x8_t const_128 = vdupq_n_s16(128);

	for (int pos = 0; pos < ((height * width / 2 / 8) &(~127)); pos++) {
		uint8x8x4_t yuv = vld4_u8(yuv_buf);

		int16x8_t y0 = vreinterpretq_s16_u16(vmovl_u8(yuv.val[0]));
		int16x8_t y1 = vreinterpretq_s16_u16(vmovl_u8(yuv.val[2]));

		int16x8_t u = vsubq_s16(vreinterpretq_s16_u16(vmovl_u8(yuv.val[1])), const_128);
		// int16x8_t v = vsubq_s16(vreinterpretq_s16_u16(vmovl_u8(yuv.val[3])), const_128);

		int16x8_t bcfg = vshrq_n_s16(vmulq_n_s16(u, 198), 8);
		bcfg = vaddq_s16(bcfg, u);

		int16x8_t btmp00 = vaddq_s16(y0, bcfg);
		uint8x8_t b0 = vqmovun_s16(btmp00);

		uint8x8x2_t ret;
		ret.val[0] = b0;
		int16x8_t btmp10 = vaddq_s16(y1, bcfg);
		uint8x8_t b1 = vqmovun_s16(btmp10);

		ret.val[1] = b1;
		vst2_u8(rgb_buf, ret);

		yuv_buf += 32;
		rgb_buf += 16;
	}
}


void YUV422ToGray_G(const void* inbuf, void* outbuf, int width, int height)
{
	unsigned char *yuv_buf;
	unsigned char *rgb_buf;

	yuv_buf = (unsigned char *)inbuf;
	rgb_buf = (unsigned char *)outbuf;

	const int16x8_t const_128 = vdupq_n_s16(128);

	// const uint8x8_t const_8 = vdup_n_u8(8);

	int16_t dat[4] = { 88,198,103,183 };

	int16x4_t const_dat = vld1_s16(dat);

	for (int pos = 0; pos < ((height * width / 2 / 8) &(~127)); pos++) {
		uint8x8x4_t yuv = vld4_u8(yuv_buf);

		int16x8_t y0 = vreinterpretq_s16_u16(vmovl_u8(yuv.val[0]));
		int16x8_t y1 = vreinterpretq_s16_u16(vmovl_u8(yuv.val[2]));

		int16x8_t u = vsubq_s16(vreinterpretq_s16_u16(vmovl_u8(yuv.val[1])), const_128);
		int16x8_t v = vsubq_s16(vreinterpretq_s16_u16(vmovl_u8(yuv.val[3])), const_128);

		int16x8_t gtmp0 = vmulq_lane_s16(u, const_dat, 0);
		int16x8_t gtmp1 = vmulq_lane_s16(v, const_dat, 3);

		int16x8_t gcfg = vshrq_n_s16(vaddq_s16(gtmp0, gtmp1), 8);

		int16x8_t gtmp00 = vsubq_s16(y0, gcfg);
		uint8x8_t g0 = vqmovun_s16(gtmp00);

		uint8x8x2_t ret;
		ret.val[0] = g0;

		int16x8_t gtmp10 = vsubq_s16(y1, gcfg);
		uint8x8_t g1 = vqmovun_s16(gtmp10);

		ret.val[1] = g1;
		vst2_u8(rgb_buf, ret);

		yuv_buf += 32;
		rgb_buf += 16;
	}
}

void YUV422ToGray_R(const void* inbuf, void* outbuf, int width, int height)
{
	unsigned char *yuv_buf;
	unsigned char *rgb_buf;

	yuv_buf = (unsigned char *)inbuf;
	rgb_buf = (unsigned char *)outbuf;

	const int16x8_t const_128 = vdupq_n_s16(128);

	// const uint8x8_t const_8 = vdup_n_u8(8);

	int16_t dat[4] = { 88,198,103,183 };

	int16x4_t const_dat = vld1_s16(dat);

	for (int pos = 0; pos < ((height * width / 2 / 8) &(~127)); pos++) {
		uint8x8x4_t yuv = vld4_u8(yuv_buf);

		int16x8_t y0 = vreinterpretq_s16_u16(vmovl_u8(yuv.val[0]));
		int16x8_t y1 = vreinterpretq_s16_u16(vmovl_u8(yuv.val[2]));

		// int16x8_t u = vsubq_s16(vreinterpretq_s16_u16(vmovl_u8(yuv.val[1])), const_128);
		int16x8_t v = vsubq_s16(vreinterpretq_s16_u16(vmovl_u8(yuv.val[3])), const_128);

		int16x8_t rcfg = vshrq_n_s16(vmulq_lane_s16(v, const_dat, 2), 8);
		rcfg = vaddq_s16(rcfg, v);

		int16x8_t rtmp00 = vaddq_s16(y0, rcfg);
		uint8x8_t r0 = vqmovun_s16(rtmp00);

		uint8x8x2_t ret;
		ret.val[0] = r0;


		int16x8_t rtmp10 = vaddq_s16(y1, rcfg);
		uint8x8_t r1 = vqmovun_s16(rtmp10);
		ret.val[1] = r1;

		vst2_u8(rgb_buf, ret);

		yuv_buf += 32;
		rgb_buf += 16;
	}
}

void YUV422ToGray(const void* inbuf, void* outbuf, int width, int height, int flg)
{
	if (flg == 1) {
		YUV422ToGray_R(inbuf, outbuf, width, height);
	}

	if (flg == 2) {
		YUV422ToGray_G(inbuf, outbuf, width, height);
	}

	if (flg == 3) {
		YUV422ToGray_B(inbuf, outbuf, width, height);
	}
}



#endif


