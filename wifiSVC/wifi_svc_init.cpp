#include "wifi_ctrl.h"

uint16_t WIFI_BASE_FUNCTION::static_msg_id = 0;
//单接收
WIFI_BASE_FUNCTION * GetSn2Center(WIFI_INFO & wifi);
WIFI_BASE_FUNCTION * GetCleanFilesystem(WIFI_INFO & wifi);
//一次发送
WIFI_BASE_FUNCTION * GetWIFI_QUERY_SINGLE_DATA(WIFI_INFO & wifi);
WIFI_BASE_FUNCTION * GetWIFI_WRITE_SINGLE_CFG(WIFI_INFO & wifi);
//文件上传
WIFI_BASE_FUNCTION * Getuploadupatefile(WIFI_INFO & wifi);
//文件下载
WIFI_BASE_FUNCTION * Getdownloadupatefile(WIFI_INFO & wifi);
WIFI_BASE_FUNCTION * Getdownloadcfgtable(WIFI_INFO & wifi);
typedef WIFI_BASE_FUNCTION * (* WIFIINT)(WIFI_INFO & wifi);

WIFIINT wifigrp[] =
{
	//单接收
	GetSn2Center,
	GetCleanFilesystem,
	//一次发送
	GetWIFI_QUERY_SINGLE_DATA,
	GetWIFI_WRITE_SINGLE_CFG,
	//文件上传
	Getuploadupatefile,
	//文件下载
	Getdownloadupatefile,
	Getdownloadcfgtable,
};


void InitWIFI_svc(WIFI_INFO & wifi)
{
	int index = sizeof(wifigrp) / sizeof(WIFIINT);

	int pos = 0;

	wifi.svcGrp = new WIFI_BASE_FUNCTION*[index];

	for (auto & p : wifigrp) {
		auto tmp = p(wifi);
		if (tmp) {
			wifi.svcGrp[pos++] = tmp;
		}
	}
	wifi.svcCnt = pos;
}

void WIFI_INFO::delete_svc()
{
	if (svcGrp && svcCnt >= 0) {
		for (int i = 0; i < svcCnt; i++) {
			WIFI_BASE_FUNCTION* tmp = svcGrp[i];
			delete tmp;
		}
		delete svcGrp;
		svcGrp = 0;
		svcCnt = 0;
	}
}



WIFI_BASE_FUNCTION * FindFunction(WIFI_INFO & wifi, int funMask, int funid)
{
	if (wifi.svcCnt <= 0 || 0 == wifi.svcGrp) {
		return nullptr;
	}

	for (int i = 0; i < wifi.svcCnt; i++) {
		WIFI_BASE_FUNCTION * tmp = wifi.svcGrp[i];

		if ((tmp->GetProMask() & funMask) && (tmp->GetFunctionID() == funid)) {
			return tmp;
		}
	}
	return nullptr;
}

void WIFI_BASE_FUNCTION::ADD_FUN(WIFI_BASE_FUNCTION * pfun)
{
	std::unique_lock<std::mutex> lk(info.mtx_write_fun_list);

	info.write_fun_list.push_back(pfun);
}






