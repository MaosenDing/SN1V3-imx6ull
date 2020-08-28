#include "sn1v3cfg.h"
#include "tableWork.h"
#include "configOperator.h"
#include <memory>
#include <vector>
#include "SunPosTable.h"
#include <fstream>
using namespace std;



int saveSunTable(shared_ptr <vector <SUNPOS >> &tab ,const char * savePath)
{

	ofstream filout(savePath, ios::trunc);

	char buff[256];

	for (auto & point : *tab) {
		int hour = point.tt / 3600;
		int min = point.tt / 60 % 60;
		int sec = point.tt % 60;

		int size = snprintf(buff, 256,
			"%02d:%02d:%02d,%f,%f,%f,%f"
			",%f,%f,%f,%f"
			"\n",
			hour, min, sec,
			point.ZR_u, point.ZR_v, point.SD_u, point.SD_v
			, point.ZR_At, point.ZR_Az, point.SD_At, point.SD_Az
		);
		filout.write(buff, size);
	}


}


int createTabletest(int argc, char* argv[])
{
	Tg_table tg_table;
	scanfAllTable(tg_table, Mask_All);

	time_t now = time(0);
	tm t2;

	localtime_r(&now, &t2);

	const int year = t2.tm_year + 1900;
	const int mon = t2.tm_mon + 1;
	const int day = t2.tm_mday;


	shared_ptr< vector < SUNPOS > > tab = GenerateSunTable(year, mon, day, 8, 17, tg_table.T1.latitude, tg_table.T1.longtitude,
		tg_table.T1.Temperature, tg_table.T1.Pressure, tg_table.T1.DeltaT, tg_table.T1.elevation, tg_table.T6.SN1_P4_x, tg_table.T6.SN1_P4_y, tg_table.T6.SN1_P3,
		tg_table.T6.cam_rotAnglex, tg_table.T6.cam_rotAngley, tg_table.T6.cam_rotAnglez,
		tg_table.T1.HeliostatPointX, tg_table.T1.HeliostatPointY, tg_table.T1.HeliostatPointZ,
		tg_table.T1.AimPointX1, tg_table.T1.AimPointY1, tg_table.T1.AimPointZ1,
		tg_table.T1.AimPointX2, tg_table.T1.AimPointY2, tg_table.T1.AimPointZ2,
		tg_table.T6.MappingCoefficients1, tg_table.T6.MappingCoefficients2, tg_table.T6.MappingCoefficients3, tg_table.T6.MappingCoefficients4,
		tg_table.T6.SN1_P1, tg_table.T6.SN1_P2,
		tg_table.T6.cam_viewAngleh, tg_table.T6.cam_viewAnglev,
		tg_table.T6.centerU, tg_table.T6.centerV
	);
	saveSunTable(tab,"1.txt");
	return 0;
}

shared_ptr< vector < SUNPOS > > createTable(Tg_table &tg_table,int year,int mon,int day)
{
	printf("createTable : year = %d ,mon = %d ,day = %d\n", year, mon, day);
	shared_ptr< vector < SUNPOS > > tab = GenerateSunTable(year, mon, day, 8, 17, tg_table.T1.latitude, tg_table.T1.longtitude,
		tg_table.T1.Temperature, tg_table.T1.Pressure, tg_table.T1.DeltaT, tg_table.T1.elevation, tg_table.T6.SN1_P4_x, tg_table.T6.SN1_P4_y, tg_table.T6.SN1_P3,
		tg_table.T6.cam_rotAnglex, tg_table.T6.cam_rotAngley, tg_table.T6.cam_rotAnglez,
		tg_table.T1.HeliostatPointX, tg_table.T1.HeliostatPointY, tg_table.T1.HeliostatPointZ,
		tg_table.T1.AimPointX1, tg_table.T1.AimPointY1, tg_table.T1.AimPointZ1,
		tg_table.T1.AimPointX2, tg_table.T1.AimPointY2, tg_table.T1.AimPointZ2,
		tg_table.T6.MappingCoefficients1, tg_table.T6.MappingCoefficients2, tg_table.T6.MappingCoefficients3, tg_table.T6.MappingCoefficients4,
		tg_table.T6.SN1_P1, tg_table.T6.SN1_P2,
		tg_table.T6.cam_viewAngleh, tg_table.T6.cam_viewAnglev,
		tg_table.T6.centerU, tg_table.T6.centerV
	);
	return tab;
}


static int compare_sec(const void * a, const void * b)
{
	SUNPOS * suna = (SUNPOS *)a;
	SUNPOS * sunb = (SUNPOS *)b;

	return suna->tt / 5 - sunb->tt / 5;
}

int find_useful_pos(int hour, int min, int sec, vector<SUNPOS> & tab, SUNPOS & retcfg)
{
	int using_sec = hour * 3600 + min * 60 + sec;

	SUNPOS tmppos;
	tmppos.tt = using_sec;

	SUNPOS * ret = (SUNPOS *)bsearch(&tmppos, &tab[0], tab.size(), sizeof(SUNPOS), compare_sec);

	if (ret) {
		retcfg = *ret;
		return 0;
	}
	return -1;
}




