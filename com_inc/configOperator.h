#ifndef __configOperator_h____
#define __configOperator_h____


#include "errHandle.h"
#include <map>
#include "sn1v3cfg.h"
	ERR_STA ScanfFile(const char * fileName, std::map<std::string, std::string>& dataGroup);
	int scanfOneTable(const void * tableaddr, const CFG_GROUP * group, std::map<std::string, std::string> &datamap);


#endif
