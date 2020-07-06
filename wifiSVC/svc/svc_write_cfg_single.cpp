#include "../wifi_svc.h"
#include "../wifi_ctrl.h"
#include "svc_once_write.h"
#include "jd_share.h"
#include <string.h>
struct WIFI_WRITE_SINGLE_DATA :public WIFI_FUNCTION_ONCE_WRITE
{
	WIFI_WRITE_SINGLE_DATA(WIFI_INFO & info) :WIFI_FUNCTION_ONCE_WRITE(info)
	{
		PRO_MASK = WIFI_BASE_FUNCTION::MASK_READ;
		functionID = 1;
	}

	WIFI_WRITE_SINGLE_DATA(WIFI_INFO & info, int inmsgid, int intable, int inindex) :WIFI_FUNCTION_ONCE_WRITE(info)
	{
		PRO_MASK = WIFI_BASE_FUNCTION::MASK_SELF_UPLOAD;
		functionID = 1;
		msgid = inmsgid;
		table = intable;
		index = inindex;
	}


	virtual const char * FUNCTION_NAME() final
	{
		return "write single data";
	}

	int table = 0;
	int index = 0;
	int msgid = 0;
	virtual void read_pro_fun(WIFI_BASE_SESSION & sec)final
	{
		WIFI_DATA_SUB_PROTOCOL sub;
		mk_WIFI_DATA_SUB_PROTOCOL(sec, sub);

		if (sec.frame_index == -1 && sub.datalen > 2) {
			if (info.dbg_pri_msg) {
				printf("table =%d, id = %d,val = %s\n"
					, sub.function_data[0]
					, sub.function_data[1]
					, &sub.function_data[2]
				);
			}
			table = sub.function_data[0];
			index = sub.function_data[1];

			const CFG_GROUP * grp = find_group_by_cfg_index(table);
			if (grp) {
				CFG_INFO * info_group = grp->group;
				size_t sz = grp->sz;
				CFG_INFO * aiminfo = find_info_by_seqIndex(info_group, sz, index);
				if (aiminfo) {
					char tmpdat[128];
					memcpy(tmpdat, &sub.function_data[2], sub.datalen - 2);
					tmpdat[sub.datalen - 2] = 0;
					scanfSingleDataCtype((char *)&info.cfg + grp->diff, tmpdat, aiminfo);
					
					printTable2cfgfile(&info.cfg, grp, writeUseful);
				}
			}

			ADD_FUN(new WIFI_WRITE_SINGLE_DATA(info
				, sub.message_id
				, sub.function_data[0]
				, sub.function_data[1]));
		}
	}

	virtual void mk_write_session_data(WIFI_BASE_SESSION & sec) final
	{
		sec.data_len = 5;
		sec.data[0] = msgid;
		sec.data[1] = msgid >> 8;
		sec.data[2] = functionID;
		sec.data[3] = table;
		sec.data[4] = index;

		unsigned char * datpos = &sec.data[sec.data_len];
		const CFG_GROUP * grp = find_group_by_cfg_index(table);
		if (grp) {
			CFG_INFO * info_group = grp->group;
			size_t sz = grp->sz;
			const CFG_INFO * aiminfo = find_info_by_seqIndex(info_group, sz, index);
			if (aiminfo) {
				char outdata[128] = { 0 };
				//int len = query_data_by_index((char *)&info.cfg + grp->diff, aiminfo, outdata, 128);
				int len = printData2String(outdata, 128, (char *)&info.cfg + grp->diff, aiminfo);

				printf("update name = %s,len = %d,ret=%s\n", aiminfo->name, len, outdata);
				if (len > 0) {
					memcpy(datpos, outdata, len);
					sec.data_len += len;
				}
			}
		}
	}

	virtual void destor_write_fun()final
	{
		delete this;
	}
};


WIFI_BASE_FUNCTION * GetWIFI_WRITE_SINGLE_CFG(WIFI_INFO & wifi)
{
	return new WIFI_WRITE_SINGLE_DATA(wifi);
}


