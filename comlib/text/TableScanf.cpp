#include "SN1V2_com.h"
#include <regex.h>
#include <iostream>
#include <string.h>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include "errHandle/errHandle.h"
#include <algorithm>
#include <mutex>
#include <regex>
#include "sn1v3cfg.h"

using namespace std;




static ERR_STA ScanfFile(const char * fileName, map<string, string>& dataGroup)
{
	string stText;
	ERR_STA sta = loadFile((char *)fileName, stText);

	if (sta != err_ok) {
		SN1V2_ERROR_MWSSAGE_WITH("load error", sta);
		return sta;
	}

	//%ABC,dsofsii
	//%CDF,-2948.3994
	regex reg("%(\\w+),([\\w\\.:+-/]*)");
	smatch match;

	string::const_iterator star = stText.begin();
	string::const_iterator end = stText.end();
	while (regex_search(star, end, match, reg)) {
#if 0
		fprintf(stderr, "Name = %s,config = %s\n", match[1].str().c_str(), match[2].str().c_str());
#endif

#if 1
		//数据不允许覆盖并发出警告
		auto ret = dataGroup.emplace(match[1], match[2]);

		if (false == ret.second) {
			LOG(WARNING) << "error insert ,key = " << match[1] << ",val= " << match[2] << ",org val =" << dataGroup[match[1]];
		}
#else
		//数据覆盖方案
		dataGroup[match[1]] = match[2];
#endif
		star = match[0].second;
	}

	if (dataGroup.size() == 0) {
		SN1V2_ERROR_CODE_RET(err_conf_get_null);
	}
	return err_ok;
}

static void writeData(void * addr, string & data, dateType typ, void(*default_value)(void *))
{
	switch (typ) {
	case STRING16:
		strncpy((char *)addr, data.c_str(), 16);
		break;

	case STRING32:
		strncpy((char *)addr, data.c_str(), 32);
		break;

	case STRING64:
		strncpy((char *)addr, data.c_str(), 64);
		break;

	case FLOAT32:	
		float tmpfloat;
		if (sscanf(data.c_str(), "%f", &tmpfloat) == 1) {
			*(float *)addr = tmpfloat;
		}
		else {
			if(default_value) default_value(addr);
		}	
		break;

	case DOUBLE64:
		double tmpdouble;
		if (sscanf(data.c_str(), "%lf", &tmpdouble) == 1) {
			*(double *)addr = tmpdouble;
		} else {
			if (default_value) default_value(addr);
		}
		break;
	default:
		break;
	}

}


void scanfAllTable(T1_table & tb1)
{
	map<string, string> datamap;
	ScanfFile("table1", datamap);
	//
	//add more table
	//
	void * outPutCfg[] = {
		&tb1,
	};

	for (size_t i = 0; i < max_group_cnt(); i++) {
		CFG_GROUP *group = find_group(i);


		CFG_INFO * info_group = group->group;
		size_t sz = group->sz;
		void * tableaddr = outPutCfg[i];

		for (int datapos = 0; datapos < sz; datapos++) {
			//cfg address
			CFG_INFO * info = &info_group[datapos];
			void * dataAddr = tableaddr + info->diff;

			if (datamap.count(info->name) > 0) {
				writeData(dataAddr, datamap[info->name], info->typ, info->default_value);
			} else {
				if (info->default_value) {
					info->default_value(dataAddr);
				}
			}
		}
	}
}

void printData(void * baseaddr, CFG_INFO * info)
{
	void * dataAddr = baseaddr + info->diff;

	printf("cfgname = %s,", info->name);

	switch (info->typ) {
	case STRING16:
	case STRING32:
	case STRING64:
		printf("%s", (char *)dataAddr);
		break;
	case FLOAT32:
		printf("%f", *(float *)dataAddr);
		break;
	case DOUBLE64:
		printf("%lf", *(double *)dataAddr);
		break;
	default:
		printf("no type");
		break;
	}
	printf("\n");
}

void printTable(void * table , int groupID)
{	
	if (groupID < max_group_cnt())
	{
		CFG_GROUP *group = find_group(groupID);

		CFG_INFO * info = group->group;
		size_t sz = group->sz;

		for (int datapos = 0; datapos < sz; datapos++) {
			printData(table, &info[datapos]);
		}
	}
	else {
		printf("groupID %d over max\n", groupID);
	}		
}



void testpro()
{
	T1_table tb1;
	scanfAllTable(tb1);
	printTable(&tb1, 0);
}


