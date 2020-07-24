#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "SN1V2_com.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>
#include <string.h>
#include "errHandle/errHandle.h"
using namespace std;





//接管所有数据
IMAGEDATA & IMAGEDATA::operator = (IMAGEDATA & fromData)
{
	this->itype = fromData.itype;
	this->left = fromData.left;
	this->right = fromData.right;
	this->top = fromData.top;
	this->bottom = fromData.bottom;

	this->byte_per_pix = fromData.byte_per_pix;
	this->Image_data = fromData.Image_data;

	return *this;
}




bool checkImageType(IMAGE_TYPE test)
{
	switch (test)
	{
	case IT_JPG:
	case IT_BMP:
	case IT_RGB565:
	case IT_RGB888:
	case IT_YUV422:
	case IT_YUV444:
	case IT_GRAY_ONE_BYTE:
	case IT_BIN_ONE_BYTE:
		return true;
	default:
		return false;
	}
}


bool checkImageType(IMAGEDATA & img)
{
	return checkImageType(img.itype);
}



ERR_STA IMAGEDATA::clone(int x, int y, int width, int height, std::shared_ptr<IMAGEDATA> & outPoint)
{
	int inleft = x;
	int inright = x + width;
	int inup = y;
	int inDown = y + height;

	if (this->itype != IT_BIN_ONE_BYTE && this->itype != IT_GRAY_ONE_BYTE)
	{
		SN1V2_ERROR_CODE_RET(err_Image_type_unsupported);
	}

	if (inright > this->right || inDown > this->bottom || x < 0 || y < 0)
	{
		SN1V2_WARN_LOG("init x = %d,y = %d,width = %d, height = %d", x, y, width, height);
		SN1V2_WARN_CODE_RET(err_Inval_image);
	}

	try
	{
		*outPoint = *this;//复制所有属性;
		outPoint->Image_data = (unsigned char *)malloc(width*height);
		outPoint->delflg = 1;
	}
	catch (std::bad_alloc &bd)
	{
		SN1V2_ERROR_CODE_RET(err_out_of_memory);
	}

#if COUT_IMAGE_DEBUG_INFO > 0
	cout << "clone copy data width,height =" << width << "," << height << endl;
#endif
	outPoint->left = 0;
	outPoint->right = width;
	outPoint->top = 0;
	outPoint->bottom = height;

	int tmpx;
	int tmpy;

	try
	{
		for (tmpy = 0; tmpy < height; tmpy++)
			for (tmpx = 0; tmpx < width; tmpx++)
			{
				{
					int thisx = tmpx + inleft;
					int thisy = tmpy + inup;

					outPoint->at(tmpx, tmpy) = this->at(thisx, thisy);
				}
			}
	}
	catch (out_of_range & exp)
	{
		outPoint.reset();
		SN1V2_ERROR_CODE_RET(err_invaild_memory_access);
	}
	return err_ok;
}


