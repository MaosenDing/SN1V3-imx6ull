#ifndef __timset_h____
#define __timset_h____

#ifdef __cplusplus
extern "C" {
#endif

	int timeSet(long tms);
	//获取时间 秒和微妙
	int timeGet(long * pSec, long *pUsec);

#ifdef __cplusplus
}
#endif

#endif
