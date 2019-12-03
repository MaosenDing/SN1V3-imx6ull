#include <iostream>
#include "SN1V2_com.h"
#include <memory>
#include "jpgInc/jpeglib.h"
#include "errHandle/errHandle.h"
using namespace std;

ERR_STA SaveGRAYJpg(char * fName, IMAGEDATA & regImg)
{
	if (regImg.itype != IT_BIN_ONE_BYTE && regImg.itype != IT_GRAY_ONE_BYTE)
	{
		cout << "type error =" <<(int)regImg.itype << endl;
		SN1V2_ERROR_CODE_RET(err_Image_type_unsupported);
	}


	int width = regImg.right;
	int heigth = regImg.bottom;
	//struct jpeg_common_struct
	FILE * outfile = nullptr;
	outfile = fopen(fName, "wb");

	if (outfile == nullptr)
	{
		cout << "open file " << fName << " fail" << endl;
		SN1V2_ERROR_CODE_RET(err_cannot_open_file);
	}
	else
	{
		shared_ptr<FILE> fil(outfile, fclose);
		jpeg_compress_struct jcs;
		jpeg_error_mgr jerr;

		JSAMPROW row_point[1];
		//int row_stride;

		jcs.err = jpeg_std_error(&jerr);
		jpeg_create_compress(&jcs);
		jpeg_stdio_dest(&jcs, fil.get());


		jcs.image_width = width;
		jcs.image_height = heigth;
		jcs.input_components = 1;
		jcs.in_color_space = JCS_GRAYSCALE;

		jpeg_set_defaults(&jcs);
		jpeg_set_quality(&jcs, 80, boolean::TRUE);
		jpeg_start_compress(&jcs, boolean::TRUE);
#if 1
		for (int i = 0; i < heigth; i++)
		{
			row_point[0] = &regImg.at(0, i);
			jpeg_write_scanlines(&jcs, row_point, 1);
		}
#else
		row_point[0] = &regImg.at(0, 0);
		jpeg_write_scanlines(&jcs, row_point, heigth);
#endif

		jpeg_finish_compress(&jcs);
		jpeg_destroy_compress(&jcs);
		return (err_ok);
	}
}

extern "C" bool rgb565_to_rgb888(unsigned char * psrc, unsigned char * pdst, int width, int height, unsigned long *len);

ERR_STA SaveRGB565Jpg(char * fName, unsigned char * rgb565, int width, int heigth)
{
	FILE * outfile = nullptr;
	outfile = fopen(fName, "wb");

	vector<unsigned char > data;
	try
	{
		data.resize(width* heigth * 3);
	}
	catch (std::bad_alloc &bd)
	{
		fprintf(stderr, "len = %fm\n", width* heigth * 3/1024.0/1024.0);
		SN1V2_ERROR_CODE_RET(err_out_of_memory);
	}

	unsigned long len;
	rgb565_to_rgb888(rgb565, &data[0], width, heigth, &len);

	if (outfile == nullptr)
	{
		cout << "open file " << fName << " fail" << endl;
		SN1V2_ERROR_CODE_RET(err_cannot_open_file);
	}
	else
	{
		fprintf(stderr, "save jpeg\n");
		shared_ptr<FILE> fil(outfile, fclose);
		jpeg_compress_struct jcs;
		jpeg_error_mgr jerr;

		JSAMPROW row_point[1];
		//int row_stride;

		jcs.err = jpeg_std_error(&jerr);
		jpeg_create_compress(&jcs);
		jpeg_stdio_dest(&jcs, fil.get());


		jcs.image_width = width;
		jcs.image_height = heigth;
		jcs.input_components = 3;
		jcs.in_color_space = JCS_RGB;

		jpeg_set_defaults(&jcs);
		jpeg_set_quality(&jcs, 80, boolean::TRUE);
		jpeg_start_compress(&jcs, boolean::TRUE);
#if 1
		for (int i = 0; i < heigth; i++)
		{
			//row_point[0] = &regImg.at(0, i);
			row_point[0] = &data[i * width * 3];
			jpeg_write_scanlines(&jcs, row_point, 1);
		}
#else
		row_point[0] = &regImg.at(0, 0);
		jpeg_write_scanlines(&jcs, row_point, heigth);
#endif

		jpeg_finish_compress(&jcs);
		jpeg_destroy_compress(&jcs);
		return (err_ok);
	}
}


ERR_STA SaveRGB565Jpg(char * fName, IMAGEDATA & regImg)
{
	if (regImg.itype != IT_RGB565)
	{
		cout << "type error =" << (int)regImg.itype << endl;
		SN1V2_ERROR_CODE_RET(err_Image_type_unsupported);
	}

	int width = regImg.right;
	int heigth = regImg.bottom;
	//struct jpeg_common_struct

	return SaveRGB565Jpg(fName, &regImg.at(0, 0), width, heigth);
}


#if 1
#include <stdlib.h>
#include <string.h>
//#define  BYTE char



static int __getJPEGfromFile(FILE * fp, vector<unsigned char> &outData, int &w, int &h ,int & dataType)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, fp);

	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	unsigned long width = cinfo.output_width;
	unsigned long height = cinfo.output_height;
	unsigned short depth = cinfo.output_components;

	w = width;
	h = height;
	dataType = cinfo.out_color_space;
	try {
		outData.resize(width * height*depth);
	}
	catch (std::bad_alloc & bd)
	{
		return -2;
	}

	char * buff = (char *)&outData.at(0);
	memset(buff, 0, width*height*depth);


	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, width*depth, 1);

	char *startPos = buff;
	char * nextpoint = buff;
	while (cinfo.output_scanline < height)
	{
		jpeg_read_scanlines(&cinfo, buffer, 1);
		memcpy(nextpoint, *buffer, width*depth);
		nextpoint = &startPos[cinfo.output_scanline * width * depth];
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return 0;
}


int getJPEGfromFile(char * file, vector<unsigned char> &outData, int & w, int &h ,int & imgType)
{
	FILE *fp = fopen(file, "rb");

	if (fp == nullptr)
	{
		cout << "open error :" << file << endl;
		return -1;
	}
	else
	{
		int ret = __getJPEGfromFile(fp, outData, w, h , imgType);
		fclose(fp);
		return ret;
	}
}



#endif
