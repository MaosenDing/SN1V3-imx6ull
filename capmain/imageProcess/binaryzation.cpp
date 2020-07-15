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
