#include "JDcomhead.h"
#include "jd_share.h"
#include "svc.h"
#include <thread>
#include "SN1V2_com.h"
#include <mutex>
#include <iostream>
#include <algorithm>
#include <math.h>
using namespace std;

static void rm_back(std::vector<timTableSet> & ts)
{
	time_t tms = time(0);
	while (true) {
		if (ts.empty()) {
			break;
		}
		time_t tmpt = ts.back().tt;
		if (tmpt < tms) {
			ts.pop_back();
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
	std::vector<timTableSet> timeset;

	void trig_cpl(JD_FRAME & jfr)
	{
		//if (jfr.jd_data_len > 0) {
		//	aim[using_index].succ_flag = 1;
		//}
	}

	void _scan_file(const char * fil)
	{
		std::unique_lock<std::mutex> lk(tableLock);
		timeset.clear();
		ERR_STA err;
		if ((err = load_table(force_time_table_save_path, timeset)) != err_ok) {
			cout << "fail to load " << endl;
		} else {
			cout << "succ to load ,size = " << timeset.size() << endl;

			time_t now = time(0);
			tm  reftm;
			localtime_r(&now, &reftm);

			for (auto &p : timeset) {
				reftm.tm_hour = p.tm_hour;
				reftm.tm_min = p.tm_min;
				reftm.tm_sec = p.tm_sec;

				p.tt = mktime(&reftm);
			}

			std::sort(timeset.begin(), timeset.end(), [](const timTableSet & a, const timTableSet &  b) {return a.tt > b.tt; });
			rm_back(timeset);
			cout << "table size = " << timeset.size() << endl;
		}
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

	virtual int need_service(JD_INFO & jif) final
	{
		if (jif.JD_MOD != mdc_mode_table) {
			return 0;
		}

		timeval tv;
		gettimeofday(&tv, nullptr);

		if (timeset.empty()) {
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

	//bool checkDeg(JD_INFO & jif)
	//{
	//	for (auto & p : jif.mdcCtrl) {
	//		double tmp = p.manual.manual_deg - p.sta.deg;
	//		if (fabs(tmp) > 0.05f) {
	//			return false;
	//		}
	//	}
	//	return true;
	//}


	virtual void service_pro(JD_INFO & jif)final
	{
		std::unique_lock<std::mutex> lk(tableLock);
		rm_back(timeset);
		if (!timeset.empty()) {
			auto &pp = (JD_INFO_TIM &)jif;
			auto psn1 = pp.psn1;
			psn1->helo_status = psn1->Helo_not_ready;

			float f0 = fabs(timeset.back().YxAng - jif.mdcCtrl[0].sta.deg);
			float f1 = fabs(timeset.back().ZxAng - jif.mdcCtrl[1].sta.deg);

			if (jif.JD_MOD != mdc_mode_table) {
				return;
			}

			if (0 == jif.mdcCtrl[0].sta.runningflg) {
				jif.mdcCtrl[0].manual.trig_set(timeset.back().YxAng);
			}

			if (0 == jif.mdcCtrl[1].sta.runningflg) {
				jif.mdcCtrl[1].manual.trig_set(timeset.back().ZxAng);
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

	int mkPack(char * databuff, int maxsz, std::vector<timTableSet> &timesettmp, int using_index)
	{
		int len = 0;
		for (auto & p : timesettmp) {
			len += snprintf(databuff, maxsz - len, "%d-%d-%d,%.4f\n", p.tm_hour, p.tm_min, p.tm_sec, using_index ? p.ZxAng : p.YxAng);
		}
		return len;
	}
};

static jdtablesvc jsvc;

JDAUTOSEND * jdsvc_table()
{
	return &jsvc;
}


int JD_table_rec(JD_INFO & jif, JD_FRAME & jfr)
{
	JD_INFO_TIM & jit = (JD_INFO_TIM &)jif;

	jsvc.trig_cpl(jfr);

	return JD_OK;
}


