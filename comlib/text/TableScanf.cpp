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
	regex reg("%(\\w+),([\\w\\.:+\\-/]*)");
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


static int find_if_true(const char * st)
{
	const char  * truegroup[] = {
	"t",
	"T",
	"True",
	"true",
	"TRUE",
	};

	for (const char * tr_st : truegroup) {
		if (!strcmp(st, tr_st)) {
			return 1;
		}
	}
	return 0;
}


static void writeData(void * addr, string & data, dateType typ, void(*default_value)(void *))
{
	switch (typ) {
	case dateType::STRING16:
		strncpy((char *)addr, data.c_str(), 16);
		break;

	case dateType::STRING32:
		strncpy((char *)addr, data.c_str(), 32);
		break;

	case dateType::STRING64:
		strncpy((char *)addr, data.c_str(), 64);
		break;

	case dateType::STRING128:
		strncpy((char *)addr, data.c_str(), 128);
		break;

	case dateType::FLOAT32:
		float tmpfloat;
		if (sscanf(data.c_str(), "%f", &tmpfloat) == 1) {
			*(float *)addr = tmpfloat;
		} else {
			if (default_value) default_value(addr);
		}
		break;

	case dateType::DOUBLE64:
		double tmpdouble;
		if (sscanf(data.c_str(), "%lf", &tmpdouble) == 1) {
			*(double *)addr = tmpdouble;
		} else {
			if (default_value) default_value(addr);
		}
		break;
	case dateType::INT32:
		int32_t tmpint;
		if (sscanf(data.c_str(), "%d", &tmpint) == 1) {
			*(int32_t *)addr = tmpint;
		} else {
			if (default_value) default_value(addr);
		}
	case dateType::TIM16:


		break;

	case dateType::MAC:
		int tmpmac[6];
		if (sscanf(data.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x"
			, &tmpmac[0]
			, &tmpmac[1]
			, &tmpmac[2]
			, &tmpmac[3]
			, &tmpmac[4]
			, &tmpmac[5]
		) == 6) {
			for (int i = 0; i < 6; i++) {
				*((unsigned int *)addr + i) = tmpmac[i];
			}
		} else {
			if (default_value) default_value(addr);
		}
		break;

	case dateType::BOOLTYPE:
		*(int *)addr = find_if_true(data.c_str());
		break;

		//ip内存中使用小端格式
	case dateType::IP:
		int tmpIP[4];
		if (sscanf(data.c_str(), "%d.%d.%d.%d"
			, &tmpIP[0]
			, &tmpIP[1]
			, &tmpIP[2]
			, &tmpIP[3]
		) == 4) {
			char * dat = (char *)addr;
			dat[3] = tmpIP[0];
			dat[2] = tmpIP[1];
			dat[1] = tmpIP[2];
			dat[0] = tmpIP[3];
		} else {
			if (default_value) default_value(addr);
		}
		break;

		break;
	default:
		break;
	}
}


int scanfOneTable(const void * tableaddr, const char * tableName, map<string, string> &datamap)
{
	const CFG_GROUP *group = find_group_name(tableName);

	if ((!group) || (!tableaddr)) {
		return -1;
	}

	const CFG_INFO * info_group = group->group;
	size_t sz = group->sz;

	for (size_t datapos = 0; datapos < sz; datapos++) {
		//cfg address
		const CFG_INFO * info = &info_group[datapos];
		void * dataAddr = (char *)tableaddr + info->diff;

		if (datamap.count(info->name) > 0) {
			writeData(dataAddr, datamap[info->name], info->typ, info->default_value);
		} else {
			if (info->default_value) {
				info->default_value(dataAddr);
			}
		}
	}
	return 0;
}

void setfromDefault(const void * tableaddr, const char * tableName)
{
	const CFG_GROUP *group = find_group_name(tableName);

	if ((!group) || (!tableaddr)) {
		return;
	}

	const CFG_INFO * info_group = group->group;
	size_t sz = group->sz;

	for (size_t datapos = 0; datapos < sz; datapos++) {
		//cfg address
		const CFG_INFO * info = &info_group[datapos];
		void * dataAddr = (char *)tableaddr + info->diff;

		if (info->default_value) {
			info->default_value(dataAddr);
		}
	}
}


void scanfAllTable(Tg_table & tb,uint32_t table_mask)
{
	size_t maxcnt = max_group_cnt();
	map<string, string> datamap;
	for (size_t i = 0; i < maxcnt; i++) {
		const CFG_GROUP * grp = find_group_index(i);		
		if (grp->cfgMask & table_mask) {
			ScanfFile(grp->cfgName, datamap);
			scanfOneTable((char *)&tb + grp->diff, grp->groupName, datamap);
			datamap.clear();
		}
	}
}



void printData2String(string & outstring, const void * baseaddr, const CFG_INFO * info)
{
	void * dataAddr = (char *)baseaddr + info->diff;

	char tmpbuff[64] = { 0 };


	switch (info->typ) {
	case dateType::STRING16:
	case dateType::STRING32:
	case dateType::STRING64:
		snprintf(tmpbuff, 64, "%%%s,%s\n", info->name, (char *)dataAddr);
		outstring.append(tmpbuff);
		break;
	case dateType::FLOAT32:
		snprintf(tmpbuff, 64, "%%%s,%f\n", info->name, *(float *)dataAddr);
		outstring.append(tmpbuff);
		break;
	case dateType::DOUBLE64:
		snprintf(tmpbuff, 64, "%%%s,%lf\n", info->name, *(double *)dataAddr);
		outstring.append(tmpbuff);
		break;
	case dateType::INT32:
		snprintf(tmpbuff, 64, "%%%s,%d\n", info->name, *(int *)dataAddr);
		outstring.append(tmpbuff);
		break;
	case dateType::MAC:
	{
		char * macDat = (char *)dataAddr;
		snprintf(tmpbuff, 64, "%%%s,%02x:%02x:%02x:%02x:%02x:%02x\n", info->name
			, macDat[0]
			, macDat[1]
			, macDat[2]
			, macDat[3]
			, macDat[4]
			, macDat[5]
		);
		outstring.append(tmpbuff);
	}
	break;

	case dateType::BOOLTYPE:
		if (*(int *)dataAddr == 0) {
			snprintf(tmpbuff, 64, "%%%s,false\n", info->name);
		} else {
			snprintf(tmpbuff, 64, "%%%s,true\n", info->name);
		}
		outstring.append(tmpbuff);
		break;
	case dateType::IP:
	{
		//ip内存中使用小端格式
		char * IPdat = (char *)dataAddr;
		snprintf(tmpbuff, 64, "%%%s,%d.%d.%d.%d\n", info->name
			, IPdat[3]
			, IPdat[2]
			, IPdat[1]
			, IPdat[0]
		);
		outstring.append(tmpbuff);
	}
	break;

	default:
		break;
	}
}

void printTable2String(string & outstring, void * table, const char * tableName)
{
	const CFG_GROUP * group = find_group_name(tableName);

	if (!group) {
		return;
	}

	const CFG_INFO * info = group->group;
	size_t sz = group->sz;

	for (size_t datapos = 0; datapos < sz; datapos++) {
		printData2String(outstring, table, &info[datapos]);
	}
}


void testpro()
{
	Tg_table tg_table;

	scanfAllTable(tg_table, Mask_All);


	size_t maxcnt = max_group_cnt();
	map<string, string> datamap;
	for (size_t i = 0; i < maxcnt; i++) {
		const CFG_GROUP * grp = find_group_index(i);
		string st;
		st.append("print:");
		st.append(grp->groupName);
		st.append("\n");
		printTable2String(st, (char *)&tg_table + grp->diff, grp->groupName);
		cout << st << endl;
	}
}


