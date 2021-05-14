#include <errHandle/errHandle.h>
#include <string>
#include <SN1V2_com.h>
#include <fstream>
#include <regex>
#include <algorithm>
#include <time_interval.h>
#include <vector>
#include <math.h>
using namespace std;


static shared_ptr< vector< RESOBJ>> getRes(const char * resName)
{
	auto loadbin = make_shared<string>();
	int fil = loadFile((char  *)resName, *loadbin);

	auto ret = make_shared< vector< RESOBJ>>();

	if (fil == err_ok) {
		TimeInterval p("time:");

		//获取数据
		//匹配 一天的秒,8个浮点数
		//匹配年月日，但不使用 也不对其正确性做判断

		regex reg("\\d{6}((?:,-?\\d*(?:\\.\\d+)?){8})");
		smatch match;
		string::const_iterator star = loadbin->begin();
		string::const_iterator end = loadbin->end();
		while (regex_search(star, end, match, reg)) {
			star = match[0].second;

			RESOBJ tmpobj;
			int snum;
			int timnum;
			if (9 == (snum = sscanf((char *)&(*match[0].first)
				, "%d,%lf,%lf,%d,%lf,%lf,%lf,%lf,%lf"
				, &timnum
				, &tmpobj.pos[0], &tmpobj.pos[1]
				, &tmpobj.sz
				, &tmpobj.data[0]
				, &tmpobj.data[1]
				, &tmpobj.data[2]
				, &tmpobj.data[3]
				, &tmpobj.data[4]
			))) {
				tmpobj.hour = timnum / 10000;
				tmpobj.min = timnum / 100 % 100;
				tmpobj.sec = timnum % 100;
				tmpobj.useflag = 1;
				ret->push_back(tmpobj);
			}
		}
		__attribute__((unused)) int LoadSize = loadbin->size();
		loadbin.reset();
	}
	return ret;
}






bool timeChk(shared_ptr < vector<RESOBJ>>retlist, const SN1_CFG * const pcfg)
{
	//time filter

	int bfnoon = 0, afnoon = 0;
	for (auto & p : *retlist) {
		if (p.hour < 12)
			bfnoon++;
		else
			afnoon++;
	}

	if (pcfg) {
		SN1V2_ERR_LOG("aim thres before,%f,after,%f\n", pcfg->bfnoon_thres, pcfg->afnoon_thres);
		SN1V2_ERR_LOG("cap size before,%d,after,%d\n", bfnoon, afnoon);
		//SN1V2_ERR_LOG("aim size before,%d,after,%d\n", 2069, 1996);
		SN1V2_ERR_LOG("aim size before,%d,after,%d\n", pcfg->bf_size, pcfg->af_size);

		if ((bfnoon == 0) || (afnoon == 0)) {
			SN1V2_ERR_LOG("image null\n");
			return false;
		}

		double CapMinSize[2] = {
			pcfg->bfnoon_thres * pcfg->bf_size,
			pcfg->afnoon_thres * pcfg->af_size,
		};

		SN1V2_ERR_LOG("cap min size before,%lf,after,%lf\n", CapMinSize[0], CapMinSize[1]);

		if ((bfnoon > CapMinSize[0]) &&
			(afnoon > CapMinSize[1])) {
			SN1V2_ERR_LOG("day check ok\n");
			return true;
		}
	}
	SN1V2_ERR_LOG("day check failed\n");
	return false;
}


static bool cmpRES(RESOBJ & a, RESOBJ & b)
{
	return a.angle < b.angle;
}

void REStoTxt(const char * resName, vector<RESOBJ>  & reslist, res_filter_mode mod)
{
	ofstream outtest(resName);

	if (outtest.good()) {
		outtest.clear();
		char tmpbuff[128];
		for (auto & p : reslist) {
			switch (mod) {
			case res_filter_normal:
				break;
			case res_filter_exchange:
				swap(p.pos[0], p.pos[1]);
				break;
			case res_filter_exchange_then_reserve_x:
				swap(p.pos[0], p.pos[1]);
				p.pos[1] = -p.pos[1];
				break;
			case res_filter_exchange_then_reserve_y:
				swap(p.pos[0], p.pos[1]);
				p.pos[0] = -p.pos[0];
				break;
			case res_filter_exchange_then_reserve_x_y:
				swap(p.pos[0], p.pos[1]);
				p.pos[0] = -p.pos[0];
				p.pos[1] = -p.pos[1];
				break;
			case res_filter_noexchange_reserve_x:
				p.pos[1] = -p.pos[1];
				break;
			case res_filter_noexchange_reserve_y:
				p.pos[0] = -p.pos[0];
				break;
			case res_filter_noexchange_reserve_x_y:
				p.pos[0] = -p.pos[0];
				p.pos[1] = -p.pos[1];
				break;
			default:
				break;
			}
			int sz = sprintf(tmpbuff, "%02d%02d%02d,%lf,%lf,%d,%.3lf,%.3lf,%.6lf,%.6lf,%.6lf\n"
				, p.hour, p.min, p.sec
				, p.pos[0], p.pos[1]
				, p.sz
				, p.data[0]
				, p.data[1]
				, p.data[2]
				, p.data[3]
				, p.data[4]
			);
			outtest.write(tmpbuff, sz);
		}
	}
}


void chkAng(shared_ptr < vector<RESOBJ>>retlist, const SN1_CFG * const pcfg, double(&extrData)[6])
{
	if (pcfg && retlist->size()) {
		double & MaxVal = extrData[3];
		double & MinVal = extrData[4];
		double & meanVal = extrData[5];

		for (auto & p : extrData) p = 0;

		SN1V2_INF_LOG("SPS = %lf,SFL = %lf\n", pcfg->SPS, pcfg->SFL);

		int cnt[3] = { 0,0,0 };

		for (auto &point : *retlist) {
			double s1 = sqrt(
				(point.pos[0] * point.pos[0]) + (point.pos[1] * point.pos[1]));
			s1 *= pcfg->SPS;

			point.angle = atan2(s1, pcfg->SFL) * 1000;
			meanVal += point.angle;

			if (point.angle < 1.000001f)
				cnt[0] ++;
			if (point.angle < 2.000001f)
				cnt[1] ++;
			if (point.angle < 3.000001f)
				cnt[2] ++;
		}
		auto maximal = std::max_element(retlist->begin(), retlist->end(), cmpRES);
		auto minimal = std::min_element(retlist->begin(), retlist->end(), cmpRES);

		meanVal /= retlist->size();
		extrData[0] = (double)cnt[0] / (double)retlist->size() * 100.0f;
		extrData[1] = (double)cnt[1] / (double)retlist->size() * 100.0f;
		extrData[2] = (double)cnt[2] / (double)retlist->size() * 100.0f;
		MaxVal = maximal->angle;
		MinVal = minimal->angle;

		SN1V2_INF_LOG("maximal is %d,time = %2d,%2d,%2d,pos = %lf,%lf,ang = %lf\n",
			distance(retlist->begin(), maximal),
			maximal->hour, maximal->min, maximal->sec,
			maximal->pos[0], maximal->pos[1], maximal->angle);

		SN1V2_INF_LOG("minimal is %d,time = %2d,%2d,%2d,pos = %lf,%lf,ang = %lf\n",
			distance(retlist->begin(), minimal),
			minimal->hour, minimal->min, minimal->sec,
			minimal->pos[0], minimal->pos[1], minimal->angle);

		SN1V2_INF_LOG("size = %d,mean val = %lf\n", retlist->size(), meanVal);

		SN1V2_INF_LOG("1mrad = %d , 2mrad = %d ,3mrad = %d\n", cnt[0], cnt[1], cnt[2]);
		SN1V2_INF_LOG("1mrad = %lf , 2mrad = %lf ,3mrad = %lf\n", extrData[0], extrData[1], extrData[2]);

	} else {
		SN1V2_INF_LOG("no cfg\n");
		return;
	}
}

__attribute__((unused)) static void prilist(shared_ptr< vector< RESOBJ>>retlist)
{
	printf("list size = %d\n", retlist->size());
	for (auto & p : *retlist) {
		printf("index = %d,", std::distance(&*retlist->begin(), &p));

		printf("%d-%d-%d,%lf,%lf,%d"
			, p.hour, p.min, p.sec, p.pos[0], p.pos[1], p.sz
		);
		for (auto &f : p.data) {
			printf(",%lf", f);
		}
		printf(",angle = %.12lf", p.angle);
		printf("\n");
	}
}

#define DBG_DISABLE_TIMCHK 0




ERR_STA res_filter(const char * resName, const SN1_CFG * const pcfg, double(&exta)[6], res_filter_mode mod)
{
	auto retlist = getRes(resName);
	//get res list
	int retsize = retlist->size();
	SN1V2_INF_LOG("list size = %d\n", retsize);

	if (retsize) {
		if (pcfg) {
			bool timchkFLG = timeChk(retlist, pcfg);
#if DBG_DISABLE_TIMCHK == 0
			if (false == timchkFLG) {
#if 0 
				remove(resName);
				printf("rm %s\n", resName);
#endif
				SN1V2_ERROR_CODE_RET(err_data_filter_day_break);
			}
#else
			SN1V2_ERR_LOG("skip timchk =%d\n", timchkFLG);
#endif	
		}
		chkAng(retlist, pcfg, exta);
		REStoTxt(resName, *retlist, mod);
	} else {
		SN1V2_INF_LOG("rm %s\n", resName);
		remove(resName);
		SN1V2_WARN_CODE_RET(err_data_filter_NULL_RES);
	}
#if 0
	prilist(retlist);
#endif
	return err_ok;
}






