#ifndef __tableWork_H____
#define __tableWork_H____

#include <vector>
#include <string>
#include <memory>

#include "SN1V2_com.h"

struct tableWork
{
	virtual void work(unsigned int time, int flag) = 0;
	virtual ~tableWork() = default;
};
int timTableWork(std::vector<unsigned int> & table, tableWork & test);


struct setWork
{
	virtual void work(timTableSet & time, int flag) = 0;
	virtual ~setWork() = default;
};

int timTableWork(std::vector<timTableSet> & table, setWork & test);
int timTableWorkWithReserveTime(std::vector<timTableSet> & table, setWork & test, int TimeS);
int dummyLongTimeCapTest(setWork & test, int period);


#if USING_DUMMY_CAP_SRC > 0
struct dummyWork2 :public setWork
{
	int thres;
	float thresPer;
	char *rdpath;
	std::shared_ptr<std::vector<std::string> >sp;
	char *aimPath;
	float QualityThres;
	int save_org;
	int save_bin;
	
	SN1_SHM * psn1;

	explicit dummyWork2(int thr, float per, char *pa, char * aimP, float inQuality,int orgSave,int binSave);
	virtual void work(timTableSet & time, int flag) override;
private:
	int index;
	dummyWork2(const dummyWork2&);
	const dummyWork2& operator =(dummyWork2 &);
};
#endif
#include <chrono>


typedef ERR_STA(T_ImageCapRGB)(const char * dstPath, int width, int height, PROCESS_RESULT & res, int thres, float thresPer
	, bool ORGjpgSaveFlag, bool BINjpgSaveFlag, unsigned int MinCntGrp, const unsigned int gain, const unsigned int expo
	, const int horflip, const int verFlip
	);


struct capWork : public setWork
{
	char *rdpath;
	char *aimPath;
	const char *res_path;

	SN1_CFG cfg;
	SN1_SHM * psn1;
	
	int test_for_cap_once = 0;
	
	explicit capWork(char * aimP,SN1_CFG & inCfg,SN1_SHM * in_psn1,char *in_res_path , T_ImageCapRGB * incap);
	virtual void work(timTableSet & time, int flag) override;
	T_ImageCapRGB * This_ImageCapRGB;
private:
	int index;
	capWork(const capWork&);
	const capWork& operator =(capWork &);
	//called when miss to take photo
	void cap_miss_process(timTableSet &);
	//called when mdc status error
	void cap_status_error_process(timTableSet &);
	std::chrono::time_point<std::chrono::system_clock> lastEnd;//记录最后一次时间
	void cap_ok();	
};

#endif
