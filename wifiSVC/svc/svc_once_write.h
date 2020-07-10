#ifndef __SVC_ONCE_WRITE_H___
#define __SVC_ONCE_WRITE_H___
#include "../wifi_svc.h"
#include "../wifi_ctrl.h"
#include <mutex>
using namespace  std;
//单次接收发送模板

//重写mk_write_data
struct WIFI_FUNCTION_ONCE_WRITE :public WIFI_BASE_FUNCTION
{
	WIFI_FUNCTION_ONCE_WRITE(WIFI_INFO & info, int inmsgid) :WIFI_BASE_FUNCTION(info), msgid(inmsgid)
	{
		SetProMask(WIFI_BASE_FUNCTION::MASK_SELF_UPLOAD);
	}

	virtual WIFI_PRO_STATUS wifi_read(WIFI_BASE_SESSION & sec) final
	{
		return WIFI_PRO_STATUS::WIFI_PRO_END;
	}


	virtual WIFI_PRO_STATUS wifi_write(WIFI_BASE_SESSION & sec) final
	{
		sec.data[0] = msgid;
		sec.data[1] = msgid >> 8;
		sec.data[2] = functionID;
		sec.data_len = 3 + mk_write_data(&sec.data[3], 100);
		//单次上传
		sec.frame_index = -1;
		return  WIFI_PRO_STATUS::WIFI_PRO_END;
	}

	virtual int mk_write_data(unsigned char * dat, int maxlen) = 0;
private:
	int msgid;
};

#endif

