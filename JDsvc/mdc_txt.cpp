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

static void reset_info()
{
	for (auto & p : mdc_info) {
		p.dataStatus = dataInit;
	}
}

SCANF_DATA real_scan_file(const char * fil)
{
	SCANF_DATA out;

	map<string, string> mdcdata;
	ScanfFile(fil, mdcdata);
	reset_info();
	scanfOneTable((char *)&out, mdc_info, sizeof(mdc_info) / sizeof(CFG_INFO), mdcdata);
#if 0
	prinMap(mdcdata);
	string st;
	printTable2String(st, &out, mdc_info, sizeof(mdc_info) / sizeof(CFG_INFO), writeUseful);
	printf("ttt\n%s\n", st.c_str());
#endif
	return out;
}





void merge_data(MDC_INFO * pjif, SCANF_DATA & dat)
{
	//printf("merge_data 00000000\n");
	//dat.mod
	if (GetInfo("mod").dataStatus == dataFromTable) {
		//printf("merge_data 000111111\n");
		//??????
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
		//??????
		else if (dat.mod == mdc_mode_off) {
			pjif->mdcCtrl[0].stop.trig_set();
			pjif->mdcCtrl[1].stop.trig_set();
			pjif->work_mod = mdc_mode_off;
		} else {
			//??????
			pjif->work_mod = mdc_mode_table;
		}
	} else {
		pjif->work_mod = mdc_mode_table;
		//printf("work_mod = mdc_mode_table\n");
	}
	//??????
	if (GetInfo("correct0").dataStatus == dataFromTable) {
		pjif->mdcCtrl[0].correct.trig_set(dat.correct0);
		printf("correct0 ok\n");
	}
	
	if (GetInfo("correct1").dataStatus == dataFromTable) {
		pjif->mdcCtrl[1].correct.trig_set(dat.correct1);
		//printf("correct1 0001444444\n");
	}
	

	for (auto & aim : pjif->mdcCtrl) {
		aim.par.setflg = 0;
		if (aim.parget.succ_flag == 0) {
			aim.parget.retry_num = 0;
		}
	}
	//printf("merge_data 000555555\n");
	//????????????
	if (GetInfo("initspeed0").dataStatus == dataFromTable) {
		pjif->mdcCtrl[0].correct.trig_set(dat.initspeed0);
		//printf("initspeed0 0001444444\n");
	}
	if (GetInfo("initspeed1").dataStatus == dataFromTable) {
		pjif->mdcCtrl[1].correct.trig_set(dat.initspeed1);
		//printf("initspeed1 0001444444\n");
	}
	if (GetInfo("maxspeed0").dataStatus == dataFromTable) {
		pjif->mdcCtrl[0].correct.trig_set(dat.maxspeed0);
		//printf("maxspeed0 0001444444\n");
	}
	if (GetInfo("maxspeed1").dataStatus == dataFromTable) {
		pjif->mdcCtrl[1].correct.trig_set(dat.maxspeed1);
		//printf("maxspeed1 0001444444\n");
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
	//????????????
	//printf("type = %d ???0???????????????????????????\n", GetInfo("getpar0").dataStatus);

	if (GetInfo("getpar0").dataStatus == dataFromTable) {
		pjif->mdcCtrl[0].parget.trig_get();
		//printf("getpar0 0001444444\n");
	}
	if (GetInfo("getpar1").dataStatus == dataFromTable) {
		pjif->mdcCtrl[1].parget.trig_get();
	}
	//????????????
	if (GetInfo("cleanalarm0").dataStatus == dataFromTable) {
		pjif->mdcCtrl[0].alarm.trig();
	}
	if (GetInfo("cleanalarm1").dataStatus == dataFromTable) {
		pjif->mdcCtrl[1].alarm.trig();
	}
}



