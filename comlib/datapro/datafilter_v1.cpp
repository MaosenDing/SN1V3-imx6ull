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

static void sr_filter(const char * srName, shared_ptr< vector< RESOBJ>> retlist)
{
	auto loadbin = make_shared<string>();
	int fil = loadFile((char  *)srName, *loadbin);

	if (fil == err_ok) {
		TimeInterval p("sr got:");

		regex reg("\\d{2}:\\d{2}:\\d{2}((?:,-?\\d*(?:\\.\\d+)?){4})");
		smatch match;
		string::const_iterator star = loadbin->begin();
		string::const_iterator end = loadbin->end();
		while (regex_search(star, end, match, reg)) {
			star = match[0].second;

			RESOBJ tmpobj;
			int snum;
			double cirNum;
			double tmpsz;
			if (7 == (snum = sscanf((char *)&(*match[0].first)
				, "%d:%d:%d,%lf,%lf,%lf,%lf"
				, &tmpobj.hour, &tmpobj.min, &tmpobj.sec
				, &tmpobj.pos[0], &tmpobj.pos[1]
				, &tmpsz
				, &cirNum
			))) {
				if (cirNum > 0.8) {
					auto pos = std::find_if(retlist->begin(), retlist->end()
						, [&tmpobj](RESOBJ& testppp) {
						return ((tmpobj.hour == testppp.hour)
							&& (tmpobj.min == testppp.min))
							&& (tmpobj.sec == testppp.sec + 1);
					});
					if (pos != retlist->end()) {
						pos->pos[0] = tmpobj.pos[0];
						pos->pos[1] = tmpobj.pos[1];
						pos->sz = tmpobj.sz;
						pos->sz = (int)tmpsz;
						pos->useflag++;
					}
				}
			}
		}
		loadbin.reset();
	}
}

static shared_ptr< vector< RESOBJ>>  sh_got(const char * shName)
{
	auto loadbin = make_shared<string>();
	int fil = loadFile((char  *)shName, *loadbin);

	auto ret = make_shared< vector< RESOBJ>>();

	if (fil == err_ok) {
		TimeInterval p("sh got:");
		//筛选sh文件
		/*
2018-11-22 07:31:10,52.815,79.978,0.595199,0.784498,0.174072
2018-11-22 07:31:20,52.785,79.953,0.595567,0.784123,0.174504
		*/
		regex reg("\\d{2}:\\d{2}:\\d{2}((?:,-?\\d*(?:\\.\\d+)?){5})");
		smatch match;
		string::const_iterator star = loadbin->begin();
		string::const_iterator end = loadbin->end();
		while (regex_search(star, end, match, reg)) {
			star = match[0].second;

			RESOBJ tmpobj;
			int snum;
			if (8 == (snum = sscanf((char *)&(*match[0].first)
				, "%d:%d:%d,%lf,%lf,%lf,%lf,%lf"
				, &tmpobj.hour, &tmpobj.min, &tmpobj.sec
				, &tmpobj.data[0]
				, &tmpobj.data[1]
				, &tmpobj.data[2]
				, &tmpobj.data[3]
				, &tmpobj.data[4]
			))) {
				tmpobj.useflag = 1;
				ret->push_back(tmpobj);
			}
		}
		loadbin.reset();
	}
	return ret;
}



static shared_ptr< vector< RESOBJ>> getResTest(const char * shName, const char *srName)
{
	auto firstlist = sh_got(shName);
	sr_filter(srName, firstlist);
	printf("first list size = %d\n", firstlist->size());
	shared_ptr< vector< RESOBJ>> ret = make_shared<vector<RESOBJ>>();

	for (auto &p : *firstlist) {
		if (p.useflag == 2) {
			ret->push_back(p);
		}
	}
	return ret;
}


void chkAng(shared_ptr < vector<RESOBJ>>retlist, const SN1_CFG * const pcfg, double(&extrData)[6]);
void REStoTxt(const char * resName, vector<RESOBJ>  & reslist, res_filter_mode mod);

void v1_res_filter(const char * shName, const char *srName, const char * resName, const char * creName, const SN1_CFG * const pcfg)
{
	auto retlist = getResTest(shName, srName);

	//获取cre
	auto creDataGrp = initCre(creName);
	CREOBJ  creData = GetNewCre(*creDataGrp);

	if (retlist->size() == 0) {
		printf("no res data\n");
		return;
	}

	//计算偏差角度
	chkAng(retlist, pcfg, creData.extra);
	//写回res文件
	REStoTxt(resName, *retlist , res_filter_normal);

	//保存数据
	if (err_ok == CalCre(resName, creData, pcfg->SPS, pcfg->SFL)) {
		//添加成功的cre数据
		creDataGrp->push_back(creData);
		//导出文本
		save_cre(creName, *creDataGrp, pcfg->SID);
	}
}






