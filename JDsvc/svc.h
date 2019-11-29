#ifndef __svc_H___
#define __svc_H___
#include "JDcomhead.h"

struct JDAUTOSEND
{
	virtual int need_service() = 0;
	virtual void service_pro(JD_INFO & jif) = 0;
};






#endif

