#ifndef __timeTableV2_H____
#define __timeTableV2_H____

#include <vector>
#include <list>
#include <string>
#include <memory>

#include "SN1V2_com.h"



struct timTableSetV2 {
	time_t tt;

	float ZxAng;
	float YxAng;

	int mdc_work_length;//mdc在这个状态的工作时间
	int mdc_mod;//mdc工作模式
	int weigth;//这个时间节点的权重 高权重可以覆盖地权重
	int cap_reserve;//需要拍摄时，相机允许的最长MDC运行到位时间
};


int testTimeTableV2(int argc, char * argv[]);
void convertTimTable2V2(std::vector<timTableSet> &timeset, std::list<timTableSetV2> &outlist);
void FixTimeTableV2(std::list<timTableSetV2> & inputList);
#endif
