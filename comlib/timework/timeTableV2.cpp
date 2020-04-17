#include "timeTableV2.h"
#include "errHandle.h"
#include "tableWork.h"
#include <iostream>
#include <regex>

using namespace std;

static inline bool checkTime_type2(timTableSet & reftm)
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
			//匹配 2018-5-14 15:14:13,-1.1,-2.2,-3.3,-4.4,-5.5
			//匹配年月日，但不使用 也不对其正确性做判断
			regex reg("\\d{4}-\\d{1,2}-\\d{1,2} (\\d{1,2}):(\\d{1,2}):(\\d{1,2})((?:,-?\\d*(?:\\.\\d+)?){5})");
			smatch match;

			//outTable.reserve(loadbin.size() / 60);
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
					//if ((cnt = sscanf(pos, "%d:%d:%d,%f,%f,%f,%f,%f"
					//	, &rtm.tm_hour, &rtm.tm_min, &rtm.tm_sec
					//	, &rtm.ZxAng, &rtm.YxAng
					//	, &rtm.RIx, &rtm.RIy, &rtm.RIz
					//)) == 8) {
					//	if (checkTime_type2(rtm)) {
					//		outTable.emplace_back(rtm);
					//	} else {
					//		SN1V2_ERR_LOG("time check error,%d:%d : %d, %f, %f, %f, %f, %f"
					//			, rtm.tm_hour, rtm.tm_min, rtm.tm_sec
					//			, rtm.ZxAng, rtm.YxAng
					//			, rtm.RIx, rtm.RIy, rtm.RIz);
					//	}
					//} else {
					//	SN1V2_ERR_LOG("scanf cnt = %d", cnt);
					//}
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


int testTimeTableV2(int argc, char * argv[])
{


	return 0;
}
