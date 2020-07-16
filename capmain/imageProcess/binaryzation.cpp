#include "SN1V2_com.h"
#include <algorithm>
#include <stdint.h>
#include <iostream>
#include "time_interval.h"
#include "errHandle/errHandle.h"
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
#include <arm_neon.h>
void RGB565GRAY(uint16_t * srcdata, uint8_t *dst, size_t pixCount)
{
	//pixCount = 8;
	const uint16x8_t RGB565_RED = vdupq_n_u16(0xf800);
	const uint16x8_t RGB565_GREEN = vdupq_n_u16(0x07e0);
	const uint16x8_t RGB565_BLUE = vdupq_n_u16(0x001f);
	const uint16x8_t ROUND_NUM = vdupq_n_u16(50/8);

	//const uint16x8_t const_299 = vdupq_n_u16(299);
	//const uint16x8_t const_587 = vdupq_n_u16(587);
	//const uint16x8_t const_114 = vdupq_n_u16(114);
	const uint16x8_t const_1000 = vdupq_n_u16(1000);

	uint16_t * endp = &srcdata[pixCount];

	while (srcdata < endp){
		uint16x8_t rgb565 = vld1q_u16(srcdata);
		uint16x8_t tmpr0 = vandq_u16(rgb565, RGB565_RED);
		uint16x8_t tmpg0 = vandq_u16(rgb565, RGB565_GREEN);
		uint16x8_t tmpb0 = vandq_u16(rgb565, RGB565_BLUE);

		uint16x8_t tmpr1 = vshrq_n_u16(tmpr0, 8);
		uint16x8_t tmpg1 = vshrq_n_u16(tmpg0, 3);
		uint16x8_t tmpb1 = vshlq_n_u16(tmpb0, 3);

		uint16x8_t ans = vmulq_n_u16(tmpr1, 299/8);
		ans = vmlaq_n_u16(ans, tmpg1, 587/8);
		ans = vmlaq_n_u16(ans, tmpb1, 114/8);

		ans = vaddq_u16(ans, ROUND_NUM);

		uint8x8_t ret = vshrn_n_u16(ans, 10-3);
		vst1_u8(dst, ret);
		dst += 8;
		srcdata += 8;
	}
}

#endif

void neon_test(uint8_t * srcdata, uint8_t *dst, size_t pixCount)
{
	asm  volatile(
		"vdup.u8 q0,%2 \n \t"
		//"add %1,%1,#16 \n\t"
		//"vld1.8 {d2,d3} ,[%0]! \n \t "
		"vst1.8 {d0,d1},[%1] \n \t"

		:
	: "r"(srcdata), "r"(dst), "r"(pixCount)
		: "q0", "q1"
		);

}

void binary_tes(unsigned char * src, int thr, size_t len)
{
	if (len % 2 == 0)
	{
		for (size_t i = 0;i < len/2;i++)
		{
			unsigned char *test1 = &src[2 * i];
			unsigned char *test2 = &src[2 * i + 1];


			if (*test1 > thr)
			{
				*test1 = 255;
			}
			else
			{
				*test1 = 0;
			}

			if (*test2 > thr)
			{
				*test2 = 255;
			}
			else
			{
				*test2 = 0;
			}
		}
	}
	else
	{
		for (size_t i = 0;i < len;i++)
		{
			if (src[i] > thr)
			{
				src[i] = 255;
			}
			else
			{
				src[i] = 0;
			}
		}
	}
}







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
#if 0
	uint8x16_t comp = vdupq_n_u8(thres);
	insize /= 16;
	while (insize--) {
		uint8x16_t dat = vld1q_u8(src);
		uint8x16_t dat2 = vcgtq_u8(dat, comp);
		vst1q_u8(src, dat2);
		src += 16;
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