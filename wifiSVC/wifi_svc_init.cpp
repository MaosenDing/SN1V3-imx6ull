#include "wifi_ctrl.h"


WIFI_BASE_FUNCTION * GetTEST(WIFI_INFO & wifi);

typedef WIFI_BASE_FUNCTION * (* WIFIINT)(WIFI_INFO & wifi);

WIFIINT wifigrp[] =
{
	GetTEST,
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
			tmp->DESTORY_FIRST(*this);
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








