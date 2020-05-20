#include "../wifi_svc.h"
#include "../wifi_ctrl.h"
#include <mutex>
using namespace  std;




struct WIFI_FUNCTION_UPLOADFILE_FILE :public WIFI_BASE_FUNCTION
{
	WIFI_FUNCTION_UPLOADFILE_FILE(WIFI_INFO & info) :WIFI_BASE_FUNCTION(info)
	{

	}

	virtual WIFI_PRO_STATUS wifi_read(WIFI_BASE_SESSION & sec) final
	{
		if (frameIndex == 0) {
			//if succ
			//如果可以发送 准备数据
			//prepare_data();
			//error
			//
		}


		//if succ frameindex ++
		//error resend frame
		return WIFI_PRO_STATUS::WIFI_PRO_END;
	}


	virtual WIFI_PRO_STATUS wifi_write(WIFI_BASE_SESSION & sec) final
	{
		if (frameIndex == 0) {


		}
		return  WIFI_PRO_STATUS::WIFI_PRO_END;
	}

	virtual void DESTORY_FIRST(WIFI_INFO & info) final
	{
		delete this;
	}

	virtual void DESTORY_WRITE(WIFI_INFO & info) final
	{
		destor_write_fun();
	}

	virtual WIFI_BASE_FUNCTION * create_write_fun() = 0;
	virtual void destor_write_fun() = 0;

	enum read_sta {
		sta_need_write = 1,
		sta_end = 2,
		sta_send_error = 3,
	};

	virtual void mk_upload_head_data(WIFI_BASE_SESSION & sec) = 0;
	virtual char * getData(int frameIndex) = 0;
	virtual int getDataMaxFrame() = 0;
	//
	virtual read_sta prepare_data(read_sta sta) = 0;

private:
	int frameIndex = 0;
};



