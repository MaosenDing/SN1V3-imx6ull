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
	ͼ��ṹ��
	*/
	enum IMAGE_TYPE {
		IT_NULL = 0,//δ��ʼ��������״̬
		IT_JPG = 1,
		IT_BMP = 2,
		IT_RGB565 = 3,
		IT_RGB888 = 4,
		IT_YUV422 = 5,
		IT_YUV444 = 6,
		IT_GRAY_ONE_BYTE = 7,//�ҶȻ�
		IT_BIN_ONE_BYTE = 8,//��ֵ��
	};


	struct IMAGEDATA{
		IMAGE_TYPE itype;
		//ͼ��߽�
		int left;
		int right;
		int top;
		int bottom;
		//ÿ�����ص���ռ�ռ��С -1��ʾjpgѹ�� 0��ʾδ��ʼ��
		int byte_per_pix;

		//char *Image_data;//ͼ������
		std::shared_ptr<std::vector<uint8_t> > Image_data;


		IMAGEDATA() :itype(IT_NULL),
			left(0),right(0),top(0),bottom(0),
			byte_per_pix(0),Image_data(new std::vector<uint8_t>())
		{
		}
		ERR_STA clone(int x, int y,int width,int height, std::shared_ptr<IMAGEDATA> & outPoint);
		IMAGEDATA & operator = (IMAGEDATA &);
		
		//�����κ�У�飬������Χ ��Ϊδ����
		//ֻʹ����1�ֽ�ͼ��
		//ʹ����ǰȷ�������ڴ��ʹ�÷�Χ
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

	//�̶��ָ�
	ERR_STA RTF(char * filePath, char fdelimiter, std::vector<int>& frange, std::vector <std::string> & outVector);
	//д��
	ERR_STA WTF(char * filePath, char fdelimiter, char wtype, std::string & fdata);

	//�����ļ�
	ERR_STA saveBin(char * filePath, char * data, unsigned long dataSize);
	ERR_STA saveBin(char * filePath, std::vector<uint8_t> & vdata);
	ERR_STA SavImg(char * filPath, IMAGEDATA & ImageData);
	ERR_STA saveCsv(char * filePath, std::vector<uint8_t> & vdata, int lineWidth);
	//�����ļ�
	ERR_STA loadFile(char *fname, std::string & refVect);
	ERR_STA loadFile(char *fname, std::vector<uint8_t> & refVect);
	ERR_STA loadFileC(char *fname, char ** refVect, int * outsize);
	//rgb565 ת��
	bool RGB565binaryzation(char * srcdata, char *dst, size_t pixCount);
	bool RGB565binaryzation(std::vector<uint8_t> & srcdata, std::vector <uint8_t> &dst);
	
	//�ҶȻ� �ޱ߽���
	void RGB565GRAY(uint16_t * srcdata, uint8_t *dst, size_t pixCount);
	ERR_STA RGB565GRAY(unsigned char * LoadImg, int inputSize, std::shared_ptr<std::vector<uint8_t> > & outdata);
	//��ֵ��ͼ��
	ERR_STA BinaImg(IMAGEDATA & inputData, unsigned int gth, float bth, IMAGEDATA & outImage);
	ERR_STA BinaImg(IMAGEDATA & procData, unsigned int gth, float bth);
	//��ȡ��ֵ�������߽�
	ERR_STA getLightBound(IMAGEDATA & inImage, int & top, int & bottom, int & left, int & right);
	//��ȡ�����ͨ����
	ERR_STA RegImg(IMAGEDATA & inImage, int & pixCnt);




	//����ʱ���ַ���
	ERR_STA ResTime(const std::string & sString, const std::string & tType, tm & outTime);
	
	//��� "yyyy-MM-dd hh:mm:ss"
	ERR_STA regTmType_1(const std::string & ppp, tm & rtm);

	//��ȡʱ��
	ERR_STA GetTim(std::string & outString);
	ERR_STA GetTim(tm & reftm);
	//����ʱ��
	ERR_STA SetTim(std::string & outString);
	ERR_STA SetTim(tm & reftm);
	//ʱ����ʱ
	ERR_STA TimDelayUntil(std::string &sTim);
	ERR_STA TimDelayUntil(time_t tUntil);

	ERR_STA TimDelay(time_t inSec);

	//ʱ��ת�� tm->outTim
	ERR_STA mktime(const tm& inTm, time_t & outTim);

	//ɨ�������ı�
	ERR_STA SDS(const char * fileName);
	//��ȡ��������
	ERR_STA SDG(const char *, std::string & outData);
	ERR_STA SDG(const char * inkey, float & outfloat);
	ERR_STA SDG(const char * inkey, int & outfloat);
	ERR_STA SDG(const char * inkey, unsigned int & outfloat);
	ERR_STA SDG(const char * inkey, bool & outbool);
	ERR_STA getConf(const char * configName, SN1_CFG * cfg,int flag = 1);
	//����jpgͼ���ʽ
	ERR_STA SaveGRAYJpg(char * fName, IMAGEDATA & regImg);
	ERR_STA SaveRGB565Jpg(char * fName, IMAGEDATA & regImg);
	ERR_STA SaveRGB565Jpg(char * fName, unsigned char * rgb565, int width, int heigth);

	//���ͼ����̵�����Ϣ
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

	//����RGBͼ��
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


	//����ʱ���
	int load_table(char * filename, std::vector<unsigned int> & outTable , scanf_type intype = type_with_underline);
	ERR_STA load_table(char * filename, std::vector<timTableSet> & outTable);
	//ʱ���ü�
	//void rm_past_tm(std::vector<unsigned int> &table);

#define USING_DUMMY_CAP_SRC 1

	//ת��rgb�ļ�
	ERR_STA ImageTestRGBFile(const char * fName, const char * srcPath, const char * dstPath, int width, int height, PROCESS_RESULT & res, int thres, float thresPer, const char * jpgName = nullptr);

	std::shared_ptr<std::vector<std::string> > getAllFileName(char * path, const char * postfix = ".jpg.rgb565");

	//��Ӫ�����
	ERR_STA SHG(int Year, int Month, int Day, int HelioAdjTime, int HelioPreTime, int StartAngle, \
		double Longitude, double Latitude, int E, int P, int T, int Delta_T, double dTHI, double (&EPEs)[8]
		, std::vector<timTableSet> & rts
		);

	ERR_STA SHG(int Year, int Month, int Day, char fdir[], int HelioAdjTime, int HelioPreTime, int StartAngle, \
		double Longitude, double Latitude, int E, int P, int T, int Delta_T, double dTHI, double (&EPEs)[8]);

	//������Ӫ��
	ERR_STA save_timTableSet(char * fdir, int year, int mon, int day, std::vector<timTableSet> & rts);
	//������������

	enum res_filter_mode {
		//���� ������
		res_filter_normal = 0,
		//����x y
		res_filter_exchange,
		//����x y���ҷ�ת x y
		res_filter_exchange_then_reserve_x,
		res_filter_exchange_then_reserve_y,
		res_filter_exchange_then_reserve_x_y,
		//������ ���Ƿ�ת x y
		res_filter_noexchange_reserve_x,
		res_filter_noexchange_reserve_y,
		res_filter_noexchange_reserve_x_y,
	};


	ERR_STA res_filter(const char * resName,const SN1_CFG * const pcfg ,double (&exta) [6] ,res_filter_mode mod = res_filter_normal);

	//��ȡĳ�����
	std::shared_ptr< std::vector <CREOBJ>> initCre(const char * creName);
	bool ChkTodayCre(std::vector<CREOBJ> & creDataGrp);
	CREOBJ GetNewCre(std::vector<CREOBJ> & creDataGrp);
	ERR_STA CalCre(const char * resName, CREOBJ & creData, const double SPS, const double SFL);
	ERR_STA save_cre(const char * creName, std::vector <CREOBJ> & creGrp, const int PID);
	//bool getLastErrConfig(const char * creName, CREOBJ & creObj);
	//��������
	extern "C" char * CRE(const char *path, double sps, double sfl, double pep[8]);

	//ipc
	key_t getKey(const char *path, int num);
	void * getSHM(key_t key, int sz);
#endif

