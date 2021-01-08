#include <errHandle/errHandle.h>
#include <string>
#include <SN1V2_com.h>
#include <fstream>
#include <regex>
#include <algorithm>
#include <time_interval.h>
#include "jd_share.h"


#include <string.h>
#include <math.h>
using namespace std;

static shared_ptr< vector< CREOBJ>> Get_CRE(std::string & loadbin);

static int sizeOfNaN(double(&ppp)[8])
{
	int sz = 0;
	for (auto & p : ppp)
	{
		if (std::isnan(p))
			sz++;
	}
	return sz;
}

static 	bool _creData(CREOBJ &a, CREOBJ & b)
{
#if 0
	if (a.year > b.year)
		return true;

	if (a.month > b.month)
		return true;

	if (a.day > b.day)
		return true;


	return false;
#else
	return a.capTime > b.capTime;
#endif
}


ERR_STA CalCre(const char * resName, CREOBJ & creData, const double SPS, const double SFL)
{
	time_t ttt = time(0);
	tm tNow;

	localtime_r(&ttt, &tNow);
	tNow.tm_year = tNow.tm_year + 1900;
	tNow.tm_mon = tNow.tm_mon + 1;
	tNow.tm_mday = tNow.tm_mday;

	{
		creData.year = tNow.tm_year;
		creData.month = tNow.tm_mon;
		creData.day = tNow.tm_mday;		

		__attribute__((unused)) 
		char * CREstr = CRE(resName, SPS, SFL, creData.errData);
		cout << CREstr << endl;
		if (sizeOfNaN(creData.errData)) {//无效数据
			SN1V2_ERR_LOG("error = err_cre_get_NAN");
			SN1V2_ERROR_CODE_RET(err_cre_get_NAN);
		} else {//有效数据
			//log cre change
			char chbuff[512];
			sprintf(chbuff, "cre data from sps = %lf ,sfl = %lf,org cre = %lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf to %s"
				, SPS, SFL
				, creData.errData[0], creData.errData[1], creData.errData[2], creData.errData[3]
				, creData.errData[4], creData.errData[5], creData.errData[6], creData.errData[7]
				, CREstr
			);

			SN1V2_ERR_LOG(chbuff);
			creData.capTime++;
		}
	}
	return err_ok;
}

ERR_STA save_cre(const char * creName, std::vector <CREOBJ> & creGrp, const int PID)
{
	std::sort(creGrp.begin(), creGrp.end(), _creData);

	ofstream of(creName, ios::ate);

	unsigned char buff[3];
	memcpy(buff, &PID, 3);

	char outbuff[20];

	JD_Name_transfer(buff, outbuff, sizeof(outbuff));

	if (of.good()) {
		char buff[512];

		for (auto & creData : creGrp) {
			snprintf(buff, 512, "%04d-%02d-%02d"
				",%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%.1lf,%.1lf,%.1lf,%.2lf,%.2lf,%.2lf"//14ge
				",SN1_%s,%d"  
				, creData.year, creData.month, creData.day
				, creData.errData[0], creData.errData[1], creData.errData[2], creData.errData[3]
				, creData.errData[4], creData.errData[5], creData.errData[6], creData.errData[7]
				, creData.extra[0], creData.extra[1], creData.extra[2]
				, creData.extra[3], creData.extra[4], creData.extra[5]
				, outbuff, creData.capTime);

			LOG(INFO) << buff;
			of << buff << endl;
		}
	} else {
		SN1V2_ERR_LOG("can't open cre.txt");
		SN1V2_ERROR_CODE_RET(err_cannot_open_file);
	}
	return err_ok;
}


static shared_ptr< vector< CREOBJ>> getLastErrConfig(const char * creName)
{
	ERR_STA err;
	string loadbin;
	if ((err = loadFile((char*)creName, loadbin)) == err_ok)
		return Get_CRE(loadbin);
	
	return make_shared< vector< CREOBJ>>();
}



shared_ptr< vector <CREOBJ>> initCre(const char * creName)
{
	auto errConfig = getLastErrConfig(creName);

	if (0 == errConfig->size())
		LOG(WARNING) << "CRE:get null";
	else
		std::sort(errConfig->begin(), errConfig->end(), _creData);

	return errConfig;
}

bool ChkTodayCre(vector<CREOBJ> & creDataGrp)
{
	time_t ttt = time(0);
	tm tNow;

	localtime_r(&ttt, &tNow);
	tNow.tm_year = tNow.tm_year + 1900;
	tNow.tm_mon = tNow.tm_mon + 1;
	tNow.tm_mday = tNow.tm_mday;


	for (auto & cre : creDataGrp) {
		if ((tNow.tm_year == cre.year)
			&& (tNow.tm_mon == cre.month)
			&& (tNow.tm_mday == cre.day)) {
			return true;
		}
	}
	return false;
}




CREOBJ GetNewCre(vector<CREOBJ> & creDataGrp)
{
	CREOBJ  creData;

	if (creDataGrp.size())
		creData = creDataGrp.at(0);
	else
		for (auto & p : creData.errData)
			p = 0.0f;

	return creData;
}
#include <string.h>
static shared_ptr< vector< CREOBJ>> Get_CRE(std::string & loadbin)
{
	regex reg("\\d{4}-\\d{1,2}-\\d{1,2}(?:(?:,-?\\d*(?:\\.\\d+)?){14}),SN1_(\\d+),\\d+");
	smatch match;

	string::const_iterator star = loadbin.begin();
	string::const_iterator end = loadbin.end();

	auto retPtr = make_shared< vector < CREOBJ>>();
	while (regex_search(star, end, match, reg)) {
		CREOBJ creObj;

		double(&val)[8] = creObj.errData;
		int snum;
		int dumyPID;
		if (19 == (snum = sscanf(match[0].str().c_str(),
			"%d-%d-%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf"
			",%lf,%lf,%lf,%lf,%lf,%lf"
			",SN1_%u,%u"
			, &creObj.year, &creObj.month, &creObj.day
			, &val[0], &val[1]
			, &val[2], &val[3]
			, &val[4], &val[5]
			, &val[6], &val[7]
			, &creObj.extra[0], &creObj.extra[1], &creObj.extra[2]
			, &creObj.extra[3], &creObj.extra[4], &creObj.extra[5]
			, &dumyPID
			, &creObj.capTime
		))) {
			strncpy(creObj.pidbuff, match[1].str().c_str(), sizeof(creObj.pidbuff));
			retPtr->push_back(creObj);
		}
		star = match[0].second;
	}
	return retPtr;
}


