#include <regex>
#include <map>
#include "SN1V2_com.h"
#include "mdc_ctrl.h"
#include "configOperator.h"
#include <iostream>
using namespace std;

void prinMap(map<string, string> & mdcdata)
{
	printf("||||||||||||||||");
	map<string, string>::iterator iter;
	iter = mdcdata.begin();
	while (iter != mdcdata.end()) {
		printf("%s:%s\n", iter->first.c_str(), iter->second.c_str());
		iter++;
	}
}

#define SCANF_Set_data(member,typ)      Set_data(SCANF_DATA,member,typ,0,0)

CFG_INFO mdc_info[] = {
	SCANF_Set_data(mod,dateType::INT32),
	SCANF_Set_data(manual0,dateType::FLOAT32),
	SCANF_Set_data(manual1,dateType::FLOAT32),
	SCANF_Set_data(correct0,dateType::FLOAT32),
	SCANF_Set_data(correct1,dateType::FLOAT32),
	SCANF_Set_data(initspeed0,dateType::INT32),
	SCANF_Set_data(initspeed1,dateType::INT32),
	SCANF_Set_data(maxspeed0,dateType::INT32),
	SCANF_Set_data(maxspeed1,dateType::INT32),
	SCANF_Set_data(phase0,dateType::INT32),
	SCANF_Set_data(phase1,dateType::INT32),
	SCANF_Set_data(period0,dateType::INT32),
	SCANF_Set_data(period1,dateType::INT32),
	SCANF_Set_data(current0,dateType::INT32),
	SCANF_Set_data(current1,dateType::INT32),
	SCANF_Set_data(ratio0,dateType::INT32),
	SCANF_Set_data(ratio1, dateType::INT32),
	SCANF_Set_data(getpar0,dateType::INT32),
	SCANF_Set_data(getpar1,dateType::INT32),
	SCANF_Set_data(cleanalarm0,dateType::INT32),
	SCANF_Set_data(cleanalarm1,dateType::INT32),
};

static CFG_INFO & GetInfo(const char * key)
{
	static CFG_INFO failinfo = { "fail",0,dateType::INT32,nullptr,0,DATASTATUS::dataInit };
	size_t maxsz = sizeof(mdc_info) / sizeof(CFG_INFO);

	for (size_t i = 0; i < maxsz; i++) {
		if (!strcmp(key, mdc_info[i].name)) {
			return mdc_info[i];
		}
	}
	return failinfo;
}


SCANF_DATA real_scan_file(const char * fil)
{
	SCANF_DATA out;

	ERR_STA err;
	map<string, string> mdcdata;
	ScanfFile(fil, mdcdata);
	scanfOneTable((char *)&out, mdc_info, sizeof(mdc_info) / sizeof(CFG_INFO), mdcdata);
	prinMap(mdcdata);

	string st;
	printTable2String(st, &out, mdc_info, sizeof(mdc_info) / sizeof(CFG_INFO), writeUseful);
	printf("ttt\n%s\n", st.c_str());
	return out;
}





void merge_data(MDC_INFO * pjif, SCANF_DATA & dat)
{
	//dat.mod
	if (GetInfo("mod").dataStatus == dataFromTable) {
		//手动
		printf("mod %d\n", dat.mod);
		if (dat.mod == mdc_mode_manual) {

			pjif->work_mod = mdc_mode_manual;

			if (GetInfo("manual0").dataStatus == dataFromTable) {
				pjif->mdcCtrl[0].manual.trig_set(dat.manual0);
			} else {
				pjif->mdcCtrl[0].manual.cpl_flag = 1;
			}

			if (GetInfo("manual1").dataStatus == dataFromTable) {
				pjif->mdcCtrl[1].manual.trig_set(dat.manual1);
			} else {
				pjif->mdcCtrl[1].manual.cpl_flag = 1;
			}
		}
		//暂停
		else if (dat.mod == mdc_mode_off) {
			pjif->mdcCtrl[0].stop.trig_set();
			pjif->mdcCtrl[1].stop.trig_set();
			pjif->work_mod = mdc_mode_off;
		} else {
			//自动
			pjif->work_mod = mdc_mode_table;
		}
	} else {
		pjif->work_mod = mdc_mode_table;
	}
	//校正
	if (GetInfo("correct0").dataStatus == dataFromTable) {
		pjif->mdcCtrl[0].correct.trig_set(dat.correct0);
	}
	if (GetInfo("correct1").dataStatus == dataFromTable) {
		pjif->mdcCtrl[1].correct.trig_set(dat.correct1);
	}


	for (auto & aim : pjif->mdcCtrl) {
		aim.par.setflg = 0;
		if (aim.parget.succ_flag == 0) {
			aim.parget.retry_num = 0;
		}
	}

	//参数设置
	if (GetInfo("initspeed0").dataStatus == dataFromTable) {
		pjif->mdcCtrl[0].correct.trig_set(dat.initspeed0);
	}
	if (GetInfo("initspeed1").dataStatus == dataFromTable) {
		pjif->mdcCtrl[1].correct.trig_set(dat.initspeed1);
	}
	if (GetInfo("maxspeed0").dataStatus == dataFromTable) {
		pjif->mdcCtrl[0].correct.trig_set(dat.maxspeed0);
	}
	if (GetInfo("maxspeed1").dataStatus == dataFromTable) {
		pjif->mdcCtrl[1].correct.trig_set(dat.maxspeed1);
	}

	if (GetInfo("phase0").dataStatus == dataFromTable) {
		pjif->mdcCtrl[0].correct.trig_set(dat.phase0);
	}
	if (GetInfo("phase1").dataStatus == dataFromTable) {
		pjif->mdcCtrl[1].correct.trig_set(dat.phase1);
	}

	if (GetInfo("period0").dataStatus == dataFromTable) {
		pjif->mdcCtrl[0].correct.trig_set(dat.period0);
	}
	if (GetInfo("period1").dataStatus == dataFromTable) {
		pjif->mdcCtrl[1].correct.trig_set(dat.period1);
	}

	if (GetInfo("current0").dataStatus == dataFromTable) {
		pjif->mdcCtrl[0].correct.trig_set(dat.current0);
	}
	if (GetInfo("current1").dataStatus == dataFromTable) {
		pjif->mdcCtrl[1].correct.trig_set(dat.current1);
	}

	if (GetInfo("ratio0").dataStatus == dataFromTable) {
		pjif->mdcCtrl[0].correct.trig_set(dat.ratio0);
	}
	if (GetInfo("ratio1").dataStatus == dataFromTable) {
		pjif->mdcCtrl[1].correct.trig_set(dat.ratio1);
	}
	//参数查询
	printf("type = %d\n", GetInfo("getpar0").dataStatus);
	if (GetInfo("getpar0").dataStatus == dataFromTable) {
		pjif->mdcCtrl[0].parget.trig_get();
	}
	if (GetInfo("getpar1").dataStatus == dataFromTable) {
		pjif->mdcCtrl[1].parget.trig_get();
	}
	//解除报警
	if (GetInfo("cleanalarm0").dataStatus == dataFromTable) {
		pjif->mdcCtrl[0].alarm.trig();
	}
	if (GetInfo("cleanalarm1").dataStatus == dataFromTable) {
		pjif->mdcCtrl[1].alarm.trig();
	}
}



