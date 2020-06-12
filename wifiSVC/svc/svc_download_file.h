#include "../wifi_svc.h"
#include "../wifi_ctrl.h"
using namespace  std;
//单次接收发送模板

//重写read_pro_fun 接收处理函数 返回sta_need_write 启动 create_write_fun、
//重写mk_write_session_data,修改发送的数据
//重写virtual WIFI_BASE_FUNCTION * create_write_fun() = 0; 投递写入结构体
//重写virtual void destor_write_fun() = 0; 与上面对应 销毁写入结构体

struct WIFI_FUNCTION_DOWNLOAD_FILE :public WIFI_BASE_FUNCTION
{
	WIFI_FUNCTION_DOWNLOAD_FILE(WIFI_INFO & info) :WIFI_BASE_FUNCTION(info)
	{

	}


	virtual void DESTORY_FIRST(WIFI_INFO & info) final
	{
		delete this;
	}

	virtual void DESTORY_WRITE(WIFI_INFO & info) final
	{
		destor_write_fun();
	}

	virtual void destor_write_fun() = 0;
#if 0
	virtual int read_pro_fun(WIFI_BASE_SESSION & sec) = 0;
	virtual void mk_download_head_data(WIFI_BASE_SESSION & sec) = 0;
#endif
};



