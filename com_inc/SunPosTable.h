#ifndef __sunpostable_h___
#define __sunpostable_h___
#include <time.h>
#include <memory>
#include <vector>
#include <sn1v3cfg.h>
struct SUNPOS{

	time_t tt;
	
	double ZR_u;
	double ZR_v;
	double ZR_At;
	double ZR_Az;

	double SD_u;
	double SD_v;
	double SD_At;
	double SD_Az;

	double a0;
	double a1;
	double a2;
	double a3;
};

std::shared_ptr< std::vector < SUNPOS > > createTable(Tg_table &tg_table, int year, int mon, int day);

//r1 r2 默认取1.0f
int ConAlg(float ut, float vt, float ZR_u, float ZR_v, float ZR_At, float ZR_Az, float r1 , float r2 , float q, float fx, float fy
	, float *At, float *Az, int *SpdSig_At, int *SpdSig_Az
);

#endif

