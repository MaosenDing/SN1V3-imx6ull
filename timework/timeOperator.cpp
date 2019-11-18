
#include "SN1V2_com.h"

#include <iostream>
#include <string>
#include <regex>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "errHandle/errHandle.h"

using namespace std;

ERR_STA ResTime(
	const std::string & sString,
	const std::string & tType,
	tm & outTime)
{

	cout << "string = " << sString << endl;


	if (tType == "yyyy-MM-dd hh:mm:ss")
	{
		return regTmType_1(sString, outTime);
	}
	else
	{
		SN1V2_ERROR_CODE_RET(err_tim_analysis_not_support);
	}
	SN1V2_ERROR_CODE_RET(err_UNKNOWN);
}


ERR_STA GetTim(tm & reftm)
{
	time_t now = time(0);
	localtime_r(&now, &reftm);
	//reftm.tm_year += 1900;
	//reftm.tm_mon += 1;
	return err_ok;
}


ERR_STA GetTim(std::string & outString)
{
	tm  thisTm;
	GetTim(thisTm);

	char buff[128];
	int len = sprintf(buff, "%4d-%02d-%02d %02d:%02d:%02d",
		thisTm.tm_year + 1900, 
		thisTm.tm_mon  + 1, 
		thisTm.tm_mday,
		thisTm.tm_hour, 
		thisTm.tm_min, 
		thisTm.tm_sec);

	outString.clear();
	outString.append(buff, len);

	cout << outString << endl;
	
	return err_ok;
}

#include "timSet.h"


ERR_STA mktime(const tm& inTm, time_t & outTim)
{
	tm t2(inTm);

	time_t tms = mktime(&t2);

	if (tms < 0)
	{
		SN1V2_ERROR_CODE_RET(err_tim_tm_transfer_error);
	}
	outTim = tms;
	return err_ok;
}


ERR_STA SetTim(tm & reftm)
{
	time_t tms;
	ERR_STA retsta = mktime(reftm, tms);
	if (retsta == err_ok)
	{
		int ret = timeSet((long)tms);
		if (ret)
		{
			SN1V2_ERR_LOG("tim set error code = %d", ret);
			SN1V2_ERROR_CODE_RET(err_tim_set_error);
		}
	}
	return retsta;
}


ERR_STA SetTim(std::string & instring)
{
	tm thisTm;
	memset(&thisTm, 0, sizeof(tm));
	ERR_STA sta = ResTime(instring, string("yyyy-MM-dd hh:mm:ss"), thisTm);
	if (sta == err_ok)
	{
		return SetTim(thisTm);
	}
	else
	{
		SN1V2_ERROR_WITH(sta);
		return sta;
	}
	return err_UNKNOWN;
}

ERR_STA TimDelayUntil(time_t tUntil)
{
	time_t tNow = time(0);

	if (tUntil > tNow)
	{
		while (1)
		{
			usleep(100 * 1000);//sleep 100 ms
			tNow = time(0);
			if (tNow > tUntil)
			{
				SN1V2_WARN_CODE_RET(err_dly_long);
			}
			if (tNow == tUntil)
			{
				return err_ok;
			}
		}
	}
	if (tUntil == tNow)
	{
		LOG(WARNING) << "delay time == now time";
		return err_ok;
	}
	else
	{
		SN1V2_WARN_LOG("time missed,aim is %ld,now is %ld", tUntil, tNow);
		SN1V2_WARN_CODE_RET(err_dly_short);
	}
}




ERR_STA TimDelayUntil(string &sTim)
{
	tm thisTm;
	memset(&thisTm, 0, sizeof(tm));
	ERR_STA sta = ResTime(sTim, string("yyyy-MM-dd hh:mm:ss"), thisTm);

	if (sta == err_ok)
	{
		time_t tms;
		sta = mktime(thisTm, tms);
		if (sta == err_ok)
		{
			return TimDelayUntil(tms);
		}
	}
	SN1V2_ERROR_WITH(sta);
	return sta;
}

ERR_STA TimDelay(time_t inSec)
{
	if (inSec <= 0)
	{
		SN1V2_ERROR_CODE_RET(err_dly_short);
	}
	
	return TimDelayUntil(time(0) + inSec);
}