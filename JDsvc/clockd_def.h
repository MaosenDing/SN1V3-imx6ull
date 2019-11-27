#ifndef __DEF_CLOCKD_H__
#define __DEF_CLOCKD_H__

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum __FIL_SV_RET
	{
		fsRet_ok = 0,
		fsRet_NoWork = -1,
		fsRet_TimOut = -2,
		fsRet_Clean = -3,
		fsRet_InotifyFail = -4,
	}fsRet;



#define USING_FAKE_CRE 0
#define ADD_TEST_JD_FUN 0


#ifdef __cplusplus
}
#endif



#endif

