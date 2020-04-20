#include "timeTableV2.h"
#include "errHandle.h"
#include "tableWork.h"
#include <iostream>
#include <regex>

using namespace std;

static inline bool checkTime_type2(tm & reftm)
{
	return 1
		&& is_valid_daytim(reftm.tm_hour, reftm.tm_min, reftm.tm_sec)
		//&& is_valid_date(reftm.tm_year, reftm.tm_mon, reftm.tm_mday)
		;
}


ERR_STA load_table(char * filename, std::list<timTableSetV2> & outTable)
{
	ERR_STA err = err_UNKNOWN;
	cout << "load type 2" << endl;
	if (filename) {
		string loadbin;

		if ((err = loadFile(filename, loadbin)) == err_ok) {
			//匹配 15:14:13,-1.1,-2.2,3,4,5,6
			//匹配年月日，但不使用 也不对其正确性做判断
			regex reg("\\d{1,2}:\\d{1,2}:\\d{1,2}(?:,-?\\d*(?:\\.\\d+)?){6}");
			smatch match;


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
				timTableSetV2 rtm;

				tm  thisTm;
				GetTim(thisTm);

				char *pos = (char *)&(*match[0].first);
				int cnt;
				if ((cnt = sscanf(pos, "%d:%d:%d,%f,%f,%d,%d,%d,%d"
					, &thisTm.tm_hour, &thisTm.tm_min, &thisTm.tm_sec
					, &rtm.ZxAng, &rtm.YxAng
					, &rtm.mdc_work_length, &rtm.mdc_mod, &rtm.weigth, &rtm.cap_reserve
				)) == 9) {
					if (checkTime_type2(thisTm)) {
						rtm.tt = mktime(&thisTm);
						outTable.emplace_back(rtm);
					} else {
						SN1V2_ERR_LOG("time check error,%d:%d:%d,%f,%f,%d,%d,%d,%d"
							, thisTm.tm_hour, thisTm.tm_min, thisTm.tm_sec
							, rtm.ZxAng, rtm.YxAng
							, rtm.mdc_work_length, rtm.mdc_mod, rtm.weigth, rtm.cap_reserve
						);
					}
				} else {
					SN1V2_ERR_LOG("scanf cnt = %d", cnt);
				}
				star = match[0].second;

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

void FixTimeTableV2(list<timTableSetV2> & outTable)
{
	outTable.sort([](timTableSetV2 & a, timTableSetV2 & b) {return a.tt < b.tt; });





}


void printTable(list<timTableSetV2> & reflist)
{
	for (auto &p : reflist) {
		time_t tt = p.tt;
		tm ref;
		localtime_r(&tt, &ref);

		printf("start tim %d:%d:%d  deg=%f,%f  worklen %d workmod %d weight %d cap %d\n ",
			ref.tm_hour,ref.tm_min,ref.tm_sec,
			p.ZxAng,p.YxAng,
			p.mdc_work_length,p.mdc_mod,p.weigth,p.cap_reserve
		);
	}
}

int testTimeTableV2(int argc, char * argv[])
{
	list<timTableSetV2>  b1;
	load_table("test/b1.txt", b1);
	printf("scanf p1\n");
	printTable(b1);

	list<timTableSetV2>  b2;
	load_table("test/b2.txt", b2);
	printf("scanf p2\n");
	printTable(b2);

	list<timTableSetV2>  b3;
	load_table("test/b3.txt", b3);
	printf("scanf p3\n");
	printTable(b3);

	list<timTableSetV2>  all;
	all.merge(b1, [] (timTableSetV2 a, timTableSetV2 b){return true; });
	all.merge(b2, [] (timTableSetV2 a, timTableSetV2 b){return true; });
	all.merge(b3, [] (timTableSetV2 a, timTableSetV2 b){return true; });
	printf("all\n");
	printTable(all);



	FixTimeTableV2(all);
	printf("all fix 1\n");
	printTable(all);



	return 0;
}
