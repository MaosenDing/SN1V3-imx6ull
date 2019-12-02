
#include "SN1V2_com.h"

#include <iostream>
#include <string>
#include <time.h>

#include <regex.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include "errHandle/errHandle.h"
using namespace std;


static inline bool is_valid_data(int y, int m, int d)
{
	int month[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)
	{
		month[1] = 29;
	}
	return y > 1900 && y < 2030 && m > 0 && m <= 12 && d > 0 && d <= month[m - 1];
}

static inline bool is_valid_daytim(int hour, int min, int sec)
{
	return hour >= 0 && hour < 24 
		&& min >= 0 && min <60 
		&& sec >= 0 && sec < 60;
}

static inline void fix_struct_tm(tm & rtm)
{
	rtm.tm_year -= 1900;
	rtm.tm_mon -= 1;
}


static inline void getTmType_1(const char * pos, tm & rtm)
{
	sscanf(pos, "%d-%d-%d %d:%d:%d",
		&rtm.tm_year, &rtm.tm_mon, &rtm.tm_mday,
		&rtm.tm_hour, &rtm.tm_min, &rtm.tm_sec);
}


static inline bool checkTime(tm & reftm)
{
	return is_valid_daytim(reftm.tm_hour, reftm.tm_min, reftm.tm_sec) && 
		is_valid_data(reftm.tm_year,reftm.tm_mon,reftm.tm_mday);
}

#if 0
static regex_t * getRegType_1(void)
{
	static bool flag = 0;
	static regex_t reg;

	if (!flag)
	{
		const char * pattern = "([0-9]{1,4}-[0-9]{1,2}-[0-9]{1,2}) ([0-9]{1,2}:[0-9]{1,2}:[0-9]{1,2})";
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

ERR_STA regTmType_1(const string & ppp, tm & rtm)
{
	const size_t nmatch = 1;
	regmatch_t pmatch[nmatch];

	memset(pmatch, 0, sizeof(regmatch_t) * 1);

	regex_t * pRegType_1 = getRegType_1();
	if (pRegType_1 == 0)
	{
		cout << "reg comp error" << endl;
		SN1V2_ERROR_CODE_RET(err_tim_analysis_error);
	}

	int status = regexec(pRegType_1, ppp.c_str(), nmatch, pmatch, 0);

	if (status == REG_NOMATCH)
	{
		cout << "string =" << ppp.c_str() << endl;
		cout << "no match" << endl;
		SN1V2_ERROR_CODE_RET(err_tim_analysis_error);
	}
	else if (status == 0)
	{
		regmatch_t & refm = pmatch[0];

		if (refm.rm_eo != 0)
		{
			string st = ppp.substr(refm.rm_so, refm.rm_eo - refm.rm_so);
			getTmType_1(&st[0], rtm);
			if (true == checkTime(rtm))
			{
				fix_struct_tm(rtm);
				return err_ok;
			}
			else
			{
				SN1V2_ERROR_CODE_RET(err_tim_data_error);
			}
		}

	}
	SN1V2_ERROR_CODE_RET(err_UNKNOWN);
}
#else
#include <regex>
ERR_STA regTmType_1(const string & ppp, tm & rtm)
{
	regex reg("\\d{4}-\\d{1,2}-\\d{1,2} \\d{1,2}:\\d{1,2}:\\d{1,2}");
	smatch smt;
	if (regex_search(ppp, smt, reg))
	{		
		getTmType_1(&*smt[0].first, rtm);

		if (true == checkTime(rtm))
		{
			fix_struct_tm(rtm);
			return err_ok;
		}
		else
		{
			SN1V2_ERROR_CODE_RET(err_tim_data_error);
		}
		SN1V2_ERROR_CODE_RET(err_tim_data_error);
	}
	else
	{
		SN1V2_ERROR_CODE_RET(err_tim_analysis_error);
	}
}
#endif

