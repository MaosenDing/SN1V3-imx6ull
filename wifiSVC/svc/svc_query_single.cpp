#include "../wifi_svc.h"
#include "../wifi_ctrl.h"
#include "svc_once_write.h"
#include "svc_once_read.h"
#include "jd_share.h"
#include <string.h>



struct WIFI_QUERY_SINGLE_DATA :public WIFI_FUNCTION_ONCE_WRITE
{

	WIFI_QUERY_SINGLE_DATA(WIFI_INFO & info, int inmsgid, int intable, int inindex) :WIFI_FUNCTION_ONCE_WRITE(info,inmsgid)
	{
		functionID = 2;
		table = intable;
		index = inindex;
	}


	virtual const char * FUNCTION_NAME() final
	{
		return "query single data";
	}

	int table = 0;
	int index = 0;

	virtual int mk_write_data(unsigned char * dat, int maxlen) final
	{
		dat[1] = functionID;
		dat[2] = table;
		dat[3] = index;
		int retlen = 3;
		unsigned char * datpos = &dat[3];
		const CFG_GROUP * grp = find_group_by_cfg_index(table);
		if (grp) {
			CFG_INFO * info_group = grp->group;
			size_t sz = grp->sz;
			const CFG_INFO * aiminfo = find_info_by_seqIndex(info_group, sz, index);
			if (aiminfo) {
				char outdata[128] = { 0 };
				//int len = query_data_by_index((char *)&info.cfg + grp->diff, aiminfo, outdata, 128);
				int len = printData2String(outdata, 128, (char *)&info.cfg + grp->diff, aiminfo);

				printf("query name = %s,len = %d,ret=%s\n", aiminfo->name, len, outdata);
				if (len > 0) {
					memcpy(datpos, outdata, len);
					retlen += len;
				}
			}
		}
		return retlen;
	}
};

struct WIFI_QUERY_SINGLE_DATA_HEAD :public WIFI_FUNCTION_ONCE_READ
{
	WIFI_QUERY_SINGLE_DATA_HEAD(WIFI_INFO & info) :WIFI_FUNCTION_ONCE_READ(info)
	{
		functionID = 2;
	}
	virtual const char * FUNCTION_NAME() final
	{
		return "query single data";
	}

	virtual void read_pro_fun(WIFI_BASE_SESSION & sec)final
	{
		WIFI_DATA_SUB_PROTOCOL sub;
		mk_WIFI_DATA_SUB_PROTOCOL(sec, sub);

		if (sec.frame_index == -1 && sub.datalen == 2) {
			if (info.dbg_pri_msg) {
				printf("table =%d, id = %d\n"
					, sub.function_data[0]
					, sub.function_data[1]
				);
			}
			ADD_FUN(new WIFI_QUERY_SINGLE_DATA(info
				, WIFI_BASE_FUNCTION::static_msg_id++
				, sub.function_data[0]
				, sub.function_data[1]));
		}
	}
};

WIFI_BASE_FUNCTION * GetWIFI_QUERY_SINGLE_DATA(WIFI_INFO & wifi)
{
	return new WIFI_QUERY_SINGLE_DATA_HEAD(wifi);
}



