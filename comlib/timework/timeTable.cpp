#include "SN1V2_com.h"

#include <iostream>
#include <string>
#include <regex>

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "errHandle/errHandle.h"
#include "time_interval.h"
#include <regex.h>
#include "tableWork.h"
#include <regex>
using namespace std;




bool is_valid_date(int y, int m, int d)
{
	int month[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) {
		month[1] = 29;
	}
	return y > 1900 && y < 2030 && m > 0 && m <= 12 && d > 0 && d <= month[m - 1];
}

bool is_valid_daytim(int hour, int min, int sec)
{
	return hour >= 0 && hour < 24
		&& min >= 0 && min < 60
		&& sec >= 0 && sec < 60;
}






static inline bool checkTime_type2(timTableSet & reftm)
{
	return 1
		&& is_valid_daytim(reftm.tm_hour, reftm.tm_min, reftm.tm_sec) 
		//&& is_valid_date(reftm.tm_year, reftm.tm_mon, reftm.tm_mday)
		;
}

static inline void getTmType_underLine(const char * pos, timTableSet & rtm)
{
	sscanf(pos, "%d_%d_%d",
		&rtm.tm_hour, &rtm.tm_min, &rtm.tm_sec);
}

static inline void getTmType_colon(const char * pos, timTableSet & rtm)
{
	sscanf(pos, "%d:%d:%d",
		&rtm.tm_hour, &rtm.tm_min, &rtm.tm_sec);
}

static inline void getTmType_fullConfig(const char * pos, timTableSet & rtm)
{
	sscanf(pos, "%d:%d:%d,%f,%f,%f,%f,%f"
		,&rtm.tm_hour, &rtm.tm_min, &rtm.tm_sec
		,&rtm.ZxAng,&rtm.YxAng
		,&rtm.RIx,&rtm.RIy,&rtm.RIz
		);
}

ERR_STA load_table(const char * filename, std::vector<timTableSet> & outTable)
{
	ERR_STA err = err_UNKNOWN;
	cout << "load type 2" << endl;
	if (filename) {
		string loadbin;

		if ((err = loadFile(filename, loadbin)) == err_ok) {
			//匹配 2018-5-14 15:14:13,-1.1,-2.2,-3.3,-4.4,-5.5
			//匹配年月日，但不使用 也不对其正确性做判断
			regex reg("\\d{4}-\\d{1,2}-\\d{1,2} (\\d{1,2}):(\\d{1,2}):(\\d{1,2})((?:,-?\\d*(?:\\.\\d+)?){5})");
			smatch match;

			outTable.reserve(loadbin.size() / 60);
			{
				string::const_iterator star = loadbin.begin();
				string::const_iterator end = loadbin.end();
				while (regex_search(star, end, match, reg)) {
#if 0
					int num = match.size();
					cout << "size = " << num << endl;
					for (int i = 0; i < num; i++) {
						cout << match[i] << endl;
					}
					cout << "----------------------------" << endl;
					/* 典型值
					size = 5
					2018 - 05 - 14 05:14 : 50, 106.213, 80.263, -0.275194, 0.946397, 0.169119
					05
					14
					50
					, 106.213, 80.263, -0.275194, 0.946397, 0.169119
					*/
#endif
					timTableSet rtm;

					char *pos = (char *)&(*match[1].first);
					int cnt;
					if ((cnt = sscanf(pos, "%d:%d:%d,%f,%f,%f,%f,%f"
						, &rtm.tm_hour, &rtm.tm_min, &rtm.tm_sec
						, &rtm.ZxAng, &rtm.YxAng
						, &rtm.RIx, &rtm.RIy, &rtm.RIz
					)) == 8) {
						if (checkTime_type2(rtm)) {
							outTable.emplace_back(rtm);
						} else {
							SN1V2_ERR_LOG("time check error,%d:%d : %d, %f, %f, %f, %f, %f"
								, rtm.tm_hour, rtm.tm_min, rtm.tm_sec
								, rtm.ZxAng, rtm.YxAng
								, rtm.RIx, rtm.RIy, rtm.RIz);
						}
					} else {
						SN1V2_ERR_LOG("scanf cnt = %d", cnt);
					}
					star = match[0].second;
				}
			}
		} else {
			SN1V2_ERROR_MWSSAGE_WITH("load err", err);
			return err;
		}
	} else {
		SN1V2_ERROR_CODE_RET(err_inval_path);
	}
	return err;
}

//////////////////////////////////////////////////////////////////////////////////
static void rm_past_tm(vector<timTableSet> & table)
{
	tm  thisTm;
	ERR_STA err = GetTim(thisTm);
	time_t tms = mktime(&thisTm);

	if (err != err_ok || tms < 0)
	{
		return;
	}


	auto size0 = table.size();

	while (true)
	{
		if (table.empty())
		{
			LOG(WARNING) << "rm null";
			break;
		}
		time_t tmpt = table.back().tt;
		if (tmpt < tms)
		{
			table.pop_back();
		}
		else
		{
			break;
		}
	}
	auto size1 = table.size();

#if 0
	auto & outTable = table;
	for (size_t i = 0; i < 5; i++)
	{
		time_t tt = outTable.at(outTable.size() - 1 - i).tt;
		tm ref;
		localtime_r(&tt, &ref);

		fprintf(stdout, "year = %d,mon = % d,day = %d""hour = %d,min = % d,sec = %d\n"
			, ref.tm_year + 1900, ref.tm_mon + 1, ref.tm_mday
			, ref.tm_hour, ref.tm_min, ref.tm_sec);
	}
#endif
	LOG(INFO) << "rm size = " << size0 - size1;
}



static ERR_STA load_table_fixed(vector<timTableSet> & outTable)
{
	std::sort(outTable.begin(), outTable.end(), [](const timTableSet & a, const timTableSet &  b) {return a.tt > b.tt; });
#if 0
	for (size_t i = 0; i < 5; i++) {
		time_t tt = outTable.at(i).tt;
		tm ref;
		localtime_r(&tt, &ref);

		fprintf(stdout, "year = %d,mon = % d,day = %d""hour = %d,min = % d,sec = %d\n"
			, ref.tm_year + 1900, ref.tm_mon + 1, ref.tm_mday
			, ref.tm_hour, ref.tm_min, ref.tm_sec);
	}
#endif

	return err_ok;
}




int timTableWork(vector<timTableSet> & table, setWork & test)
{
	load_table_fixed(table);

	rm_past_tm(table);
	table.shrink_to_fit();

	int sz = table.size();
	LOG(INFO) << "table size =" << table.size();

	while (!table.empty())
	{
		auto & cap = table.back();
		time_t captime = cap.tt;

		ERR_STA err = TimDelayUntil(captime);

		if (err == err_ok)
		{
			test.work(cap, 0);
		}
		else
		{
			test.work(cap, 1);
		}
		table.pop_back();
	}
	if (sz) {
		LOG(INFO) << "cap size = " << sz;
	}

	return sz;
}


int timTableWorkWithReserveTime(vector<timTableSet> & table, setWork & test , int TimeS)
{
	load_table_fixed(table);

	rm_past_tm(table);
	table.shrink_to_fit();

	int sz = table.size();
	LOG(INFO) << "table size =" << table.size();

	while (!table.empty()) {
		auto & cap = table.back();
		time_t captime = cap.tt;
		//printf("captime = cap.tt = %ld \n", captime);

		ERR_STA err = TimDelayUntil(captime);

		if (err == err_ok) {
			test.work(cap, 0);
			//printf("cap work ----------------1\n");
		} else {
			err = TimDelayUntil(captime + TimeS);

			if (err == err_ok) {
				SN1V2_WARN_LOG("cap over head tick");
				test.work(cap, 0);
				//printf("cap work ----------------2\n");
			} else {
				SN1V2_WARN_LOG("cap over tail tick");
				test.work(cap, 1);
				//printf("cap work ----------------3\n");
			}
		}
		table.pop_back();
	}
	if (sz) {
		LOG(INFO) << "cap size = " << sz;
	}

	return sz;
}


int dummyLongTimeCapTest(setWork & test, int period)
{
	time_t tstart = time(0);
	uint32_t sz;

	while (true) {
		tstart += period;
		tm tmptm;

		localtime_r(&tstart, &tmptm);

		timTableSet tts;
		tts.tt = tstart;
		tts.tm_hour = tmptm.tm_hour;
		tts.tm_min = tmptm.tm_min;
		tts.tm_sec = tmptm.tm_sec;

		ERR_STA err = TimDelayUntil(tstart);

		if (err == err_ok) {
			test.work(tts, 0);
			sz++;
		} else {
			SN1V2_WARN_LOG("cap over tick");
		}
	}
	return sz;
}



ERR_STA GetTableSet(char * fName, CREOBJ & creDate
	, const int year, const int mon, const int day
	, const float Lon, const float lati
	, vector<timTableSet> &timeset
	, const float SSA
	, const float SHT
	, const float SPT
	, const float TDT
	, const float SCH
	, const float TEE
	, const float TEP
	, const float TET
	, const unsigned int SaveTimeTable
)
{
	ERR_STA err;
	bool rebuildFlag = false;

	//vector	<timTableSet> timeset;

	struct stat stbuff;
	if ((stat(fName, &stbuff) == -1) ||
		(stbuff.st_size < 20000)) {
		//rebuild table
		cout << "create table" << endl;;
		TimeInterval p("create table:");

		err = SHG(year, mon, day
			, SHT, SPT, SSA, Lon, lati, TEE, TEP, TET
			, TDT, SCH
			, creDate.errData
			, timeset);
		//err = SHG(year, mon, day
		//	, 3, 0, 5, Lon, lati, 1400, 1, 32
		//	, 0, 0
		//	, errConf
		//	, timeset);
		if (err != err_ok)
			return err;

		rebuildFlag = true;
	} else {//scanf table
		cout << "scanf table" << endl;;
		TimeInterval ppp("scanf table:");

		if ((err = load_table(fName, timeset)) != err_ok)
			return err;
	}

	tm  thisTm;
	GetTim(thisTm);
	thisTm.tm_hour = 0;
	thisTm.tm_min = 0;
	thisTm.tm_sec = 0;
	time_t tms = mktime(&thisTm);
	for (auto & rtm : timeset) {
		rtm.tt = tms + rtm.tm_hour * 3600 + rtm.tm_min * 60 + rtm.tm_sec;
	}

	{
		cout << "make table" << endl;
		TimeInterval ppp("make table:");

		if ((rebuildFlag == true) && (SaveTimeTable > 0)) {
			if ((err = save_timTableSet(fName, year, mon, day, timeset)) != err_ok)
				return err;
		}

#ifdef force_time_table_save_path
		save_timTableSet(force_time_table_save_path, year, mon, day, timeset);
#endif

		cout << "size=" << timeset.size() << endl;
		return err_ok;
	}
	return err_UNKNOWN;
}




void RmTimeTableForTimeNotSuit(vector <timTableSet> & timeset, int LimitBeforeNoon, int LimitAfterNoon)
{
	SN1V2_WARN_LOG("org timeset = %d\n", timeset.size());

#define SP_BEFORE_NOON -1
#define SP_AFTER_NOON 30

	if ((LimitBeforeNoon >= 24) || (LimitBeforeNoon < 0)) {
		LimitBeforeNoon = SP_BEFORE_NOON;
	}


	if ((LimitAfterNoon >= 24) || (LimitAfterNoon < 0)) {
		LimitAfterNoon = SP_AFTER_NOON;
	}

	if ((LimitBeforeNoon == SP_BEFORE_NOON) && (LimitAfterNoon == SP_AFTER_NOON)) {
		SN1V2_WARN_LOG("fixed null\n");
		return;
	}


	vector <timTableSet> tmpset;


	for (auto &p : timeset) {
		if ((p.tm_hour >= LimitBeforeNoon) && (p.tm_hour < LimitAfterNoon)) {
			tmpset.push_back(p);
		}
	}
	timeset.swap(tmpset);

	SN1V2_WARN_LOG("fixed timeset = %d\n", timeset.size());
}






