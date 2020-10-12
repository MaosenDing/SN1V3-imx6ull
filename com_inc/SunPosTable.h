#ifndef __sunpostable_h___
#define __sunpostable_h___
#include <time.h>
#include <memory>
#include <vector>
#include <sn1v3cfg.h>
struct SUNPOS {
	time_t tt;

	double ZR_u;
	double ZR_v;
	double ZR_At;
	double ZR_Az;

	double SD_u;
	double SD_v;
	double SD_At;
	double SD_Az;

	double a0;
	double a1;
	double a2;
	double a3;

	double d[2][3];
	double test[2];
};

std::shared_ptr<std::vector<SUNPOS> > createTable(Tg_table &tg_table, int year, int mon, int day);

//r1 r2 默认取1.0f
int ConAlg(float ut, float vt, float ZR_u, float ZR_v, float ZR_At, float ZR_Az, float r1, float r2,
	   float q, float fx, float fy, float x_pos_2, float y_pos_2, float *At, float *Az,
	   int *SpdSig_At, int *SpdSig_Az, int *Slpflg);

//保存表
int saveSunTable(std::shared_ptr<std::vector<SUNPOS> > &tab, const char *savePath);
//生成表
std::shared_ptr<std::vector<SUNPOS> >
GenerateSunTable(int year, int mon, int day, int startHour, int endHour, double lati, double longti,
		 double temperature, double pressure, double delta_T, double elevation,
		 double focus_x, double focus_y, double cam_pixSize, //焦距 相元大小
		 double cam_rotAnglex, double cam_rotAngley, double cam_rotAnglez, //相机安装角度
		 double heliopoint_x, double heliopoint_y, double heliopoint_z, //定日镜坐标
		 double aimpoint_x, double aimpoint_y, double aim_point_z, //指向点坐标
		 double aimpoint1_x, double aimpoint1_y, double aim_point1_z, //指向点坐标
		 double Coef1, double Coef2, double Coef3, double Coef4, //矫正系数
		 int viewAnglev, int viewAngleh, //行列
		 double cam_viewAngle_h, double cam_viewAngle_v, float centerU, float centerV);

int find_useful_pos(int hour, int min, int sec, std::vector<SUNPOS> &tab, SUNPOS &retcfg);
#define TABLE_POS_INTERVAL_SECONDS (5)
#endif

