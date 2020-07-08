#ifndef __SVC_ONCE_WRITE_H___
#define __SVC_ONCE_WRITE_H___
#include "../wifi_svc.h"
#include "../wifi_ctrl.h"
#include <mutex>
using namespace  std;
//单次接收发送模板

//重写read_pro_fun 接收处理函数 返回sta_need_write 启动 create_write_fun、
//重写mk_write_session_data,修改发送的数据
//重写virtual WIFI_BASE_FUNCTION * create_write_fun() = 0; 投递写入结构体
//重写virtual void destor_write_fun() = 0; 与上面对应 销毁写入结构体

struct WIFI_FUNCTION_ONCE_WRITE :public WIFI_BASE_FUNCTION
{
	WIFI_FUNCTION_ONCE_WRITE(WIFI_INFO & info) :WIFI_BASE_FUNCTION(info)
	{
		SetProMask(WIFI_BASE_FUNCTION::MASK_SELF_UPLOAD);
	}

	virtual WIFI_PRO_STATUS wifi_read(WIFI_BASE_SESSION & sec) final
	{
		return WIFI_PRO_STATUS::WIFI_PRO_END;
	}


	virtual WIFI_PRO_STATUS wifi_write(WIFI_BASE_SESSION & sec) final
	{
		mk_write_session_data(sec);
		//单次上传
		sec.frame_index = -1;
		return  WIFI_PRO_STATUS::WIFI_PRO_END;
	}

	virtual void mk_write_session_data(WIFI_BASE_SESSION & sec) = 0;
};

#endif

