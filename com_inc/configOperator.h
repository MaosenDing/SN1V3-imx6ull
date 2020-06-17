#ifndef __configOperator_h____
#define __configOperator_h____


#include "errHandle.h"
#include <map>
#include "sn1v3cfg.h"
#include "SN1V2_error.h"
	ERR_STA ScanfFile(const char * fileName, std::map<std::string, std::string>& dataGroup);
	int scanfOneTable(const void * tableaddr, CFG_INFO * info_group, const size_t sz, std::map<std::string, std::string> &datamap);
	void printTable2String(std::string & outstring, void * table, const CFG_INFO * info, size_t sz, int writeMask);
	void scanfAllTable(Tg_table & tb, uint32_t table_mask);
#endif
