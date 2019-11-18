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
#include "time_interval.h"

using namespace std;



//************************************
// Method:    CapImg
// FullName:  CapImg
// Access:    public 
// Returns:   ERR_STA
// Qualifier:
// Parameter: float ImGain
// Parameter: float imExpo
// Parameter: float imHf
// Parameter: float imVf
// Parameter: IMAGEDATA & outImage
// Des: ����ͼƬ δʵ��
//************************************
ERR_STA CapImg(float ImGain, float imExpo, float imHf, float imVf, IMAGEDATA & outImage)
{




	//outImage.Image_data = std::make_shared<std::vector<char>>();
	return err_UNKNOWN;
}



//************************************
// Method:    SavImg
// FullName:  SavImg
// Access:    public 
// Returns:   ERR_STA
// Qualifier:
// Parameter: char * filPath
// Parameter: IMAGEDATA & ImageData
//************************************
ERR_STA SavImg(char * filPath, IMAGEDATA & ImageData)
{
	if (filPath == nullptr)
	{
		SN1V2_ERROR_CODE_RET(err_inval_para);
	}

	if (!checkImageType(ImageData.itype))
	{
		SN1V2_ERROR_CODE_RET( err_Image_type_undefined);
	}

	if (ImageData.itype == IT_JPG)
	{
		return saveBin(filPath, *(ImageData.Image_data));
	}
	if (ImageData.itype == IT_BIN_ONE_BYTE) //��ֵ��  csv��ʽ
	{
		return saveCsv(filPath, *ImageData.Image_data, ImageData.right);
	}
	else
	{
		SN1V2_ERROR_CODE_RET(err_Image_type_unsupported);
	}

	SN1V2_ERROR_CODE_RET(err_UNKNOWN);
}


void fastbinaryzation(unsigned char * src, int thres, int insize);


//************************************
// Method:    BinaImg
// FullName:  BinaImg
// Access:    public 
// Returns:   ERR_STA
// Qualifier:
// Parameter: IMAGEDATA & inputData
// Parameter: unsigned int gth
// Parameter: float bth
// Parameter: IMAGEDATA & outImage
// ��ֵ������ inputData -> outImage
// ��ʵ�ֶԻҶȻ�ͼ��Ķ�ֵ��
//************************************
ERR_STA BinaImg(IMAGEDATA & inputData, unsigned int gth, float bth, IMAGEDATA & outImage)
{
	if (&inputData == &outImage)
	{
		return BinaImg(inputData, gth, bth);
	}


	if (gth > 0 && gth <= 255 && bth > 0.000001f && bth < 1.0f)
	{
		if (inputData.itype != IT_GRAY_ONE_BYTE)
		{
			SN1V2_ERROR_CODE_RET(err_Image_type_unsupported);
		}

		if (!inputData.Image_data->empty())
		{
			vector<uint8_t> & testArray = *inputData.Image_data;

			vector<uint8_t>::iterator max;
			{
				TIME_INTERVAL_SCOPE("get max :");
				max = max_element(testArray.begin(), testArray.end());
			}
#if 0
			cout << "get max gray =" << (int)*max << endl;
#endif
			int realThres = *max * bth;
			if (*max > gth)
			{
				shared_ptr<vector<uint8_t>> tmpArray;
				try
				{
					tmpArray = make_shared<vector<uint8_t>>(testArray);
				}
				catch (std::bad_alloc & bad)
				{
					cout << "BinaImg :: makeshared :bad alloc" << endl;
					return err_out_of_memory;
				}
				
				{
					TIME_INTERVAL_SCOPE("test real bin :");
#if 0
				//��ֵ��
					for_each(tmpArray->begin(), tmpArray->end(), [realThres](uint8_t & p) {
					if (p > realThres)
					{
						p = 255;
					}
					else
					{
						p = 0;
					}
				});
#else
					fastbinaryzation(&tmpArray->at(0), realThres, tmpArray->size());			
#endif
				}
#if 0
				//�����ֵ��ͼ��
				cout << "save bin error code =" << (int)saveBin((char *)"mybin", testArray) << endl;
#endif
				outImage.itype = IT_BIN_ONE_BYTE;
				outImage.left = inputData.left;
				outImage.right = inputData.right;
				outImage.top = inputData.top;
				outImage.bottom = inputData.bottom;
				outImage.byte_per_pix = 1;
				outImage.Image_data = tmpArray;

				return err_ok;
			}
			else
			{
				SN1V2_ERROR_CODE_RET(err_binaryzation_aim_null);
			}
		}
		else
		{
			SN1V2_ERROR_CODE_RET(err_Inval_image);
		}
	}
	else
	{
		SN1V2_ERROR_CODE_RET(err_inval_para);
	}
	SN1V2_ERROR_CODE_RET(err_UNKNOWN);
}


ERR_STA BinaImg(IMAGEDATA & procData, unsigned int gth, float bth)
{
	if (gth > 0 && gth <= 255 && bth > 0.000001f && bth < 1.0f)
	{
		if (procData.itype != IT_GRAY_ONE_BYTE)
		{
			SN1V2_ERROR_CODE_RET(err_Image_type_unsupported);
		}

		if (!procData.Image_data->empty())
		{
			vector<uint8_t> & testArray = *procData.Image_data;

			vector<uint8_t>::iterator max;
			{
				TIME_INTERVAL_SCOPE("get max :");
				max = max_element(testArray.begin(), testArray.end());
			}
#if 0
			cout << "get max gray =" << (int)*max << endl;
#endif
			int realThres = *max * bth;
			if (*max > gth)
			{
				shared_ptr<vector<uint8_t>> tmpArray = procData.Image_data;

				{
					TIME_INTERVAL_SCOPE("test real bin :");
#if 0
					//��ֵ��
					for_each(tmpArray->begin(), tmpArray->end(), [realThres](uint8_t & p) {
						if (p > realThres)
						{
							p = 255;
						}
						else
						{
							p = 0;
						}
					});
#else
					fastbinaryzation(&tmpArray->at(0), realThres, tmpArray->size());
#endif
				}
#if 0
				//�����ֵ��ͼ��
				cout << "save bin error code =" << (int)saveBin((char *)"mybin", testArray) << endl;
#endif
				procData.itype = IT_BIN_ONE_BYTE;
				return err_ok;
			}
			else
			{//��ͼ�Ͳ���¼����
				SN1V2_INFO_CODE_RET(err_binaryzation_aim_null);
			}
		}
		else
		{//��ͼ�Ͳ���¼����
			SN1V2_INFO_CODE_RET(err_Inval_image);
		}
	}
	else
	{
		SN1V2_ERROR_CODE_RET(err_inval_para);
	}
	SN1V2_ERROR_CODE_RET(err_UNKNOWN);
}






