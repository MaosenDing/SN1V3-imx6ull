#include <sys/time.h>
#include "timSet.h"
#include <errno.h>



int timeSet(long tms)
{
	struct timeval tv;
	int ret;
	tv.tv_sec = tms;
	tv.tv_usec = 0;

	//tz.tz_dsttime = 0;
	//tz.tz_minuteswest = 0;

	ret = settimeofday(&tv, 0);
	if (ret != 0)
	{
		//return -1;
		return errno;
	}
	return 0;
}


int timeGet(long * pSec,long *pUsec)
{
	struct timeval tv;
	//struct timezone tz;
	int ret;

	//tz.tz_dsttime = 0;
	//tz.tz_minuteswest = 0;

	ret = gettimeofday(&tv, 0);
	if (ret != 0)
	{
		//return -1;
		return errno;
	}
	*pSec = tv.tv_sec;
	*pUsec = tv.tv_usec;
	return 0;
}
