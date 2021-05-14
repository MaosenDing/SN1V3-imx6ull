#include "SN1V2_com.h"
#include <regex.h>
#include <iostream>
#include <string.h>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include "errHandle/errHandle.h"
#include <algorithm>
#include <mutex>
using namespace std;


static std::mutex mtx;
map<string, string> sysData;
#if 1
#include <regex>
ERR_STA SDS(const char * fileName)
{
	std::unique_lock<std::mutex> lk(mtx);
	string stText;
	ERR_STA sta = loadFile((char *)fileName, stText);

	if (sta != err_ok)
	{
		SN1V2_ERROR_MWSSAGE_WITH("load error", sta);
		return sta;
	}

	//%ABC,dsofsii
	//%CDF,-2948.3994
	//static regex reg("%([A-Z]{3}),((?:\\w+)|(?:-?\\d*(?:.\\d*)))");
	static regex reg("%([A-Z]{3}),([\\d\\w.+-/]*)");
	smatch match;

	string::const_iterator star = stText.begin();
	string::const_iterator end = stText.end();
	while (regex_search(star, end, match, reg))
	{
#if 0
		fprintf(stderr, "Name = %s,config = %s\n", match[1].str().c_str(), match[2].str().c_str());
#endif
		//pair<string, string> tmpp(match[1], match[2]);
		//auto ret = sysData.insert(tmpp);
		auto ret = sysData.emplace(match[1], match[2]);

		if (false == ret.second)
		{
			LOG(WARNING) << "error insert ,key = " << match[1] << ",val= " << match[2] << ",org val =" << sysData[match[1]];
		}
		star = match[0].second;
	}

	if (sysData.size() == 0)
	{
		SN1V2_ERROR_CODE_RET(err_conf_get_null);
	}
	return err_ok;
}


#else
static regex_t * getTableRegex(void)
{
	static bool flag = 0;
	static regex_t reg;

	if (!flag)
	{
		const char * pattern = "%[A-Z]{3},([A-Za-z0-9+-.]*)(\r|\n|\r\n|\n\r)";
		if (0 == regcomp(&reg, pattern, REG_EXTENDED | REG_NEWLINE))
		{
			flag = 1;
		}
		else
		{
			return 0;
		}
	}
	return &reg;
}



static void getRegex(const string & readSt, regex_t * Reg, vector<string> & outVec)
{
	outVec.clear();
	size_t regPos = 0;
	try
	{
		while (1)
		{
			const size_t nmatch = 1;
			regmatch_t pmatch[nmatch];

			memset(pmatch, 0, sizeof(regmatch_t) * nmatch);

			int status = regexec(Reg, &readSt[regPos], nmatch, pmatch, 0);

			if (status == REG_NOMATCH)
			{
				return;
			}
			else if (status == 0)
			{
				regmatch_t & refm = pmatch[0];

				if (refm.rm_eo > 0)
				{
					string st = readSt.substr(refm.rm_so + regPos, refm.rm_eo - refm.rm_so);
					regPos += refm.rm_eo;
					outVec.push_back(st);

					if (regPos > readSt.size())
						return;
				}
			}
		}
	}
	catch (std::out_of_range & p)
	{
		cout << "error range" << endl;
		return;
	}
}

static void getMapFromVector(map<string, string> & outMap, const vector<string> &inVector)
{
	outMap.clear();
	for (size_t i = 0; i < inVector.size(); i++)
	{
		const string & tmpst = inVector[i];

		char * nameBuff[24] = { 0 };
		char * ValBuff[24] = { 0 };

		sscanf(tmpst.data() + 1, "%[^,],%s", (char *)nameBuff, (char *)ValBuff);
		//cout << "string=" << tmpst << endl;
		//cout << "name =" << (char *)nameBuff << endl;
		//cout << "val =" << (char *)ValBuff << endl;

		//pair<string, string> tmpp((char *)(nameBuff), (char *)(ValBuff));
		//auto ret = sysData.insert(tmpp);
		auto ret = sysData.emplace((char *)(nameBuff), (char *)(ValBuff));

		if (false == ret.second)
		{
			LOG(WARNING) << "error insert ,key = " << nameBuff << ",val= " << ValBuff << ",org val =" << sysData[nameBuff];
		}

		outMap.insert(tmpp);
	}
}
ERR_STA SDS(const char * fileName)
{
	std::unique_lock<std::mutex> lk(mtx);
	string stText;
	ERR_STA sta = loadFile((char *)fileName, stText);
	
	if (sta != err_ok)
	{
		SN1V2_ERROR_MWSSAGE_WITH("load error", sta);
		return sta;
	}
#if 0
	cout << "#####read start#####" << endl;
	cout << stText << endl;
	cout << "#####read end#####" << endl;
#endif
	regex_t * pReg = getTableRegex();
	if (pReg == 0)
	{
		SN1V2_ERROR_CODE_RET(err_tim_analysis_error);
	}

	vector<string> vecout;
	getRegex(stText, pReg, vecout);

	if (vecout.size() == 0)
	{
		SN1V2_ERROR_CODE_RET(err_conf_get_null);
	}
#if 0
	for (size_t i = 0; i < vecout.size(); i++)
	{
		cout << "i =" << i << ",str=" << vecout[i] << endl;
	}
#endif	
	getMapFromVector(sysData, vecout);

	return err_ok;
}
#endif


ERR_STA SDG(const char * inkey, string & outData)
{
	std::unique_lock<std::mutex> lk(mtx);
	try
	{
		outData = sysData.at(inkey);
	}
	catch (out_of_range & p)
	{
		outData.clear();
		LOG(WARNING) << "log key = " << inkey << ",not found" << endl;
		SN1V2_ERROR_CODE_RET(err_conf_get_null);
	}
	return err_ok;
}

ERR_STA SDG(const char * inkey, char * outData)
{
	string st;
	ERR_STA err = SDG(inkey, st);
	if (err != err_ok) {
		return err;
	}
	strcpy(outData, st.c_str());
	return err_ok;
}


ERR_STA SDG(const char * inkey, int & outInt)
{
	string st;
	ERR_STA err = SDG(inkey, st);

	if (err != err_ok) {
		return err;
	}

	if (sscanf(st.c_str(), "%d", &outInt) == 1) {
		return err_ok;
	} else {
		SN1V2_ERROR_CODE_RET(err_conf_tran_error);
	}
}

ERR_STA SDG(const char * inkey, unsigned int & outInt)
{
	string st;
	ERR_STA err = SDG(inkey, st);

	if (err != err_ok) {
		return err;
	}

	if (sscanf(st.c_str(), "%d", &outInt) == 1) {
		return err_ok;
	} else {
		SN1V2_ERROR_CODE_RET(err_conf_tran_error);
	}
}


ERR_STA SDG(const char * inkey, float & outfloat)
{
	string st;
	ERR_STA err = SDG(inkey, st);

	if (err != err_ok) {
		return err;
	}

	if (sscanf(st.c_str(), "%f", &outfloat) == 1) {
		return err_ok;
	} else {
		SN1V2_ERROR_CODE_RET(err_conf_tran_error);
	}
}

ERR_STA SDG(const char * inkey, bool & outbool)
{
	string st;
	ERR_STA err = SDG(inkey, st);

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

	SN1V2_ERROR_CODE_RET(err_conf_tran_error);
}



ERR_STA getConf(const char * configName
	, SN1_CFG * cfg, int showflag
)
{
	ERR_STA err;
	if ((err = SDS(configName)) != err_ok)
		return err;

	if ((err = SDG("SGT", cfg->thres)) != err_ok)
		return err;
	if (showflag) cout << "SGT=" << cfg->thres << endl;

	if ((err = SDG("SBT", cfg->thresPer)) != err_ok)
		return err;
	if (showflag) cout << "SBT=" << cfg->thresPer << endl;

	if ((err = SDG("TLO", cfg->Lon)) != err_ok)
		return err;
	if (showflag) cout << "TLO=" << cfg->Lon << endl;

	if ((err = SDG("TLA", cfg->lati)) != err_ok)
		return err;
	if (showflag) cout << "TLA=" << cfg->lati << endl;

	if ((err = SDG("SRT", cfg->qualityThres)) != err_ok)
		return err;
	if (showflag) cout << "SRT=" << cfg->qualityThres << endl;

	if ((err = SDG("SSA", cfg->SSA)) != err_ok)
		return err;
	if (showflag) cout << "SSA=" << cfg->SSA << endl;

	if ((err = SDG("SHT", cfg->SHT)) != err_ok)
		return err;
	if (showflag) cout << "SHT=" << cfg->SHT << endl;

	if ((err = SDG("SPT", cfg->SPT)) != err_ok)
		return err;
	if (showflag) cout << "SPT=" << cfg->SPT << endl;

	if ((err = SDG("TDT", cfg->TDT)) != err_ok)
		return err;
	if (showflag) cout << "TDT=" << cfg->TDT << endl;

	if ((err = SDG("SCH", cfg->SCH)) != err_ok)
		return err;
	if (showflag) cout << "SCH=" << cfg->SCH << endl;

	if ((err = SDG("TEE", cfg->TEE)) != err_ok)
		return err;
	if (showflag) cout << "TEE=" << cfg->TEE << endl;

	if ((err = SDG("TEP", cfg->TEP)) != err_ok)
		return err;
	if (showflag) cout << "TEP=" << cfg->TEP << endl;

	if ((err = SDG("TET", cfg->TET)) != err_ok)
		return err;
	if (showflag) cout << "TET=" << cfg->TET << endl;

	if ((err = SDG("SFL", cfg->SFL)) != err_ok)
		return err;
	if (showflag) cout << "SFL=" << cfg->SFL << endl;

	if ((err = SDG("SPS", cfg->SPS)) != err_ok)
		return err;
	if (showflag) cout << "SPS=" << cfg->SPS << endl;

	if ((err = SDG("SID", cfg->SID)) != err_ok)
		return err;
	if (showflag) cout << "SID=" << cfg->SID << endl;

	if ((err = SDG("SIH", cfg->IMG_HEIGTH)) != err_ok)
		return err;
	if (showflag) cout << "SIH=" << cfg->IMG_HEIGTH << endl;

	if ((err = SDG("SIW", cfg->IMG_WIDTH)) != err_ok)
		return err;
	if (showflag) cout << "SIW=" << cfg->IMG_WIDTH << endl;

	bool flag;

	if ((err = SDG("SVO", flag)) != err_ok)
		return err;
	cfg->FLAG_SAVE_ORG = flag;
	if (showflag) cout << "SVO=" << cfg->FLAG_SAVE_ORG << endl;

	if ((err = SDG("SVB", flag)) != err_ok)
		return err;
	cfg->FLAG_SAVE_BIN = flag;
	if (showflag) cout << "SVB=" << cfg->FLAG_SAVE_BIN << endl;

	if ((err = SDG("MCP", cfg->MinCntGrp)) != err_ok)
		return err;
	if (showflag) cout << "MCP=" << cfg->MinCntGrp << endl;


	if ((err = SDG("SVO", flag)) != err_ok)
		return err;
	cfg->FLAG_SAVE_ORG = flag;
	if (showflag) cout << "SVO=" << cfg->FLAG_SAVE_ORG << endl;

	if ((err = SDG("SHS", flag)) != err_ok)
		return err;
	cfg->SaveTimeTable = flag;
	if (showflag) cout << "SHS=" << cfg->SaveTimeTable << endl;

	if ((err = SDG("SFD", flag)) != err_ok)
		return err;
	cfg->CleanLastDate = flag;
	if (showflag) cout << "SFD=" << cfg->CleanLastDate << endl;


	if ((err = SDG("SGV", cfg->gain)) != err_ok)
		return err;
	if (showflag) cout << "SGV=" << cfg->gain << endl;

	if ((err = SDG("SST", cfg->expo)) != err_ok)
		return err;
	if (showflag) cout << "SST=" << cfg->expo << endl;
	//图像翻转类型设置
	if ((err = SDG("OVR", cfg->overturn)) != err_ok)
		return err;
	if (showflag) cout << "OVR=" << cfg->overturn << endl;

	//sn1采集数据保存天数----dms--20210513
	if (SDG("SDN", cfg->sn1_store_days) != err_ok) {
		cfg->sn1_store_days = 5;
	}
//	if (showflag) cout << "SDN=" << cfg->sn1_store_days << endl;
		
	//sn3 设备参数
	if (SDG("NGV", cfg->sn3_gain) != err_ok) {
		cfg->sn3_gain = cfg->gain;
	}

	if (SDG("NST", cfg->sn3_expose) != err_ok) {
		cfg->sn3_expose = cfg->expo;
	}

	//水平翻转
	if ((err = SDG("SHF", flag)) != err_ok) {
		return err;
	}
	cfg->isHorisFlip = flag;
	printf("isHorisFlip %d\n", cfg->isHorisFlip);


	//竖直翻转
	if ((err = SDG("SVF", flag)) != err_ok) {
		return err;
	}
	cfg->isVeriFlip = flag;
	printf("isVeriFlip %d\n", cfg->isVeriFlip);
	//下午阈值
	if ((err = SDG("AFT", cfg->afnoon_thres)) != err_ok) {
		cfg->afnoon_thres = 0.4;
	}


	//上午阈值
	if ((err = SDG("BFT", cfg->bfnoon_thres)) != err_ok) {
		cfg->bfnoon_thres = 0.4;
	}
	cout << "cap threshold = " << cfg->bfnoon_thres << "," << cfg->afnoon_thres << endl;

	//长拍摄保留时间
	if ((err = SDG("MRT", cfg->max_reserve_time)) != err_ok) {
		cfg->max_reserve_time = 5;//默认5秒
		cout << "default cap max reserve time = " << cfg->max_reserve_time << endl;
	} else {
		cout << "cap max reserve time = " << cfg->max_reserve_time << endl;
	}

	if ((err = SDG("ISC", cfg->IsSaveCre)) != err_ok) {
		//默认保存 ，特殊测试需要不保存
		cfg->IsSaveCre = 1;
		cout << " default:save cre" << endl;
	} else {
		cout << "debug mode : do not save cre" << endl;
	}

	if ((err = SDG("LHB", cfg->limitHourBeforeNoon)) != err_ok) {
		//默认保存 ，特殊测试需要不保存
		cfg->limitHourBeforeNoon = -1;
		cout << "no beforeNoon time limit" << endl;
	} else {
		cout << "beforeNoon time limit =" << cfg->limitHourBeforeNoon << endl;
	}

	if ((err = SDG("LHA", cfg->limitHourAfterNoon)) != err_ok) {
		//默认保存 ，特殊测试需要不保存
		cfg->limitHourAfterNoon = -1;
		cout << "no beforeNoon time limit" << endl;
	} else {
		cout << "beforeNoon time limit =" << cfg->limitHourAfterNoon << endl;
	}
	//照片保存路径
	//设置后强制开启保存功能
	if ((err = SDG("FSP", cfg->ForceSavePath)) != err_ok) {
		memset(cfg->ForceSavePath, 0, 10);
	}

	if (strlen(cfg->ForceSavePath)) {
		cout << "force save in path:" << cfg->ForceSavePath << endl;
	} else {
		cout << "normal save" << endl;
	}
	return err_ok;
}



























