#ifndef __sunpostable_h___
#define __sunpostable_h___
#include <time.h>

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
};


#endif

