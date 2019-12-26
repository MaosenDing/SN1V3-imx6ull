#include <regex>
#include <map>
#include "SN1V2_com.h"
#include "JDcomhead.h"
#include <iostream>
using namespace std;

static ERR_STA SDG(map<string, string> &sysData, const char * inkey, string & outData)
{
	try {
		outData = sysData.at(inkey);
	} catch (out_of_range & p) {
		outData.clear();
		return(err_conf_get_null);
	}
	return err_ok;
}

static ERR_STA SDG(map<string, string> &sysData,const char * inkey, char * outData)
{
	string st;
	ERR_STA err = SDG(sysData, inkey, st);
	if (err != err_ok) {
		return err;
	}
	strcpy(outData, st.c_str());
	return err_ok;
}


static ERR_STA SDG(map<string, string> &sysData,const char * inkey, int & outInt)
{
	string st;
	ERR_STA err = SDG(sysData , inkey, st);

	if (err != err_ok) {
		return err;
	}

	if (sscanf(st.c_str(), "%d", &outInt) == 1) {
		return err_ok;
	} else {
		return err_conf_tran_error;
	}
}

static ERR_STA SDG(map<string, string> &sysData,const char * inkey, unsigned int & outInt)
{
	string st;
	ERR_STA err = SDG(sysData,inkey, st);

	if (err != err_ok) {
		return err;
	}

	if (sscanf(st.c_str(), "%d", &outInt) == 1) {
		return err_ok;
	} else {
		return err_conf_tran_error;
	}
}


static ERR_STA SDG(map<string, string> &sysData,const char * inkey, float & outfloat)
{
	string st;
	ERR_STA err = SDG(sysData, inkey, st);

	if (err != err_ok) {
		return err;
	}

	if (sscanf(st.c_str(), "%f", &outfloat) == 1) {
		return err_ok;
	} else {
		return err_conf_tran_error;
	}
}

static ERR_STA SDG(map<string, string> &sysData,const char * inkey, bool & outbool)
{
	string st;
	ERR_STA err = SDG(sysData, inkey, st);

	if (err != err_ok) {
		return err;
	}

	transform(st.begin(), st.end(), st.begin(), ::tolower);

	if (!strcmp(st.c_str(), "true")) {
		outbool = true;
		return err_ok;
	}

	if (!strcmp(st.c_str(), "false")) {
		outbool = false;
		return err_ok;
	}
	return	err_conf_tran_error;
}




static void regexFile(const char * fileName, map<string, string > &mdcData)
{
	string stText;
	ERR_STA sta = loadFile((char *)fileName, stText);
	if (sta != err_ok) {
		return;
	}

	regex reg("%(\\w+\\d*),([\\w\\d\\\\.+-]+)");
	smatch match;

	string::const_iterator star = stText.begin();
	string::const_iterator end = stText.end();
	while (regex_search(star, end, match, reg)) {
		auto ret = mdcData.emplace(match[1], match[2]);
		star = match[0].second;
	}
}

void prinMap(map<string, string> & mdcdata)
{
	cout << "||||||||||||||||" << endl;
	map<string, string>::iterator iter;
	iter = mdcdata.begin();
	while (iter != mdcdata.end()) {
		cout << iter->first << ":" << iter->second << endl;
		iter++;
	}
}



SCANF_DATA real_scan_file(const char * fil)
{
	SCANF_DATA out;
	ERR_STA err;

	out.JD_MOD = mdc_mode_table;
	map<string, string> mdcdata;
	regexFile(fil, mdcdata);
	prinMap(mdcdata);
	int tmpmod = 0;
	if ((err = SDG(mdcdata, "mod", tmpmod)) == err_ok) {
		out.JD_MOD = (_JD_MOD)tmpmod;
	} else {
		out.JD_MOD = mdc_mode_table;
	}

	out.manual_flag = 0;
	if ((err = SDG(mdcdata, "manual0", out.manual_deg[0])) == err_ok) {
		out.manual_flag |= 1 << 0;
	}

	if ((err = SDG(mdcdata, "manual1", out.manual_deg[1])) == err_ok) {
		out.manual_flag |= 1 << 1;
	}

	out.correct_flag = 0;
	if ((err = SDG(mdcdata, "correct0", out.correct[0])) == err_ok) {
		out.correct_flag |= 1 << 0;
	}

	if ((err = SDG(mdcdata, "correct1", out.correct[1])) == err_ok) {
		out.correct_flag |= 1 << 1;
	}



	out.set_flg = 0;
	if ((err = SDG(mdcdata, "initspeed0", out.initSpeed[0])) == err_ok) {
		out.set_flg |= diff_init0;
	}

	if ((err = SDG(mdcdata, "initspeed1", out.initSpeed[1])) == err_ok) {
		out.set_flg |= diff_init1;
	}

	if ((err = SDG(mdcdata, "maxspeed0", out.MaxSpeed[0])) == err_ok) {
		out.set_flg |= diff_max0;
	}

	if ((err = SDG(mdcdata, "maxspeed1", out.MaxSpeed[1])) == err_ok) {
		out.set_flg |= diff_max1;
	}

	if ((err = SDG(mdcdata, "phase0", out.Phase[0])) == err_ok) {
		out.set_flg |= diff_phase0;
	}

	if ((err = SDG(mdcdata, "phase1", out.Phase[1])) == err_ok) {
		out.set_flg |= diff_phase1;
	}

	if ((err = SDG(mdcdata, "period0", out.period[0])) == err_ok) {
		out.set_flg |= diff_period0;
	}

	if ((err = SDG(mdcdata, "period1", out.period[1])) == err_ok) {
		out.set_flg |= diff_period1;
	}

	if ((err = SDG(mdcdata, "current0", out.currect[0])) == err_ok) {
		out.set_flg |= diff_currect0;
	}
	if ((err = SDG(mdcdata, "current1", out.currect[1])) == err_ok) {
		out.set_flg |= diff_currect1;
	}
	if ((err = SDG(mdcdata, "ratio0", out.Ratio[0])) == err_ok) {
		out.set_flg |= diff_ratio0;
	}

	if ((err = SDG(mdcdata, "ratio1", out.Ratio[1])) == err_ok) {
		out.set_flg |= diff_ratio1;
	}


	out.get_flg = 0;
	int tmpget = 0;
	if ((err = SDG(mdcdata, "getpar0", tmpget)) == err_ok)
	{
		out.get_flg |= 1 << 0;
	}
	if ((err = SDG(mdcdata, "getpar1", tmpget)) == err_ok)
	{
		out.get_flg |= 1 << 1;
	}
	return out;
}


void merge_data(JD_INFO * pjif, SCANF_DATA & dat)
{
	//手动 自动 
	printf("mod %d man %d\n", dat.JD_MOD, dat.manual_flag);
	if (dat.JD_MOD == mdc_mode_manual && dat.manual_flag) {
		pjif->JD_MOD = mdc_mode_manual;
		if (dat.manual_flag & (1 << 0)) {
			pjif->mdcCtrl[0].manual.trig_set(dat.manual_deg[0]);
		} else {
			pjif->mdcCtrl[0].manual.cpl_flag = 1;
		}

		if (dat.manual_flag & (1 << 1)) {
			pjif->mdcCtrl[1].manual.trig_set(dat.manual_deg[1]);
		} else {
			pjif->mdcCtrl[1].manual.cpl_flag = 1;
		}

	} else if (dat.JD_MOD == mdc_mode_off) {
		pjif->mdcCtrl[0].stop.trig_set();
		pjif->mdcCtrl[1].stop.trig_set();
		pjif->JD_MOD = mdc_mode_off;
	} else {
		pjif->JD_MOD = mdc_mode_table;
	}
	//校正
	if (dat.correct_flag & (1 << 0)) {
		pjif->mdcCtrl[0].correct.trig_set(dat.correct[0]);
	}
	if (dat.correct_flag & (1 << 1)) {
		pjif->mdcCtrl[1].correct.trig_set(dat.correct[1]);
	}
	if (dat.set_flg) {
		for (auto & aim : pjif->mdcCtrl) {
			aim.par.setflg = 0;
			if (aim.parget.succ_flag == 0) {
				aim.parget.retry_num = 0;
			}
		}
	}

	//参数设置
	if (dat.set_flg & diff_init0) {
		pjif->mdcCtrl[0].par.trig_set_init(dat.initSpeed[0]);
	}

	if (dat.set_flg & diff_init1) {
		pjif->mdcCtrl[1].par.trig_set_init(dat.initSpeed[1]);
	}

	if (dat.set_flg & diff_max0) {
		pjif->mdcCtrl[0].par.trig_set_max(dat.MaxSpeed[0]);
	}

	if (dat.set_flg & diff_max1) {
		pjif->mdcCtrl[1].par.trig_set_max(dat.MaxSpeed[1]);
	}


	if (dat.set_flg & diff_phase0) {
		pjif->mdcCtrl[0].par.trig_set_pha(dat.Phase[0]);
	}

	if (dat.set_flg & diff_phase1) {
		pjif->mdcCtrl[1].par.trig_set_pha(dat.Phase[1]);
	}


	if (dat.set_flg & diff_currect0) {
		pjif->mdcCtrl[0].par.trig_set_cur(dat.currect[0]);
	}

	if (dat.set_flg & diff_currect1) {
		pjif->mdcCtrl[1].par.trig_set_cur(dat.currect[1]);
	}

	if (dat.set_flg & diff_ratio0) {
		pjif->mdcCtrl[0].par.trig_set_rat(dat.Ratio[0]);
	}

	if (dat.set_flg & diff_ratio1) {
		pjif->mdcCtrl[1].par.trig_set_rat(dat.Ratio[1]);
	}
	//参数查询
	if (dat.get_flg & (1 << 0)) {
		pjif->mdcCtrl[0].parget.trig_get();
	}
	if (dat.get_flg & (1 << 1)) {
		pjif->mdcCtrl[1].parget.trig_get();
	}
}



