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

void RGB888_2_565(uint8_t * srcdata, uint8_t *dst, size_t pixCount)
{
	const unsigned int RGB565_RED = 0xf800;
	const unsigned int RGB565_GREEN = 0x07e0;
	const unsigned int RGB565_BLUE = 0x001f;

	uint16_t *dst16 = (uint16_t*)dst;
	while (pixCount--) {
		*dst16 = ((srcdata[0] >> 3) & RGB565_BLUE) |
			((srcdata[1] << 3) & RGB565_GREEN) |
			((srcdata[2] << 8) & RGB565_RED);
		
		dst16 += 1;
		srcdata += 3;
	}
}


#ifndef CORTEX
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
#if 0
	asm  volatile(
		"vld1.u8 d0,[%0] \n \t"
		"vld1.u8 d1,[%0] \n \t"

		"mov r7,#7 \n \t"
		"vdup.8 d3,r7 \n \t"


		"vmul.u8 d2,d0,d3 \n \t"
		//"add %1,%1,#16 \n\t"
		//"vld1.8 {d2,d3} ,[%0]! \n \t "
		//"vmovn.u16 d2,q1 \n \t"
		"vst1.8 {d2},[%1] \n \t"

		:
	: "r"(srcdata), "r"(dst), "r"(pixCount)
		: "q0", "q1" , "r7"
		);
#else
	uint8x16_t dat = vld1q_u8(srcdata);
	uint8x16_t dat2 = vld1q_u8(srcdata + 16);

	uint8x16_t ret = vmaxq_u8(dat, dat2);

	vst1q_u8(dst, ret);

#endif
}


#ifndef CORTEX
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
#if 0
	uint8x16_t comp = vdupq_n_u8(thres);
	insize /= 32;
	while (insize--) {
		__asm__ __volatile__(
			"pld [%0,#32] \n"
			:
		: "r" (src));

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


#ifndef CORTEX
int getMaxVal(vector<uint8_t> &testArray)
{
	vector<uint8_t>::iterator max;
	{
		TIME_INTERVAL_SCOPE("get max :");
		max = max_element(testArray.begin(), testArray.end());
	}
#if 0
	cout << "get max gray =" << (int)*max << endl;
#endif
	return *max;
}
#else
int getMaxVal(vector<uint8_t> &testArray)
{
	TIME_INTERVAL_SCOPE("get max :");

	unsigned char * src = &testArray[0];
	size_t num = testArray.size();
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
	printf("max val = %d\r\n", max);
	return max;
}
#endif










