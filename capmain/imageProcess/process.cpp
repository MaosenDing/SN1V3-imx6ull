#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include "SN1V2_com.h"
#include "timSet.h"
#include <string.h>
#include <algorithm>
#include "errHandle/errHandle.h"
#include "time_interval.h"
using namespace std;



ERR_STA getLightBound(IMAGEDATA & inImage, int & top, int & bottom, int & left, int & right , int flgFast)
{
	TIME_INTERVAL_SCOPE("get bound :");
	if (inImage.itype != IT_BIN_ONE_BYTE) {
		SN1V2_ERROR_CODE_RET(err_Image_type_unsupported);
	}

	int width = inImage.right;
	int heigth = inImage.bottom;

#define MAXVAL (0xffff)

	top = MAXVAL;
	bottom = -1;
	left = MAXVAL;
	right = -1;

	uint32_t * src32 = (uint32_t *)&inImage.at(0, 0);
	if (flgFast && (width % 8 == 0)) {
		//向量化
		for (int y = 0; y < heigth; y++) {
			for (int x = 0; x < width / 4; x++) {
				uint32_t dat = src32[x + y * width / 4];
				if (dat) {
					if (y < top) {
						top = y;
					}

					if (y > bottom) {
						bottom = y;
					}

					if (x * 4 < left) {
						left = x * 4;
					}

					if (x * 4 > right) {
						right = x * 4 + 3;
					}
				}
			}
		}
	} else {
		for (int y = 0; y < heigth; y++) {
			for (int x = 0; x < width; x++) {
				if (inImage.at(x, y)) {
					if (y < top) {
						top = y;
					}

					if (y > bottom) {
						bottom = y;
					}

					if (x < left) {
						left = x;
					}

					if (x > right) {
						right = x;
					}
				}
			}
		}
	}



	if (top == MAXVAL || bottom == -1 || left == MAXVAL || right == -1) {
		top = -1;
		bottom = -1;
		left = -1;
		right = -1;
		SN1V2_ERROR_CODE_RET(err_binaryzation_aim_null);
	}
	return err_ok;
}



//查看感染最大使用堆栈
//#define COUT_MAX_HEAP
static unsigned int Infect(unsigned char * data, int width, int startPoint, int tag,
	const unsigned int InfectAim = 255,//感染目标
	const unsigned int MinInfect = 50,//链接最小像素
	const unsigned int CleanValue = 0)//失败处理值
{
	vector <int> v0;
	vector<int> v1;
	vector<int> cleanVect;
	cleanVect.push_back(startPoint);

	unsigned int count = 1;
	data[startPoint] = tag;

	v0.push_back(startPoint);
	//cout << "v0 =" << v0[0] << endl;
	bool ChangeFlag = true;

	while (true)
	{
		vector <int> *testVector = nullptr;
		vector<int> *gainVector = nullptr;

		if (ChangeFlag)
		{
			ChangeFlag = false;
			//cout << "flag1" << endl;
			testVector = &v0;
			gainVector = &v1;
		}
		else
		{
			ChangeFlag = true;
			//cout << "flag0" << endl;
			testVector = &v1;
			gainVector = &v0;
		}

		for (int &thisPosPPP : *testVector)
		{
			//cout << "this pos" << thisPosPPP <<"test size=" << testVector->size() << endl;

			int dirIndex[4] = {
				thisPosPPP - 1,
				thisPosPPP + 1,
				thisPosPPP - width,
				thisPosPPP + width
			};
			for (int testIndex : dirIndex)
			{
				if (data[testIndex] == InfectAim)
				{
					data[testIndex] = tag;
					gainVector->push_back(testIndex);
					if (cleanVect.size() < MinInfect)
					{
						cleanVect.push_back(testIndex);
					}
					count++;
				}
			}
		}

		if (gainVector->empty())
		{
#ifdef COUT_MAX_HEAP
			cout << "infect max count =" << max_use << endl;
#endif
			if (cleanVect.size() < MinInfect)
			{
				for (auto index : cleanVect )
				{
					data[index] = CleanValue;
				}
				count = 0;
#if 0
				cerr << "infect cnt = " << cleanVect.size() << endl;
#endif
			}
			return count;
		}
		testVector->clear();
	}
}



static void clean_other_tag(IMAGEDATA & inImage, int SaveTag)
{
#if COUT_IMAGE_DEBUG_INFO
	cout << "clean tag = " << SaveTag << endl;
#endif
	
#if 0
	int width = inImage.right;
	int heigth = inImage.bottom;
	for (int y = 0; y < heigth; y++)
	{
		for (int x = 0; x < width; x++)
		{
			unsigned char & testPos = inImage.at(x, y);
			if (testPos == SaveTag)
			{
				testPos = 255;
			}
			else
			{
				testPos = 0;
			}
		}
	}
#else
	for (size_t i = 0; i < ((size_t)inImage.size() & (~3)); i++) {
		unsigned char & testPos = inImage.Image_data[i];
		if (testPos == SaveTag) {
			testPos = 255;
		} else {
			testPos = 0;
		}
	}
#endif
}

//返回最大区域编号
static int step_in_tag(IMAGEDATA & inImage, int &pixCnt,const unsigned int MinCntGrp)
{
	int tag = 0;
	vector<int> inFectList;
	inFectList.push_back(0);
	
	size_t maxIndex = inImage.size();

	for (size_t index = 0; index < maxIndex; index++)
	{
		unsigned char & testPoint = inImage.at(index);
		if (testPoint == 255)
		{
			//递进感染编号
			if (++tag > 200)
			{
				char buff[64];
				snprintf(buff, 64, "tag sum overflow ,x=%d,y=%d", index % inImage.right, index / inImage.right);
				LOG(WARNING) << buff;
				break;
			}
			//感染扩散
			unsigned int infectCount = Infect(inImage.Image_data, inImage.right, index, tag, 255, MinCntGrp, 0);

			if (0 == infectCount)
			{
				tag--;
			}
			else
			{
				inFectList.push_back(infectCount);
			}
#if COUT_IMAGE_DEBUG_INFO > 0
			cout << "tag = " << tag << ",x=" << index%inImage.right << ",y=" << index / inImage.right << ",count =" << infectCount << endl;
#endif
		}
	}
	vector<int>::iterator Biggest = std::max_element(inFectList.begin(), inFectList.end());
#if COUT_IMAGE_DEBUG_INFO > 0
	cout << "block size =" << inFectList.size() << endl;
	cout << "max =" << *Biggest << ",index=" << std::distance(inFectList.begin(), Biggest) << endl;
#endif

#if 0
	for (auto p : inFectList)
	{
		cout << "num = " << p << endl;
	}
#endif

	pixCnt = *Biggest;
	return std::distance(inFectList.begin(), Biggest);
}

struct PixPos
{
	int x;
	int y;
};


static shared_ptr<vector<float> > getRList(vector<PixPos> & inPosList,float x, float y)
{
	shared_ptr<vector<float> >retR = make_shared<vector<float>>();
	retR->reserve(inPosList.size());

	for (auto & pix : inPosList)
	{
		float dx = x - pix.x;
		float dy = y - pix.y;

		retR->emplace_back(sqrt(dx * dx + dy * dy));
	}
	return retR;
}




static shared_ptr<vector<PixPos> > getLineament(IMAGEDATA &inImage)
{
	int width = inImage.right;
	//int heigth = inImage.bottom;

	shared_ptr<vector<PixPos> > retVect = make_shared<vector<PixPos>>();

	for (size_t index = 0; index < (size_t)inImage.size(); index++)
	{
		unsigned char & testPoint = inImage.at(index);

		if (testPoint)
		{
			size_t dirIndex[4] = {
				index - 1,
				index + 1,
				index - width,
				index + width
			};

			bool breakFlag = false;
			for (size_t sideIndex : dirIndex)
			{
				unsigned char & thisSide = inImage.at(sideIndex);
				if (thisSide == 0)
				{
					breakFlag = true;
					break;
				}
			}
			//非边界 做成内部点
			if (!breakFlag)
			{
				testPoint -= 1;
			}
		}
	}
	//消除内部点
	for (size_t index = 0; index < ((size_t)inImage.size() &(~3)); index++)
	{
		unsigned char testPoint = inImage.at(index);
		if (testPoint != 255)
		{
			testPoint = 0;
		}
		else
		{
			PixPos ps;
			ps.x = index % width;
			ps.y = index / width;
			retVect->push_back(ps);
		}
	}
#if (COUT_IMAGE_DEBUG_INFO > 0)
#if 0
	for (size_t i = 0; i < retVect->size(); i++)
	{
		cout << "R index " << i << ",x=" << retVect->at(i).x << ",y=" << retVect->at(i).y << endl;
	}
#endif
	cout << "get lineament pix count = " << retVect->size() << endl;
#endif

	return retVect;
}

ERR_STA RegImg(IMAGEDATA & inImage, int &pixCnt, const unsigned int MinCntGroup)
{
	if (inImage.itype != IT_BIN_ONE_BYTE)
	{
		SN1V2_ERROR_CODE_RET(err_Image_type_unsupported);
	}

	try
	{
		//求分割区域
		int tag = step_in_tag(inImage, pixCnt, MinCntGroup);
		if (tag == 0)
		{//infect fail..
			return err_binaryzation_aim_null;
		}

		//求唯一区域
		clean_other_tag(inImage, tag);
#if 0
		SaveGRAYJpg("save.jpg", inImage);
#endif
	}
	catch (std::bad_alloc & bd)
	{
		SN1V2_ERROR_CODE_RET(err_out_of_memory);
	}
	catch (std::out_of_range &)
	{
		SN1V2_ERROR_CODE_RET(err_out_of_memory);
	}
	catch (exception & exp)
	{
		SN1V2_ERROR_CODE_RET(err_UNKNOWN);
	}
	catch (...)
	{
		SN1V2_ERROR_CODE_RET(err_UNKNOWN);
		throw;
	}
	return err_ok;
}

static ERR_STA ImgCenter(IMAGEDATA & inImage ,float & center_x, float & center_y)
{
	int light_left = 0;
	int light_right = 0;
	int light_top = 0;
	int light_bottom = 0;
	ERR_STA err = getLightBound(inImage, light_top, light_bottom, light_left, light_right);
	
	if (err == err_ok)
	{
		center_x = (light_left + light_right) / 2.0;
		center_y = (light_top + light_bottom) / 2.0;
#if COUT_IMAGE_DEBUG_INFO > 0
		cout << "left =" << light_left << ",right =" << light_right << ",top =" << light_top << ",bottom =" << light_bottom << endl;
#endif
	}
	else
	{
#if COUT_IMAGE_DEBUG_INFO > 0
		cout << "imageCenter error =" << (int)err << endl;
#endif
	}
	return err;
}


float CirDeg(vector<PixPos> & RangePixPos, int diff_center_x, int diff_center_y)
{
	//求半径列表
	shared_ptr<vector<float> >retRList = getRList(RangePixPos, diff_center_x, diff_center_y);

	//半径最大 最小
	auto max = std::max_element(retRList->begin(), retRList->end());
	auto min = std::min_element(retRList->begin(), retRList->end());
	//平均半径
	float sum = 0;
	std::for_each(retRList->begin(), retRList->end(), [&sum](float f) {sum += f; });
	float mean = sum / retRList->size();
	//圆度
	float quality = 1 - (*max - *min) / mean;

#if COUT_IMAGE_DEBUG_INFO > 0
	cout << "get R size =" << retRList->size() << endl;
	cout << "max =" << *max << ",pos =" << std::distance(retRList->begin(), max) << endl;
	cout << "min =" << *min << ",pos =" << std::distance(retRList->begin(), min) << endl;
	cout << "mean =" << mean << endl;
#endif
	return quality;
}

int saveCir(const char * SavePath, vector<PixPos> & RangePixPos, int img_start_x, int img_start_y)
{
	vector<unsigned char> tmpwrite;

	SN1V2_INF_LOG("cap bound size = %d", RangePixPos.size());


	for (auto p : RangePixPos) {
		uint16_t x = img_start_x + p.x;
		uint16_t y = img_start_y + p.y;

		//unsigned char tmppp[4] = {x >> 8 , x & 0xff,y>>8,y&0xff};
		unsigned char tmppp[4];
		memcpy(tmppp, &x, 2);
		memcpy(tmppp + 2, &y, 2);

		for (auto p : tmppp) {
			tmpwrite.push_back(p);
		}
	}
#if 0 
	tmpwrite.resize(4000);
#endif
	saveBin(SavePath, tmpwrite);

	return 0;
}

enum error_bound {
	bound_null = 0,
	bound_top = 1 << 1,
	bound_bottom = 1 << 2,
	bound_left = 1 << 3,
	bound_right = 1 << 4,
};


static void fixBound(IMAGEDATA & inImage, error_bound er, int &left, int &right, int& top, int &bottom)
{
	if (er & bound_top) {
		for (int i = 0; i < inImage.right; i++) {
			inImage.at(i, top) = 0;
		}
		top += 1;
	}
	if (er & bound_bottom) {
		for (int i = 0; i < inImage.right; i++) {
			inImage.at(i, bottom) = 0;
		}
		bottom -= 1;
	}
	if (er & bound_left) {
		for (int i = 0; i < inImage.bottom; i++) {
			inImage.at(left, i) = 0;
		}
		left += 1;
	}
	if (er & bound_right) {
		for (int i = 0; i < inImage.bottom; i++) {
			inImage.at(right, i) = 0;
		}
		right -= 1;
	}
}

static error_bound check_bound(IMAGEDATA & inImage, int top, int bottom, int left, int right)
{
	int boudf = bound_null;
	if (top <= inImage.top) {
		boudf |= bound_top;
	}

	if (bottom >= (inImage.bottom - 1)) {
		boudf |= bound_bottom;
	}

	if (left <= inImage.left) {
		boudf |= bound_left;
	}

	if (right >= (inImage.right - 1)) {
		boudf |= bound_right;
	}
	return (error_bound)boudf;
}

static ERR_STA BinProcess(IMAGEDATA & inImage, PROCESS_RESULT & res, unsigned int MinCntGrp)
{
	ERR_STA err;

	int top = 0, bottom = 0, left = 0, right = 0;
	if (err_ok != (err = getLightBound(inImage, top, bottom, left, right , 1))) {
		SN1V2_WARN_MWSSAGE_WITH("bound error", err);
		return err;
	} else {
		error_bound boudf = check_bound(inImage, top, bottom, left, right);
		if (boudf) {
			fixBound(inImage, boudf, left, right, top, bottom);
			SN1V2_INF_LOG("bound error,left = %d,right = %d,top = %d,bottom = %d\n", left, right, top, bottom);
		}
		shared_ptr<IMAGEDATA> processImage = make_shared<IMAGEDATA>();
		int clone_left = left - 1;
		int clone_top = top - 1;
		int clone_width = right - left + 1 + 2;
		int clone_heigth = bottom - top + 1 + 2;

		err = inImage.clone(clone_left, clone_top, clone_width, clone_heigth, processImage);

		if (err != err_ok) {
			return err;
		} else {
			int pixCnt = 0;
			err = RegImg(*processImage, pixCnt, MinCntGrp);
			if (err != err_ok) {
				//cout << "reg Img error = " << (int)err << endl;
				ERR_STA no_reg_img = err;
				SN1V2_WARN_MWSSAGE_WITH("reg img", no_reg_img);
				return err;
			}
			//计算相对中心点
			float diff_center_x = 0;
			float diff_center_y = 0;
			err = ImgCenter(*processImage, diff_center_x, diff_center_y);
#if COUT_IMAGE_DEBUG_INFO > 0
			cout << "center x =" << diff_center_x << ",center y=" << diff_center_y << endl;
#endif
			//真中心点计算
			float real_center_x = diff_center_x + clone_left;
			float real_center_y = diff_center_y + clone_top;

			//计算中心偏差
			float diff_x = (real_center_x - (inImage.right - 1) / 2);
			float diff_y = (real_center_y - (inImage.bottom - 1) / 2);

			try {
				//求唯一区域轮廓
				shared_ptr<vector<PixPos> >RangePixPos = getLineament(*processImage);
				saveCir(Transimit_img_Path, *RangePixPos, clone_left, clone_top);
				float	quality = CirDeg(*RangePixPos, diff_center_x, diff_center_y);
#if 0
				cout << "real center x =" << real_center_x << ",real center y=" << real_center_y << endl;
				cout << "pix count =" << pixCnt << endl;
				cout << "diff x =" << diff_x << ",diff y=" << diff_y << endl;
				cout << "circle quality = " << quality * 100 << "%" << endl;

#endif

				res.center_x = real_center_x;
				res.center_y = real_center_y;
				res.diff_x = diff_x;
				res.diff_y = diff_y;
				res.pixCnt = pixCnt;
				res.quality = quality;
			} catch (std::out_of_range & p) {
				SN1V2_ERROR_CODE_RET(err_diff_cal);
			} catch (std::bad_alloc & p) {
				SN1V2_ERROR_CODE_RET(err_out_of_memory);
			} catch (exception &p) {
				SN1V2_ERROR_CODE_RET(err_UNKNOWN);
			} catch (...) {
				SN1V2_ERROR_CODE_RET(err_UNKNOWN);
				throw;
			}
			return err_ok;
		}
	}
	SN1V2_ERROR_CODE_RET(err_UNKNOWN);
}


ERR_STA ImageProcessRGB(const char *saveName, shared_ptr<unsigned char> LoadImg, int inputSize, int width, int height, PROCESS_RESULT & res, int thres, float thresPer, bool BINjpgSaveFlag = true, unsigned int MinCntGrp = 50)
{
	TIME_INTERVAL_SCOPE("ImageProcessRGB:");

	ERR_STA err = err_ok;
	int pixcnt = inputSize / 2;
	RGB565GRAY(LoadImg.get(), inputSize);

	//制作二值化图片
	shared_ptr<IMAGEDATA> BinImage = make_shared<IMAGEDATA>();
	{
		BinImage->Image_data = LoadImg.get();
		BinImage->left = 0;
		BinImage->right = width;
		BinImage->top = 0;
		BinImage->bottom = height;
		BinImage->byte_per_pix = 1;
		BinImage->itype = IT_BIN_ONE_BYTE;;
		{
			TIME_INTERVAL_SCOPE("bin operator:");
			err = BinaImg(LoadImg.get(), pixcnt, thres, thresPer);
		}
	}


	if (err == err_ok) {
		TIME_INTERVAL_SCOPE("image process operator:");
		err = BinProcess(*BinImage, res, MinCntGrp);
	}
	return err;
}

ERR_STA ImageProcessGRAY(const char *saveName, shared_ptr<unsigned char> LoadImg, int inputSize, int width, int height, PROCESS_RESULT & res, int thres, float thresPer, bool BINjpgSaveFlag = true, unsigned int MinCntGrp = 50)
{
	TIME_INTERVAL_SCOPE("ImageProcessGRAY:");

	ERR_STA err = err_ok;
	int pixcnt = inputSize ;
	//制作二值化图片
	shared_ptr<IMAGEDATA> BinImage = make_shared<IMAGEDATA>();
	{
		BinImage->Image_data = LoadImg.get();
		BinImage->left = 0;
		BinImage->right = width;
		BinImage->top = 0;
		BinImage->bottom = height;
		BinImage->byte_per_pix = 1;
		BinImage->itype = IT_BIN_ONE_BYTE;;
		{
			TIME_INTERVAL_SCOPE("bin operator:");
			err = BinaImg(LoadImg.get(), pixcnt, thres, thresPer);
		}
	}

	if (err == err_ok && BINjpgSaveFlag == true) {
		auto start = chrono::system_clock::now();
		SaveGRAYJpg((char *)saveName, *BinImage);
		auto tim = std::chrono::duration_cast<std::chrono::duration<float>>(chrono::system_clock::now() - start).count();
		LOG_IF(WARNING, tim > 1) << "jpg compress&save:" << tim << "S";
	}


	if (err == err_ok) 
	{
		TIME_INTERVAL_SCOPE("image process operator:");
		err = BinProcess(*BinImage, res, MinCntGrp);
	}
	return err;
}

#include <sys/stat.h>

ERR_STA ImageTestRGBFile(const char * fName, const char * srcPath, const char * dstPath, int width, int height, PROCESS_RESULT & res, int thres, float thresPer, const char * jpgName)
{
	ERR_STA err;
	char realName[64];
	sprintf(realName, "%s/%s", srcPath, fName);


	struct stat stbuff;
	if ((stat(realName, &stbuff) == -1) ||
		(stbuff.st_size != width * height * 2))
	{
		cout << "file " << realName << " len error" << endl;
		return err_cannot_open_file;
	}

	char *getData;
	int filSize;
	{
		TIME_INTERVAL_SCOPE("file operator:");
		err = loadFileC((char *)realName, &getData, &filSize);
		if (err != err_ok)
		{
			cout << "load file err = " << (int)err << endl;
			return err;
		}
	}
	shared_ptr<unsigned char > sharedata((unsigned char *)getData, [](unsigned char * p) {delete[] p; });
	//获取时间
	res.timGetImg = time(nullptr);

	char * FnameBuff = realName;
	if (jpgName != nullptr)
	{
		sprintf(FnameBuff, "%s/%s", dstPath, jpgName);
	}
	else
	{
		tm reftime;
		localtime_r(&res.timGetImg, &reftime);
		sprintf(FnameBuff, "%s/%02d-%02d-%02d.jpg", dstPath, reftime.tm_hour, reftime.tm_min, reftime.tm_sec);
	}

	return ImageProcessRGB(FnameBuff, std::move(sharedata), filSize, width, height, res, thres, thresPer);
}

ERR_STA cap_once(unsigned char * rgb565buff, int &insize, const unsigned int gain, const unsigned int expo
	, const int horizenFlip, const int VeriFlip);


ERR_STA ImageCapRGB(const char * dstPath, int width, int height, PROCESS_RESULT & res, int thres, float thresPer
	, bool ORGjpgSaveFlag, bool BINjpgSaveFlag, unsigned int MinCntGrp, const unsigned int gain, const unsigned int expo
	, const int horflip, const int verFlip
)
{
	ERR_STA err;
	int imgSize = width * height * 2;

	try {
		shared_ptr<unsigned char >ppp(new unsigned char[imgSize], [](unsigned char * p) {delete[] p; });

		if ((err = cap_once(ppp.get(), imgSize, gain, expo, horflip, verFlip)) != err_ok) {
			LOG(ERROR) << "cap error code = " << (int)err;
			return err;
		} else {
			res.timGetImg = time(nullptr);

			char FnameBuff[64];

			tm reftime;
			localtime_r(&res.timGetImg, &reftime);
			sprintf(FnameBuff, "%s/%02d-%02d-%02d.jpg", dstPath, reftime.tm_hour, reftime.tm_min, reftime.tm_sec);

			if (ORGjpgSaveFlag == true) {
				char ORGBUFF[64];
				snprintf(ORGBUFF, 64, "%s.org.jpeg", FnameBuff);

				if ((err = SaveRGB565Jpg(ORGBUFF, ppp.get(), width, height)) != err_ok) {
					LOG(WARNING) << "org rgb565 save fail =" << (int)err;
				}
			}

			return ImageProcessRGB(FnameBuff, std::move(ppp), imgSize, width, height, res, thres, thresPer, BINjpgSaveFlag, MinCntGrp);
		}
	} catch (std::bad_alloc & bd) {
		return	err_out_of_memory;
	}
	return err_UNKNOWN;
}

ERR_STA cap_once_gray(unsigned char * rgb565buff, int &insize, const unsigned int gain, const unsigned int expo
	, const int horizenFlip, const int VeriFlip, char * savename);

ERR_STA ImageCap(const char * dstPath, int width, int height, PROCESS_RESULT & res, int thres, float thresPer
	, bool ORGjpgSaveFlag, bool BINjpgSaveFlag, unsigned int MinCntGrp, const unsigned int gain, const unsigned int expo
	, const int horflip, const int verFlip
)
{
	ERR_STA err;
	int imgSize = width * height;

	try {
		shared_ptr<unsigned char >ppp(new unsigned char[imgSize], [](unsigned char * p) {delete[] p; });

		res.timGetImg = time(nullptr);

		char FnameBuff[64];

		tm reftime;
		localtime_r(&res.timGetImg, &reftime);
		sprintf(FnameBuff, "%s/%02d-%02d-%02d.jpg", dstPath, reftime.tm_hour, reftime.tm_min, reftime.tm_sec);
		char *dstptr = 0;
		char ORGBUFF[64] = {0};
		if (ORGjpgSaveFlag == true) {
			snprintf(ORGBUFF, 64, "%s.org.jpeg", FnameBuff);
			dstptr = ORGBUFF;
		}

		if ((err = cap_once_gray(ppp.get(), imgSize, gain, expo, horflip, verFlip, dstptr)) != err_ok) {
			LOG(ERROR) << "cap error code = " << (int)err;
			return err;
		} else {
			sprintf(FnameBuff, "%s/%02d-%02d-%02d.jpg", dstPath, reftime.tm_hour, reftime.tm_min, reftime.tm_sec);
			return ImageProcessGRAY(FnameBuff, std::move(ppp), imgSize, width, height, res, thres, thresPer, BINjpgSaveFlag, MinCntGrp);
		}
	} catch (std::bad_alloc & bd) {
		return	err_out_of_memory;
	}
	return err_UNKNOWN;
}

