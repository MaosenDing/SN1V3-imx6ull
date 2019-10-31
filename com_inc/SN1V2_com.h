#ifndef __SN1V2_COM____h__
#define __SN1V2_COM____h__

#include "SN1V2_error.h"
#include <vector>
#include <string>
#include <memory>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
	/*
	图像结构体
	*/
	enum IMAGE_TYPE {
		IT_NULL = 0,//未初始化或其他状态
		IT_JPG = 1,
		IT_BMP = 2,
		IT_RGB565 = 3,
		IT_RGB888 = 4,
		IT_YUV422 = 5,
		IT_YUV444 = 6,
		IT_GRAY_ONE_BYTE = 7,//灰度化
		IT_BIN_ONE_BYTE = 8,//二值化
	};


	struct IMAGEDATA{
		IMAGE_TYPE itype;
		//图像边界
		int left;
		int right;
		int top;
		int bottom;
		//每个像素点所占空间大小 -1表示jpg压缩 0表示未初始化
		int byte_per_pix;

		//char *Image_data;//图像数据
		std::shared_ptr<std::vector<uint8_t> > Image_data;


		IMAGEDATA() :itype(IT_NULL),
			left(0),right(0),top(0),bottom(0),
			byte_per_pix(0),Image_data(new std::vector<uint8_t>())
		{
		}
		ERR_STA clone(int x, int y,int width,int height, std::shared_ptr<IMAGEDATA> & outPoint);
		IMAGEDATA & operator = (IMAGEDATA &);
		
		//不做任何校验，超出范围 行为未定义
		//只使用于1字节图像
		//使用请前确保分配内存和使用范围
		inline unsigned char & at(int x, int y)
		{
			return Image_data->at(x + y*right);
			//return (*Image_data)[x + y*right];
		}

	private:
		IMAGEDATA(IMAGEDATA &);
	};

	bool checkImageType(IMAGE_TYPE test);
	bool checkImageType(IMAGEDATA & img);

	//固定分割
	ERR_STA RTF(char * filePath, char fdelimiter, std::vector<int>& frange, std::vector <std::string> & outVector);
	//写入
	ERR_STA WTF(char * filePath, char fdelimiter, char wtype, std::string & fdata);

	//保存文件
	ERR_STA saveBin(char * filePath, char * data, unsigned long dataSize);
	ERR_STA saveBin(char * filePath, std::vector<uint8_t> & vdata);
	ERR_STA SavImg(char * filPath, IMAGEDATA & ImageData);
	ERR_STA saveCsv(char * filePath, std::vector<uint8_t> & vdata, int lineWidth);
	//载入文件
	ERR_STA loadFile(char *fname, std::string & refVect);
	ERR_STA loadFile(char *fname, std::vector<uint8_t> & refVect);
	ERR_STA loadFileC(char *fname, char ** refVect, int * outsize);
	//rgb565 转换
	bool RGB565binaryzation(char * srcdata, char *dst, size_t pixCount);
	bool RGB565binaryzation(std::vector<uint8_t> & srcdata, std::vector <uint8_t> &dst);
	
	//灰度化 无边界检测
	void RGB565GRAY(uint16_t * srcdata, uint8_t *dst, size_t pixCount);
	ERR_STA RGB565GRAY(unsigned char * LoadImg, int inputSize, std::shared_ptr<std::vector<uint8_t> > & outdata);
	//二值化图像
	ERR_STA BinaImg(IMAGEDATA & inputData, unsigned int gth, float bth, IMAGEDATA & outImage);
	ERR_STA BinaImg(IMAGEDATA & procData, unsigned int gth, float bth);
	//获取二值化亮区边界
	ERR_STA getLightBound(IMAGEDATA & inImage, int & top, int & bottom, int & left, int & right);
	//获取最大连通区域
	ERR_STA RegImg(IMAGEDATA & inImage, int & pixCnt);




	//解析时间字符串
	ERR_STA ResTime(const std::string & sString, const std::string & tType, tm & outTime);
	
	//针对 "yyyy-MM-dd hh:mm:ss"
	ERR_STA regTmType_1(const std::string & ppp, tm & rtm);

	//获取时间
	ERR_STA GetTim(std::string & outString);
	ERR_STA GetTim(tm & reftm);
	//设置时间
	ERR_STA SetTim(std::string & outString);
	ERR_STA SetTim(tm & reftm);
	//时间延时
	ERR_STA TimDelayUntil(std::string &sTim);
	ERR_STA TimDelayUntil(time_t tUntil);

	ERR_STA TimDelay(time_t inSec);

	//时间转换 tm->outTim
	ERR_STA mktime(const tm& inTm, time_t & outTim);

	//扫描配置文本
	ERR_STA SDS(const char * fileName);
	//获取配置数据
	ERR_STA SDG(const char *, std::string & outData);
	ERR_STA SDG(const char * inkey, float & outfloat);
	ERR_STA SDG(const char * inkey, int & outfloat);
	ERR_STA SDG(const char * inkey, unsigned int & outfloat);
	ERR_STA SDG(const char * inkey, bool & outbool);
	ERR_STA getConf(const char * configName, SN1_CFG * cfg,int flag = 1);
	//保存jpg图像格式
	ERR_STA SaveGRAYJpg(char * fName, IMAGEDATA & regImg);
	ERR_STA SaveRGB565Jpg(char * fName, IMAGEDATA & regImg);
	ERR_STA SaveRGB565Jpg(char * fName, unsigned char * rgb565, int width, int heigth);

	//输出图像过程调试信息
#define COUT_IMAGE_DEBUG_INFO 0


	struct PROCESS_RESULT{
		float center_x;
		float center_y;

		float diff_x;
		float diff_y;

		float quality;

		size_t pixCnt;
		time_t timGetImg;
		time_t TableTimeGetImg;
	};

	//处理RGB图像
	ERR_STA ImageProcessRGB(const char *saveName, unsigned char * LoadImg, int inputSize, int width, int height, PROCESS_RESULT & res, int thres, float thresPer);


	enum scanf_type {
		type_with_colon,
		type_with_underline,
		type_with_full_config,
	};


	struct timTableSet {
		time_t tt;
		int tm_hour;
		int tm_min;
		int tm_sec;
		float ZxAng;
		float YxAng;
		float RIx;
		float RIy;
		float RIz;

		timTableSet(int inhour, int inmin, int insec,
			float a1, float a2,
			float b1, float b2, float b3) :
			tm_hour(inhour), tm_min(inmin), tm_sec(insec)
			, ZxAng(a1), YxAng(a2)
			, RIx(b1), RIy(b2), RIz(b3)
		{}

		timTableSet() {};
	};

	struct RESOBJ {
		int hour , min, sec;
		double pos[2];
		int sz;
		double data[5];
		double angle;
		char useflag;
	};


	struct CREOBJ
	{
		int year, month, day;
		double errData[8];
		double extra[6];
		unsigned int capTime = 0;
		char pidbuff[20];
	};


	//载入时间表
	int load_table(char * filename, std::vector<unsigned int> & outTable , scanf_type intype = type_with_underline);
	ERR_STA load_table(char * filename, std::vector<timTableSet> & outTable);
	//时间表裁剪
	//void rm_past_tm(std::vector<unsigned int> &table);

#define USING_DUMMY_CAP_SRC 1

	//转换rgb文件
	ERR_STA ImageTestRGBFile(const char * fName, const char * srcPath, const char * dstPath, int width, int height, PROCESS_RESULT & res, int thres, float thresPer, const char * jpgName = nullptr);

	std::shared_ptr<std::vector<std::string> > getAllFileName(char * path, const char * postfix = ".jpg.rgb565");

	//运营表计算
	ERR_STA SHG(int Year, int Month, int Day, int HelioAdjTime, int HelioPreTime, int StartAngle, \
		double Longitude, double Latitude, int E, int P, int T, int Delta_T, double dTHI, double (&EPEs)[8]
		, std::vector<timTableSet> & rts
		);

	ERR_STA SHG(int Year, int Month, int Day, char fdir[], int HelioAdjTime, int HelioPreTime, int StartAngle, \
		double Longitude, double Latitude, int E, int P, int T, int Delta_T, double dTHI, double (&EPEs)[8]);

	//保存运营表
	ERR_STA save_timTableSet(char * fdir, int year, int mon, int day, std::vector<timTableSet> & rts);
	//修正拍摄数据

	enum res_filter_mode {
		//正常 不处理
		res_filter_normal = 0,
		//交换x y
		res_filter_exchange,
		//交换x y并且翻转 x y
		res_filter_exchange_then_reserve_x,
		res_filter_exchange_then_reserve_y,
		res_filter_exchange_then_reserve_x_y,
		//不交换 但是翻转 x y
		res_filter_noexchange_reserve_x,
		res_filter_noexchange_reserve_y,
		res_filter_noexchange_reserve_x_y,
	};


	ERR_STA res_filter(const char * resName,const SN1_CFG * const pcfg ,double (&exta) [6] ,res_filter_mode mod = res_filter_normal);

	//获取某天误差
	std::shared_ptr< std::vector <CREOBJ>> initCre(const char * creName);
	bool ChkTodayCre(std::vector<CREOBJ> & creDataGrp);
	CREOBJ GetNewCre(std::vector<CREOBJ> & creDataGrp);
	ERR_STA CalCre(const char * resName, CREOBJ & creData, const double SPS, const double SFL);
	ERR_STA save_cre(const char * creName, std::vector <CREOBJ> & creGrp, const int PID);
	//bool getLastErrConfig(const char * creName, CREOBJ & creObj);
	//计算误差表
	extern "C" char * CRE(const char *path, double sps, double sfl, double pep[8]);

	//ipc
	key_t getKey(const char *path, int num);
	void * getSHM(key_t key, int sz);
#endif

