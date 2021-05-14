#include "mdc_ctrl.h"
#include "jd_share.h"
#include "svc.h"
#include <thread>
#include "SN1V2_com.h"
#include <mutex>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <list>
#include "timeTableV2.h"
using namespace std;

//去除已经过去的时间节点
static void rm_backV2(std::list<timTableSetV2> & ts)
{
	time_t tms = time(0);

	auto itr = ts.begin();
	while (itr != ts.end()) {
		time_t thistime = itr->tt;
		if (thistime < tms) {
			auto tmpitr = itr;
			itr++;
			ts.erase(tmpitr);
		} else {
			break;
		}
	}
}


struct jdtablesvc :public JDAUTOSEND {
	jdtablesvc()
	{
		scan_file(this, force_time_table_save_path);
		thread watch(SetWatchFile, force_time_table_save_path, scan_file, this);
		watch.detach();
	}
	std::mutex tableLock;

	std::list<timTableSetV2> timsetV2;
	void _scan_file(const char * fil)
	{
		std::unique_lock<std::mutex> lk(tableLock);
		ERR_STA err;
		//清空整张mdc运行表
		timsetV2.clear();
		//添加sn1表
		std::vector<timTableSet> timeset;
		if ((err = load_table(force_time_table_save_path, timeset)) != err_ok) {
			cout << "fail to load " << endl;
		} else {
			cout << "sn1 load succ ,size = " << timeset.size() << endl;
			//添加sn1表
			convertTimTable2V2(timeset, timsetV2);
		}
		//添加其他类型表单
		FixTimeTableV2(timsetV2);
	}


	static void scan_file(void * p, const char * fil)
	{
		if (!p || !fil) {
			return;
		}

		jdtablesvc *thissvc = (jdtablesvc*)p;

		thissvc->_scan_file(fil);
	}

	const int max_retry_cnt = 5;


	struct AIM {
		int succ_flag;
		int retry_cnt;
	}aim[2];

	uint64_t last_send_tim;
	int send_period_s = 5;

	virtual int need_service(JD_INFO & injif) final
	{		
		MDC_INFO& jif = (MDC_INFO &)injif;
		if (jif.work_mod != mdc_mode_table) {
			return 0;
		}
		timeval tv;
		gettimeofday(&tv, nullptr);

		if (timsetV2.empty()) {
			return 0;
		}

		uint64_t now_send_tim = tv.tv_sec / send_period_s;

		if (now_send_tim != last_send_tim) {
			last_send_tim = now_send_tim;
			return 1;
		}
		//send every 5 second
		return 0;
	}

	virtual void service_pro(JD_INFO & injif)final
	{
		printf("jdsvc_table -----------service_pro\n");
		MDC_INFO& jif = (MDC_INFO &)injif;
		std::unique_lock<std::mutex> lk(tableLock);

		rm_backV2(timsetV2);
		if (!timsetV2.empty()) {
			auto psn1 = jif.psn1;
			psn1->helo_status = psn1->Helo_not_ready;

			if (jif.work_mod != mdc_mode_table) {
				return;
			}

			float nextdeg[2] = { timsetV2.begin()->YxAng  , timsetV2.begin()->ZxAng };

			for (int i = 0; i < 2; i++) {

				if (0 == jif.mdcCtrl[i].sta.runningflg) {
					jif.mdcCtrl[i].manual.trig_set(nextdeg[i]);
				}
			}
		}
	}

private:
	int searchUncoplete()
	{
		for (auto &thisaim : aim) {
			if (thisaim.succ_flag == 0 && thisaim.retry_cnt < max_retry_cnt) {
				return  std::distance(&aim[0], &thisaim);
			}
		}
		return -1;
	}
};

static jdtablesvc jsvc;

JDAUTOSEND * jdsvc_table()
{
	return &jsvc;
}