#include "SN1V2_com.h"

#include <iostream>
#include <string>
#include <regex>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "errHandle/errHandle.h"
#include "time_interval.h"
#include <regex.h>
#include "tableWork.h"
using namespace std;



#if 0
static inline bool is_valid_data(int y, int m, int d)
{
	int month[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)
	{
		month[1] = 29;
	}
	return y > 1900 && y < 2030 && m > 0 && m <= 12 && d > 0 && d <= month[m - 1];
}
#endif

static inline bool is_valid_daytim(int hour, int min, int sec)
{
	return hour >= 0 && hour < 24
		&& min >= 0 && min < 60
		&& sec >= 0 && sec < 60;
}






static inline bool checkTime_type2(timTableSet & reftm)
{
	return 1
		&& is_valid_daytim(reftm.tm_hour, reftm.tm_min, reftm.tm_sec) 
		//&& is_valid_data(reftm.tm_year, reftm.tm_mon, reftm.tm_mday)
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




static inline regex_t * getRegType_underLine(void)
{
	static bool flag = 0;
	static regex_t reg;

	if (!flag)
	{
		const char * pattern = "([0-9]{1,2}_[0-9]{1,2}_[0-9]{1,2})";
		if (0 == regcomp(&reg, pattern, REG_EXTENDED))
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

static inline regex_t * getRegType_colon(void)
{
	static bool flag = 0;
	static regex_t reg;

	if (!flag)
	{
		const char * pattern = "([0-9]{1,2}:[0-9]{1,2}:[0-9]{1,2})";
		if (0 == regcomp(&reg, pattern, REG_EXTENDED))
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

static inline regex_t * getRegType_full(void)
{
	static bool flag = 0;
	static regex_t reg;

	if (!flag)
	{
		const char * pattern = "([0-9]{1,2}:[0-9]{1,2}:[0-9]{1,2})";
		if (0 == regcomp(&reg, pattern, REG_EXTENDED))
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


static int regTmType_2(const char * startPos, timTableSet & rtm ,int & endpos, scanf_type intype)
{
	const size_t nmatch = 1;
	regmatch_t pmatch[nmatch];

	pmatch[0].rm_eo = 0;
	pmatch[0].rm_so = 0;
	//memset(pmatch, 0, sizeof(regmatch_t) * nmatch);

	regex_t * pRegType_2;
		
	switch (intype)
	{
	case type_with_colon:
		pRegType_2 = getRegType_colon();
		break;
	case type_with_underline:
		pRegType_2 = getRegType_underLine();
		break;
	case type_with_full_config:
		pRegType_2 = getRegType_full();
		break;
	default:
		return -1;
	}
	

	int status = regexec(pRegType_2, startPos, nmatch, pmatch, 0);

	if (status == REG_NOMATCH)
	{
		//cout << "no match" << endl;
		return -1;
	}
	else if (status == 0)
	{
		regmatch_t & refm = pmatch[0];

		if (refm.rm_eo != 0)
		{
			endpos = refm.rm_eo;
			const char * st = &startPos[refm.rm_so];

			switch (intype)
			{
			case type_with_colon:
				getTmType_colon(st, rtm);
				break;
			case type_with_underline:
				getTmType_underLine(st, rtm);
				break;
			case type_with_full_config:
				getTmType_fullConfig(st, rtm);
				break;
			default:
				break;
			}
			if (true == checkTime_type2(rtm))
			{
				return 0;
			}
			else
			{
				return 1;
			}
		} 
	}
	return -3;
}


static int txt2table(char * intable, unsigned int txtlen,scanf_type intype ,vector<unsigned int> & outTable)
{
	if (intable && txtlen)
	{
		int len = 0;
		for (size_t pos = 0; pos < txtlen;pos += len)
		{
			timTableSet tmp;

			int reg;
			switch (intype)
			{
			case type_with_colon:
			case type_with_underline:
				reg = regTmType_2(&intable[pos], tmp, len, intype);
			default:
				return -2;
			}			

			if (reg < 0)
			{
				return 0;
			}

			if (reg == 0)
			{
				unsigned int tmpint = tmp.tm_hour * 3600 + tmp.tm_min * 60 + tmp.tm_sec;
				outTable.push_back(tmpint);
			}

			if (reg > 0)
			{
				continue;
			}			
		}
	}
	return -1;
}


static int txt2table(char * intable, unsigned int txtlen, vector<timTableSet> & outTable)
{
	if (intable && txtlen)
	{
		int len = 0;
		for (size_t pos = 0; pos < txtlen; pos += len)
		{
			timTableSet tmp;

			int reg = regTmType_2(&intable[pos], tmp, len, scanf_type::type_with_full_config);

			if (reg < 0)
			{
				return 0;
			}

			if (reg == 0)
			{
				outTable.emplace_back(tmp);
			}

			if (reg > 0)
			{
				continue;
			}
		}
	}
	return -1;
}


int load_table(char * filename, vector<unsigned int> & outTable ,scanf_type intype)
{
	int res = -1;
	if (filename)
	{
		vector<unsigned char> loadbin;
		int ret = loadFile(filename, loadbin);

		char *txtbuff = (char *)&loadbin.at(0);
		int txtlen = loadbin.size();

		if (ret == err_ok)
		{
			SN1V2_ERR_LOG("load ok len = %d", txtlen);
			outTable.reserve(txtlen / 8);
			{
				//TimeInterval p("test table:");
				return txt2table(txtbuff, txtlen, intype, outTable);
			}
		}
		else
		{
			SN1V2_ERR_LOG("load error code = %d", ret);
			res = -2;
		}
	}
	return res;
}
#if 0
ERR_STA load_table(char * filename, std::vector<timTableSet> & outTable)
{
	ERR_STA err = err_UNKNOWN;
	cout << "load type 1" << endl;
	if (filename)
	{
		vector<unsigned char> loadbin;

		if ((err = loadFile(filename, loadbin)) == err_ok)
		{
			char *txtbuff = (char *)&loadbin.at(0);
			int txtlen = loadbin.size();

			outTable.reserve(txtlen / 20);
			{
				//TimeInterval p("test table:");
				int ret = txt2table(txtbuff, txtlen, outTable);
			
				if (ret == 0)
				{
					return err_ok;
				}
				else
				{
					SN1V2_ERROR_MWSSAGE_WITH("get table", err);
					return err;
				}
			}
		}
		else
		{
			SN1V2_ERROR_MWSSAGE_WITH("load err", err);
			return err;
		}
	}
	else
	{
		SN1V2_ERROR_CODE_RET(err_inval_path);
	}
	return err;
}
#else
#include <regex>
ERR_STA load_table(char * filename, std::vector<timTableSet> & outTable)
{
	ERR_STA err = err_UNKNOWN;
	cout << "load type 2" << endl;
	if (filename)
	{
		string loadbin;

		if ((err = loadFile(filename, loadbin)) == err_ok)
		{
			//ƥ�� 2018-5-14 15:14:13,-1.1,-2.2,-3.3,-4.4,-5.5
			//ƥ�������գ�����ʹ�� Ҳ��������ȷ�����ж�
			regex reg("\\d{4}-\\d{1,2}-\\d{1,2} (\\d{1,2}):(\\d{1,2}):(\\d{1,2})((?:,-?\\d*(?:.\\d+)?){5})");
			smatch match;

			outTable.reserve(loadbin.size() / 60);
			{
				string::const_iterator star = loadbin.begin();
				string::const_iterator end = loadbin.end();
				while (regex_search(star, end, match, reg))
				{
#if 0
					int num = match.size();
					cout << "size = " << num << endl;
					for (int i = 0; i < num;i++)
					{
						cout << match[i] << endl;
					}
					cout << "----------------------------" << endl;
/* ����ֵ
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
					if((cnt = sscanf(pos, "%d:%d:%d,%f,%f,%f,%f,%f"
						, &rtm.tm_hour, &rtm.tm_min, &rtm.tm_sec
						, &rtm.ZxAng, &rtm.YxAng
						, &rtm.RIx, &rtm.RIy, &rtm.RIz
						)) == 8)
					{
						if (checkTime_type2(rtm))
						{
							outTable.emplace_back(rtm);
						}
						else
						{
							SN1V2_ERR_LOG("time check error,%d:%d : %d, %f, %f, %f, %f, %f"
								, rtm.tm_hour,rtm.tm_min, rtm.tm_sec
								, rtm.ZxAng, rtm.YxAng
								, rtm.RIx, rtm.RIy, rtm.RIz);
						}
					}
					else
					{
						SN1V2_ERR_LOG("scanf cnt = %d", cnt);
					}
					star = match[0].second;
				}
			}
		}
		else
		{
			SN1V2_ERROR_MWSSAGE_WITH("load err", err);
			return err;
		}
	}
	else
	{
		SN1V2_ERROR_CODE_RET(err_inval_path);
	}
	return err;
}
#endif


static void rm_past_tm(vector<unsigned int> & table)
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
			cout << "break" << endl;
			break;
		}
		time_t tmpt = table.back();
		if (tmpt < tms)
		{
			//cout << "size=" << table.size() << endl;
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
		time_t tt = outTable.at(outTable.size() - 1 - i);
		tm ref;
		localtime_r(&tt, &ref);

		fprintf(stdout, "year = %d,mon = % d,day = %d""hour = %d,min = % d,sec = %d\n"
			, ref.tm_year + 1900, ref.tm_mon + 1, ref.tm_mday
			, ref.tm_hour, ref.tm_min, ref.tm_sec);
	}
#endif
	LOG(INFO) << "rm size = " << size0 - size1 << endl;
}




static ERR_STA load_table_fixed(vector<unsigned int> & outTable)
{
	tm  thisTm;
	ERR_STA err = GetTim(thisTm);

	thisTm.tm_hour = 0;
	thisTm.tm_min = 0;
	thisTm.tm_sec = 0;

	time_t tms = mktime(&thisTm);

	if (err == err_ok || tms > 0)
	{
		for (auto &p : outTable)
		{
			p += tms;
		}

		std::sort(outTable.begin(), outTable.end(), [](unsigned int a, unsigned b) {return a > b; });
#if 0
		for (size_t i = 0; i < 5; i++)
		{
			time_t tt = outTable.at(i);
			tm ref;
			localtime_r(&tt, &ref);

			fprintf(stdout, "year = %d,mon = % d,day = %d""hour = %d,min = % d,sec = %d\n"
				, ref.tm_year + 1900, ref.tm_mon + 1, ref.tm_mday
				, ref.tm_hour, ref.tm_min, ref.tm_sec);
		}
#endif

		return err_ok;
	}
	else
	{
		SN1V2_ERROR_CODE_RET(err_tim_data_error);
	}
}




int timTableWork(vector<unsigned int> & table, tableWork & test)
{
	load_table_fixed(table);

	rm_past_tm(table);

	int sz = table.size();
	LOG(INFO) << "table size =" << table.size() << endl;

	while (!table.empty())
	{
		time_t captime = table.back();

		ERR_STA err = TimDelayUntil(captime);

		if (err == err_ok)
		{
			test.work(captime, 0);
		}
		else
		{
			test.work(captime, 1);
		}
		table.pop_back();
	}
	return sz;
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
	tm  thisTm;
	ERR_STA err = GetTim(thisTm);

	thisTm.tm_hour = 0;
	thisTm.tm_min = 0;
	thisTm.tm_sec = 0;

	time_t tms = mktime(&thisTm);

	if (err == err_ok || tms > 0)
	{
		for (auto &p : outTable)
		{
			p.tt += tms;
		}

		std::sort(outTable.begin(), outTable.end(), [](const timTableSet & a, const timTableSet &  b) {return a.tt > b.tt; });
#if 0
		for (size_t i = 0; i < 5; i++)
		{
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
	else
	{
		SN1V2_ERROR_CODE_RET(err_tim_data_error);
	}
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

		ERR_STA err = TimDelayUntil(captime);

		if (err == err_ok) {
			test.work(cap, 0);
		} else {
			err = TimDelayUntil(captime + TimeS);

			if (err == err_ok) {
				SN1V2_WARN_LOG("cap over head tick");
				test.work(cap, 0);
			} else {
				SN1V2_WARN_LOG("cap over tail tick");
				test.work(cap, 1);
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
















