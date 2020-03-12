#ifndef __wifi_ctrl_h____
#define __wifi_ctrl_h____

#include "JDcomhead.h"
#include "SN1V2_rtConfig.h"


struct WIFI_INFO :public JD_INFO_TIM {
	WIFI_INFO()
	{
	}
	SN1_SHM * psn1;
};









#endif
