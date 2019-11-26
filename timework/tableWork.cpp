#include "tableWork.h"
#include <fstream>
#include <iostream>
#include "time_interval.h"
#include "SN1V2_com.h"
#include "errHandle/errHandle.h"
#include <thread>
using namespace std;

#if USING_DUMMY_CAP_SRC > 0

dummyWork2::dummyWork2(int thr, float per, char *pa ,char * aimpp,float inQuality,int org,int bin) 
	:thres(thr), thresPer(per), rdpath(pa), sp(getAllFileName(rdpath)),aimPath(aimpp),QualityThres(inQuality),save_org(org),save_bin(bin)
	, index(0)
{}

static void PROCESS_RESULT_INIT(PROCESS_RESULT  &pro, timTableSet & tts)
{
	pro.TableTimeGetImg = tts.tt;
}

__attribute__((unused))
static void SetStampAsTimeTable(tm & reftime, PROCESS_RESULT  &pro)
{
#if 0
	//记录结果为拍摄开始时间
	time_t tt = pro.timGetImg;
#else
	//记录结果为时间表时间
	time_t tt = pro.TableTimeGetImg;
#endif
	localtime_r(&tt, &reftime);
}

__attribute__((unused))
static void SetStampAsCaptureTime(tm & reftime, PROCESS_RESULT  &pro)
{
#if 1
	//记录结果为拍摄开始时间
	time_t tt = pro.timGetImg;
#else
	//记录结果为时间表时间
	time_t tt = pro.TableTimeGetImg;
#endif
	localtime_r(&tt, &reftime);
}



void dummyWork2::work(timTableSet & tts, int flag)
{
	if (flag == 0)
	{
		const char * fName = sp->at(index).c_str();
		index++;

		{
			TimeInterval p("time:");
			cout << "----------------------------" << endl;
			PROCESS_RESULT pro;
			PROCESS_RESULT_INIT(pro, tts);
			ERR_STA err = ImageTestRGBFile(fName, rdpath, aimPath, 2592, 1944, pro, thres, thresPer, NULL);

			tm reftime;
			SetStampAsTimeTable(reftime, pro);

			char outName[64];
			sprintf(outName, "%02d_%02d_%02d.jpg.rgb565", reftime.tm_hour, reftime.tm_min, reftime.tm_sec);

			if (err == err_ok)
			{

				{//pair log

					cout << "input file =" << fName << endl;
					cout << "output file =" << outName << endl;

					char pairName[64];
					sprintf(pairName, "%s/pair.txt", aimPath);
					ofstream pair(pairName, ofstream::app);
					if (pair.good())
					{
						pair << "input file =" << fName << endl;
						pair << "output file =" << outName << endl;
					}
				}

				char buff[128];

				sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d,%f,%f,%d,%.3f,%.3f,%.6f,%.6f,%.6f"
					, reftime.tm_year + 1900, reftime.tm_mon + 1, reftime.tm_mday
					, reftime.tm_hour, reftime.tm_min, reftime.tm_sec
					, pro.diff_y, pro.diff_x, pro.pixCnt
					, tts.ZxAng, tts.YxAng
					, tts.RIx, tts.RIy, tts.RIz
					);
				cout << buff << endl;
				cout << "*****************************" << endl;

				sprintf(buff, "%d,%f,%f,%d,%.3f,%.3f,%.6f,%.6f,%.6f"
					, reftime.tm_hour * 3600 + reftime.tm_min * 60 + reftime.tm_sec
					, pro.diff_y, pro.diff_x, pro.pixCnt
					, tts.ZxAng, tts.YxAng
					, tts.RIx, tts.RIy, tts.RIz
					);


				if (QualityThres < pro.quality)
				{
					char csvPath[64];
					sprintf(csvPath, "%s/sr.txt", aimPath);
					ofstream txtRes(csvPath, ofstream::app);
					txtRes << buff << endl;
				}
				else
				{
					cout << buff << endl;
					cout << "bad quality" << endl;
				}
			}
			else
			{
				cerr << outName << ",error code =" << (int)err << endl;
				cout << outName << ",error code =" << (int)err << endl;
			}
		}
	}
	else
	{		
		time_t tt = tts.tt;
		tm reftime;
		localtime_r(&tt, &reftime);
		char buff[128];

		sprintf(buff, "img cap fail:%04d-%02d-%02d %02d:%02d:%02d"
			, reftime.tm_year + 1900, reftime.tm_mon + 1, reftime.tm_mday
			, reftime.tm_hour, reftime.tm_min, reftime.tm_sec);

		cout << buff << endl;
	}
}


#endif


capWork::capWork(char * aimpp,SN1_CFG & inCfg,SN1_SHM * in_psn1,char * in_res_path) 
	:aimPath(aimpp) ,res_path(in_res_path), cfg(inCfg), psn1(in_psn1)
	,index(0), lastEnd(chrono::system_clock::now())
{}




ERR_STA ImageCapRGB(const char * dstPath, int width, int height, PROCESS_RESULT & res, int thres, float thresPer
	, bool ORGjpgSaveFlag, bool BINjpgSaveFlag, unsigned int MinCntCrp, const unsigned int gain, const unsigned int expo
	, const int horFlip, const int VerFlip
);

void capWork::cap_miss_process(timTableSet & time)
{
	char buff[128];
	int num = 0;

	auto thisTick = chrono::system_clock::now();

	time_t last_t = chrono::system_clock::to_time_t(lastEnd);
	time_t now_t = chrono::system_clock::to_time_t(thisTick);

	tm last_tm, now_tm;
	tm aim_tm;
	localtime_r(&last_t, &last_tm);
	localtime_r(&now_t, &now_tm);
	localtime_r(&now_t, &aim_tm);

	aim_tm.tm_hour = time.tm_hour;
	aim_tm.tm_min = time.tm_min;
	aim_tm.tm_sec = time.tm_sec;

	time_t aim_t = mktime(&aim_tm);
	auto aim = chrono::system_clock::from_time_t(aim_t);
	float tick = std::chrono::duration_cast<std::chrono::duration<float>>(aim - thisTick).count();


	{
		tm & reftm = last_tm;
		num += sprintf(buff + num, "last = %d,%d,%d \n", reftm.tm_hour, reftm.tm_min, reftm.tm_sec);
	}

	{
		tm & reftm = now_tm;
		num += sprintf(buff + num, "now = %d,%d,%d \n", reftm.tm_hour, reftm.tm_min, reftm.tm_sec);
	}

	num += sprintf(buff + num, "aim = %d,%d,%d \n", time.tm_hour, time.tm_min, time.tm_sec);

	num += sprintf(buff + num, "over tick = %fS\n", tick);
	LOG(WARNING) << buff;
}

void capWork::cap_status_error_process(timTableSet & time)
{
	LOG(WARNING) << "heli not ready -> cap skip" << endl;
	if (psn1) {
		psn1->error_count++;
		psn1->last_error_code = err_Heli_not_ready;
	}
}

void capWork::cap_ok()
{
	if (psn1) {
		psn1->last_error_code = err_ok;
	}
}




void capWork::work(timTableSet & timets, int flag)
{
	cout << "----------------------------" << endl;
	TimeInterval p("cap time:");
	if (flag == 0) {

		//check if mdc error
		time_t cap_end = timets.tt + cfg.max_reserve_time;

#if 0
		printf("psn1 %#X\n", psn1);
#endif
		while (psn1 && (psn1->helo_status != SN1_SHM::Helo_ok) && (time(0) < cap_end))
			usleep(100 * 1000);
#if 0
		printf("time 0 %ld ,end %ld ,res %d ,tt = %ld\n", time(0), cap_end , cfg.max_reserve_time , timets.tt);
#endif
		if (time(0) >= cap_end) {
			cap_status_error_process(timets);
			//just skip capture
			return;
		}

		auto start = chrono::system_clock::now();

		PROCESS_RESULT pro;
		PROCESS_RESULT_INIT(pro, timets);
		ERR_STA err = ImageCapRGB(aimPath, cfg.IMG_WIDTH, cfg.IMG_HEIGTH, pro, cfg.thres, cfg.thresPer, cfg.FLAG_SAVE_ORG, cfg.FLAG_SAVE_BIN, cfg.MinCntGrp
			, cfg.gain, cfg.expo , cfg.isHorisFlip, cfg.isVeriFlip);
		//this_thread::sleep_for(chrono::milliseconds(3000));

		tm reftime;
		SetStampAsTimeTable(reftime , pro);

		//char outName[64];
		//sprintf(outName, "%02d_%02d_%02d.jpg.rgb565", reftime.tm_hour, reftime.tm_min, reftime.tm_sec);

		if (err == err_ok) {
			char buff[128];

			sprintf(buff, "%04d-%02d-%02d %02d:%02d:%02d,%f,%f,%d,%.3f,%.3f,%.6f,%.6f,%.6f,quality = %f"
				, reftime.tm_year + 1900, reftime.tm_mon + 1, reftime.tm_mday
				, reftime.tm_hour, reftime.tm_min, reftime.tm_sec
				, pro.diff_y, pro.diff_x, pro.pixCnt
				, timets.ZxAng, timets.YxAng
				, timets.RIx, timets.RIy, timets.RIz
				, pro.quality
			);
			cout << buff << endl;
			cout << "*****************************" << endl;

			if (cfg.qualityThres < pro.quality) {
#if 0 
				sprintf(buff, "%d,%f,%f,%d,%.3f,%.3f,%.6f,%.6f,%.6f"
					, reftime.tm_hour * 3600 + reftime.tm_min * 60 + reftime.tm_sec
					, pro.diff_y, pro.diff_x, pro.pixCnt
					, timets.ZxAng, timets.YxAng
					, timets.RIx, timets.RIy, timets.RIz
				);
#else
				sprintf(buff, "%02d%02d%02d,%f,%f,%d,%.3f,%.3f,%.6f,%.6f,%.6f"
					, reftime.tm_hour, reftime.tm_min, reftime.tm_sec
					, pro.diff_y, pro.diff_x, pro.pixCnt
					, timets.ZxAng, timets.YxAng
					, timets.RIx, timets.RIy, timets.RIz
				);
#endif
				if (res_path) {
					ofstream txtRes(res_path, ofstream::app);
					txtRes << buff << endl;
				}
			} else {
				LOG(WARNING) << buff << "bad quality" << endl;
				//cout << "bad quality" << endl;
			}
			cap_ok();
			lastEnd = chrono::system_clock::now();
		} else {
			if (psn1) {
				psn1->error_count++;
				psn1->last_error_code = err;
			}
			LOG(INFO) << "error code =" << (int)err;
		}
		auto end = chrono::system_clock::now();
		float tick = std::chrono::duration_cast<std::chrono::duration<float>>(end - start).count();
		LOG_IF(ERROR, tick > 4) << "cap over tick ,tim = " << tick << "S";
	} else {
		cap_miss_process(timets);
	}
}


