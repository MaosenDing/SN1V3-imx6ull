#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "errHandle/errHandle.h"
#include "SN1V2_com.h"


struct stu1{
	double SAt;
	double SAz;
	double RIx;
	double RIy;
	double RIz;
};
#ifndef M_PI
#define  M_PI (3.14159265358979323846)
#endif
// FUNC[0] 太阳姿态计算
struct stu1 SolarPos(int Year, int Month, int Day, int Hour, int Minute, int Second, int Delta_T, double Longitude, double Latitude, int E, int P, int T)
{
	// 检测地时区
	int TimeZoneLocal = ((Longitude > 0 ? 1 : (Longitude == 0 ? 0 : -1))*(floor((abs(Longitude) - 7.5) / 15) + 1));

	int AA, BB;
	if (Month <= 2)
	{
		Year = Year - 1;
		Month = Month + 12;
	}
	double Day2 = Day + (Hour - TimeZoneLocal + (double)Minute / 60 + (double)Second / 3600) / 24;
	AA = (int)(Year / 100);
	if ((Year < 1582) || ((Year == 1582) && (Month < 10)) || ((Year == 1582) && (Month == 10) && (Day < 4)))
	{
		BB = 0;
	}		
	else
	{
		BB = 2 - AA + (int)(AA / 4);
	}
	double JD = (double)((int)(365.25 * (Year + 4716)) + (int)(30.6001 * (Month + 1)) + Day2 + BB - 1524.5);		//计算儒略日
	double JDE = JD + (double)((double)(Delta_T) / 86400);		//计算儒略历书日 Julian Ephemeris Day，ResJu[1]
	double JC = (JD - 2451545) / 36525;  //计算儒略世纪数Julian Century，ResJu[2]
	double JCE = (JDE - 2451545) / 36525;  //计算儒略历书世纪数Julian Ephemeris Century，ResJu[3]
	double JME = JCE / 10;  //计算儒略历书千年数Julian Ephemeris Millennium，ResJu[4]

	// FUNC[1] 计算以太阳为中心，地球相关参数
	// L为太阳为中心时的黄道坐标下，地球的黄经
	// B为太阳为中心时的黄道坐标下，地球的黄纬
	// R为太阳为中心时的黄道坐标下，地球的向径（单位为天文单位，地月质心到太阳中心的距离为一个天文单位）

	const double L0_T[64][4] = { { 0, 175347046, 0, 0 }, { 1, 3341656, 4.6692568, 6283.07585 }, { 2, 34894, 4.6261, 12566.1517 }, \
	{ 3, 3497, 2.7441, 5753.3849 }, { 4, 3418, 2.8289, 3.5231 }, { 5, 3136, 3.6277, 77713.7715 }, \
	{ 6, 2676, 4.4181, 7860.4194 }, { 7, 2343, 6.1352, 3930.2097 }, { 8, 1324, 0.7425, 11506.7698 }, \
	{ 9, 1273, 2.0371, 529.691 }, { 10, 1199, 1.1096, 1577.3435 }, { 11, 990, 5.233, 5884.927 }, \
	{ 12, 902, 2.045, 26.298 }, { 13, 857, 3.508, 398.149 }, { 14, 780, 1.179, 5223.694 }, \
	{15, 753, 2.533, 5507.553}, { 16, 505, 4.583, 18849.228 }, { 17, 492, 4.205, 775.523 }, \
	{18, 357, 2.92, 0.067}, { 19, 317, 5.849, 11790.629 }, { 20, 284, 1.899, 796.298 }, \
	{21, 271, 0.315, 10977.079}, { 22, 243, 0.345, 5486.778 }, { 23, 206, 4.806, 2544.314 }, \
	{ 24, 205, 1.869, 5573.143 }, { 25, 202, 2.458, 6069.777 }, { 26, 156, 0.833, 213.299 }, \
	{ 27, 132, 3.411, 2942.463 }, { 28, 126, 1.083, 20.775 }, { 29, 115, 0.645, 0.98 }, \
	{ 30, 103, 0.636, 4694.003 }, { 31, 102, 0.976, 15720.839 }, { 32, 102, 4.267, 7.114 }, \
	{ 33, 99, 6.21, 2146.17 }, { 34, 98, 0.68, 155.42 }, { 35, 86, 5.98, 161000.69 }, \
	{ 36, 85, 1.3, 6275.96 }, { 37, 85, 3.67, 71430.7 }, { 38, 80, 1.81, 17260.15 }, \
	{ 39, 79, 3.04, 12036.46 }, { 40, 75, 1.76, 5088.63 }, { 41, 74, 3.5, 3154.69 }, \
	{ 42, 74, 4.68, 801.82 }, { 43, 70, 0.83, 9437.76 }, { 44, 62, 3.98, 8827.39 }, \
	{ 45, 61, 1.82, 7084.9 }, { 46, 57, 2.78, 6286.6 }, { 47, 56, 4.39, 14143.5 }, \
	{ 48, 56, 3.47, 6279.55 }, { 49, 52, 0.19, 12139.55 }, { 50, 52, 1.33, 1748.02 }, \
	{ 51, 51, 0.28, 5856.48 }, { 52, 49, 0.49, 1194.45 }, { 53, 41, 5.37, 8429.24 }, \
	{ 54, 41, 2.4, 19651.05 }, { 55, 39, 6.17, 10447.39 }, { 56, 37, 6.04, 10213.29 }, \
	{ 57, 37, 2.57, 1059.38 }, { 58, 36, 1.71, 2352.87 }, { 59, 36, 1.78, 6812.77 }, \
	{ 60, 33, 0.59, 17789.85 }, { 61, 30, 0.44, 83996.85 }, { 62, 30, 2.74, 1349.87 }, { 63, 25, 3.16, 4690.48 } };

	const double L1_T[34][4] = {{ 0, 628331966747, 0, 0 }, { 1, 206059, 2.678235, 6283.07585 }, { 2, 4303, 2.6351, 12566.1517 }, \
	{ 3, 425, 1.59, 3.523 }, { 4, 119, 5.796, 26.298 }, { 5, 109, 2.966, 1577.344 }, { 6, 93, 2.59, 18849.23 }, \
	{ 7, 72, 1.14, 529.69 }, { 8, 68, 1.87, 398.15 }, { 9, 67, 4.41, 5507.55 }, { 10, 59, 2.89, 5223.69 }, \
	{ 11, 56, 2.17, 155.42 }, { 12, 45, 0.4, 796.3 }, { 13, 36, 0.47, 775.52 }, { 14, 29, 2.65, 7.11 }, \
	{ 15, 21, 5.34, 0.98 }, { 16, 19, 1.85, 5486.78 }, { 17, 19, 4.97, 213.3 }, { 18, 17, 2.99, 6275.96 }, \
	{ 19, 16, 0.03, 2544.31 }, { 20, 16, 1.43, 2146.17 }, { 21, 15, 1.21, 10977.08 }, { 22, 12, 2.83, 1748.02 }, \
	{ 23, 12, 3.26, 5088.63 }, { 24, 12, 5.27, 1194.45 }, { 25, 12, 2.08, 4694 }, { 26, 11, 0.77, 553.57 }, \
	{ 27, 10, 1.3, 6286.6 }, { 28, 10, 4.24, 1349.87 }, { 29, 9, 2.7, 242.73 }, { 30, 9, 5.64, 951.72 }, \
	{ 31, 8, 5.3, 2352.87 }, { 32, 6, 2.65, 9437.76 }, { 33, 6, 4.67, 4690.48 } };

	const double L2_T[20][4] = { { 0, 52919, 0, 0 }, { 1, 8720, 1.0721, 6283.0758 }, { 2, 309, 0.867, 12566.152 }, { 3, 27, 0.05, 3.52 }, \
	{4, 16, 5.19, 26.3}, { 5, 16, 3.68, 155.42 }, { 6, 10, 0.76, 18849.23 }, { 7, 9, 2.06, 77713.77 }, \
	{8, 7, 0.83, 775.52}, { 9, 5, 4.66, 1577.34 }, { 10, 4, 1.03, 7.11 }, { 11, 4, 3.44, 5573.14 }, \
	{12, 3, 5.14, 796.3}, { 13, 3, 6.05, 5507.55 }, { 14, 3, 1.19, 242.73 }, { 15, 3, 6.12, 529.69 }, \
	{16, 3, 0.31, 398.15}, { 17, 3, 2.28, 553.57 }, { 18, 2, 4.38, 5223.69 }, { 19, 2, 3.75, 0.98 } };

	const double L3_T[7][4] = { { 0, 289, 5.844, 6283.076 }, { 1, 35, 0, 0 }, { 2, 17, 5.49, 12566.15 }, { 3, 3, 5.2, 155.42 }, \
	{ 4, 1, 4.72, 3.52 }, { 5, 1, 5.3, 18849.23 }, { 6, 1, 5.97, 242.73 } };

	const double L4_T[3][4] = { { 0, 114, 3.142, 0 }, { 1, 8, 4.13, 6283.08 }, { 2, 1, 3.84, 12566.15 } };

	const double L5_T[1][4] = { { 0, 1, 3.14, 0 } };

	const double B0_T[5][4] = { { 0, 280, 3.199, 84334.662 }, { 1, 102, 5.422, 5507.553 }, { 2, 80, 3.88, 5223.69 }, \
	{ 3, 44, 3.7, 2352.87 }, { 4, 32, 4, 1577.34 } };

	const double B1_T[2][4] = { { 0, 9, 3.9, 5507.55 }, { 1, 6, 1.73, 5223.69 } };

	const double R0_T[40][4] = { { 0, 100013989, 0, 0 }, { 1, 1670700, 3.0984635, 6283.07585 }, { 2, 13956, 3.05525, 12566.1517 }, \
	{ 3, 3084, 5.1985, 77713.7715 }, { 4, 1628, 1.1739, 5753.3849 }, { 5, 1576, 2.8469, 7860.4194 }, \
	{ 6, 925, 5.453, 11506.77 }, { 7, 542, 4.564, 3930.21 }, { 8, 472, 3.661, 5884.927 }, { 9, 346, 0.964, 5507.553 }, \
	{ 10, 329, 5.9, 5223.694 }, { 11, 307, 0.299, 5573.143 }, { 12, 243, 4.273, 11790.629 }, { 13, 212, 5.847, 1577.344 }, \
	{ 14, 186, 5.022, 10977.079 }, { 15, 175, 3.012, 18849.228 }, { 16, 110, 5.055, 5486.778 }, { 17, 98, 0.89, 6069.78 }, \
	{ 18, 86, 5.69, 15720.84 }, { 19, 86, 1.27, 161000.69 }, { 20, 65, 0.27, 17260.15 }, { 21, 63, 0.92, 529.69 }, \
	{ 22, 57, 2.01, 83996.85 }, { 23, 56, 5.24, 71430.7 }, { 24, 49, 3.25, 2544.31 }, { 25, 47, 2.58, 775.52 }, \
	{ 26, 45, 5.54, 9437.76 }, { 27, 43, 6.01, 6275.96 }, { 28, 39, 5.36, 4694 }, { 29, 38, 2.39, 8827.39 }, \
	{ 30, 37, 0.83, 19651.05 }, { 31, 37, 4.9, 12139.55 }, { 32, 36, 1.67, 12036.46 }, { 33, 35, 1.84, 2942.46 }, \
	{ 34, 33, 0.24, 7084.9 }, { 35, 32, 0.18, 5088.63 }, { 36, 32, 1.78, 398.15 }, { 37, 28, 1.21, 6286.6 }, \
	{ 38, 28, 1.9, 6279.55 }, { 39, 26, 4.59, 10447.39 } };

	const double R1_T[10][4] = { { 0, 103019, 1.10749, 6283.07585 }, { 1, 1721, 1.0644, 12566.1517 }, { 2, 702, 3.142, 0 }, \
	{ 3, 32, 1.02, 18849.23 }, { 4, 31, 2.84, 5507.55 }, { 5, 25, 1.32, 5223.69 }, { 6, 18, 1.42, 1577.34 }, \
	{ 7, 10, 5.91, 10977.08 }, { 8, 9, 1.42, 6275.96 }, { 9, 9, 0.27, 5486.78 } };

	const double R2_T[6][4] = { { 0, 4359, 5.7846, 6283.0758 }, { 1, 124, 5.579, 12566.152 }, { 2, 12, 3.14, 0 }, \
	{ 3, 9, 3.63, 77713.77 }, { 4, 6, 1.87, 5573.14 }, { 5, 3, 5.47, 18849.23 } };

	const double R3_T[2][4] = { { 0, 145, 4.273, 6283.076 }, { 1, 7, 3.92, 12566.15 } };

	const double R4_T[1][4] = { { 0, 4, 2.56, 6283.08 } };

	double L0 = 0;
	int i;

	for (i = 0; i <= 63; i++)
	{
		L0 = L0 + (L0_T[i][1] * cos(L0_T[i][2] + L0_T[i][3] * JME));
	}

	double L1 = 0;
	for (i = 0; i <= 33; i++)
	{
		L1 = L1 + (L1_T[i][1] * cos(L1_T[i][2] + L1_T[i][3] * JME));
	}

	double L2 = 0;
	for (i = 0; i <= 19; i++)
	{
		L2 = L2 + (L2_T[i][1] * cos(L2_T[i][2] + L2_T[i][3] * JME));
	}

	double L3 = 0;
	for (i = 0; i <= 6; i++)
	{
		L3 = L3 + (L3_T[i][1] * cos(L3_T[i][2] + L3_T[i][3] * JME));
	}

	double L4 = 0;
	for (i = 0; i <= 2; i++)
	{
		L4 = L4 + (L4_T[i][1] * cos(L4_T[i][2] + L4_T[i][3] * JME));
	}
	
	double L5 = L5_T[0][1] * cos(L5_T[0][2] + L5_T[0][3] * JME);

	double LinRadians = (L0 + L1*JME + L2*pow(JME,2) + L3*pow(JME,3) + L4*pow(JME,4) + L5*pow(JME,5)) / pow(10.0,8);
	double L = fmod((LinRadians * 180 / M_PI), 360);
	
	double B0 = 0;
	for (i = 0; i <= 4; i++)
	{
		B0 = B0 + (B0_T[i][1] * cos(B0_T[i][2] + B0_T[i][3] * JME));
	}

	double B1 = 0;
	for (i = 0; i <= 1; i++)
	{
		B1 = B1 + (B1_T[i][1] * cos(B1_T[i][2] + B1_T[i][3] * JME));
	}

	double B2 = 0;
	double B3 = 0;
	double B4 = 0;
	double B5 = 0;

	double B_in_Radians = (B0 + B1*JME + B2*pow(JME, 2) + B3*pow(JME, 3) + B4*pow(JME, 4) + B5*pow(JME, 5)) / pow(10, 8);
	double B = B_in_Radians * 180 / M_PI;

	double R0 = 0;
	for (i = 0; i <= 39; i++)
	{
		R0 = R0 + (R0_T[i][1] * cos(R0_T[i][2] + R0_T[i][3] * JME));
	}

	double R1 = 0;
	for (i = 0; i <= 9; i++)
	{
		R1 = R1 + (R1_T[i][1] * cos(R1_T[i][2] + R1_T[i][3] * JME));
	}

	double R2 = 0;
	for (i = 0; i <= 5; i++)
	{
		R2 = R2 + (R2_T[i][1] * cos(R2_T[i][2] + R2_T[i][3] * JME));
	}

	double R3 = 0;
	for (i = 0; i <= 1; i++)
	{
		R3 = R3 + (R3_T[i][1] * cos(R3_T[i][2] + R3_T[i][3] * JME));
	}

	double R4 = R4_T[0][1] * cos(R4_T[0][2] + R4_T[0][3] * JME);
	double R5 = 0;
	double R = (R0 + R1*JME + R2*pow(JME, 2) + R3*pow(JME, 3) + R4*pow(JME, 4) + R5*pow(JME, 5)) / pow(10.0, 8);

	// FUNC[2] 章动引起的太阳黄经、黄赤交角的误差
	// delta_psi为章动引起的黄赤交角的变动
	// delta_epsilon为章动引起的太阳黄经变动
	double X0 = 297.85036 + 445267.111480*JCE - 0.0019142*pow(JCE,2) + pow(JCE,3) / 189474;
	double X1 = 357.52772 + 35999.050340*JCE - 0.0001603*pow(JCE,2) - pow(JCE,3) / 300000;
	double X2 = 134.96298 + 477198.867398*JCE + 0.0086972*pow(JCE,2) + pow(JCE,3) / 56250;
	double X3 = 93.27191 + 483202.017538*JCE - 0.0036825*pow(JCE,2) + pow(JCE,3) / 327270;
	double X4 = 125.04452 - 1934.136261*JCE + 0.0020708*pow(JCE,2) + pow(JCE,3) / 450000;

	double TA43[63][9] = { { 0, 0, 0, 0, 1, -171996, -174.2, 92025, 8.9 }, { -2, 0, 0, 2, 2, -13187, -1.6, 5736, -3.1 },
	{ 0, 0, 0, 2, 2, -2274, -0.2, 977, -0.5 }, { 0, 0, 0, 0, 2, 2062, 0.2, -895, 0.5 },
	{ 0, 1, 0, 0, 0, 1426, -3.4, 54, -0.1 }, { 0, 0, 1, 0, 0, 712, 0.1, -7, 0 },
	{ -2, 1, 0, 2, 2, -517, 1.2, 224, -0.6 }, { 0, 0, 0, 2, 1, -386, -0.4, 200, 0 },
	{ 0, 0, 1, 2, 2, -301, 0, 129, -0.1 }, { -2, -1, 0, 2, 2, 217, -0.5, -95, 0.3 }, { -2, 0, 1, 0, 0, -158, 0, 0, 0 },
	{ -2, 0, 0, 2, 1, 129, 0.1, -70, 0 }, { 0, 0, -1, 2, 2, 123, 0, -53, 0 }, { 2, 0, 0, 0, 0, 63, 0, 0, 0 },
	{ 0, 0, 1, 0, 1, 63, 0.1, -33, 0 }, { 2, 0, -1, 2, 2, -59, 0, 26, 0 }, { 0, 0, -1, 0, 1, -58, -0.1, 32, 0 },
	{ 0, 0, 1, 2, 1, -51, 0, 27, 0 }, { -2, 0, 2, 0, 0, 48, 0, 0, 0 }, { 0, 0, -2, 2, 1, 46, 0, -24, 0 },
	{ 2, 0, 0, 2, 2, -38, 0, 16, 0 }, { 0, 0, 2, 2, 2, -31, 0, 13, 0 }, { 0, 0, 2, 0, 0, 29, 0, 0, 0 },
	{ -2, 0, 1, 2, 2, 29, 0, -12, 0 }, { 0, 0, 0, 2, 0, 26, 0, 0, 0 }, { -2, 0, 0, 2, 0, -22, 0, 0, 0 },
	{ 0, 0, -1, 2, 1, 21, 0, -10, 0 }, { 0, 2, 0, 0, 0, 17, -0.1, 0, 0 }, { 2, 0, -1, 0, 1, 16, 0, -8, 0 },
	{ -2, 2, 0, 2, 2, -16, 0.1, 7, 0 }, { 0, 1, 0, 0, 1, -15, 0, 9, 0 }, { -2, 0, 1, 0, 1, -13, 0, 7, 0 },
	{ 0, -1, 0, 0, 1, -12, 0, 6, 0 }, { 0, 0, 2, -2, 0, 11, 0, 0, 0 }, { 2, 0, -1, 2, 1, -10, 0, 5, 0 },
	{ 2, 0, 1, 2, 2, -8, 0, 3, 0 }, { 0, 1, 0, 2, 2, 7, 0, -3, 0 }, { -2, 1, 1, 0, 0, -7, 0, 0, 0 },
	{ 0, -1, 0, 2, 2, -7, 0, 3, 0 }, { 2, 0, 0, 2, 1, -7, 0, 3, 0 }, { 2, 0, 1, 0, 0, 6, 0, 0, 0 },
	{ -2, 0, 2, 2, 2, 6, 0, -3, 0 }, { -2, 0, 1, 2, 1, 6, 0, -3, 0 }, { 2, 0, -2, 0, 1, -6, 0, 3, 0 },
	{ 2, 0, 0, 0, 1, -6, 0, 3, 0 }, { 0, -1, 1, 0, 0, 5, 0, 0, 0 }, { -2, -1, 0, 2, 1, -5, 0, 3, 0 },
	{ -2, 0, 0, 0, 1, -5, 0, 3, 0 }, { 0, 0, 2, 2, 1, -5, 0, 3, 0 }, { -2, 0, 2, 0, 1, 4, 0, 0, 0 },
	{ -2, 1, 0, 2, 1, 4, 0, 0, 0 }, { 0, 0, 1, -2, 0, 4, 0, 0, 0 }, { -1, 0, 1, 0, 0, -4, 0, 0, 0 },
	{ -2, 1, 0, 0, 0, -4, 0, 0, 0 }, { 1, 0, 0, 0, 0, -4, 0, 0, 0 }, { 0, 0, 1, 2, 0, 3, 0, 0, 0 },
	{ 0, 0, -2, 2, 2, -3, 0, 0, 0 }, { -1, -1, 1, 0, 0, -3, 0, 0, 0 }, { 0, 1, 1, 0, 0, -3, 0, 0, 0 },
	{ 0, -1, 1, 2, 2, -3, 0, 0, 0 }, { 2, -1, -1, 2, 2, -3, 0, 0, 0 }, { 0, 0, 3, 2, 2, -3, 0, 0, 0 },
	{ 2, -1, 0, 2, 2, -3, 0, 0, 0 } };

	double delta_psi = 0;
	double delta_epsilon = 0;
	for (i = 0; i <= 62; i++)
	{
		delta_psi = delta_psi + ((TA43[i][5] + TA43[i][6] * JCE) * sin((X0*TA43[i][0] + X1*TA43[i][1] + X2*TA43[i][2] \
			+ X3*TA43[i][3]	+ X4*TA43[i][4])*M_PI / 180)) ;

		delta_epsilon = delta_epsilon + ((TA43[i][7] + TA43[i][8] * JCE) * cos((X0*TA43[i][0] + X1*TA43[i][1] + X2*TA43[i][2] \
			+ X3*TA43[i][3] + X4*TA43[i][4])*M_PI / 180));
	}
	delta_psi = delta_psi / 36000000;
	delta_epsilon = delta_epsilon / 36000000;

	// FUNC[3] 修正后的黄赤交角计算
	double U = JME / 10;
	double epsilon0 = 84381.448 - 4680.93*U - 1.55*pow(U,2) + 1999.25*pow(U,3) - 51.38*pow(U,4) - 249.67*pow(U,5) \
		- 39.05*pow(U,6) + 7.12*pow(U,7) + 27.87*pow(U,8) + 5.79*pow(U,9) + 2.45*pow(U,10);
	double epsilon = epsilon0 / 3600 + delta_epsilon;

	// FunC[4] 太阳姿态计算
	double THETA = fmod((L + 180), 360);     // 以地球为中心的太阳的黄经，单位为度
	double BETA = -B;   // 以地球为中心的太阳的黄纬，单位为度
	double delta_tau = -20.4898 / (3600 * R);   // 计算光行差，单位为度
	double lambda2 = THETA + delta_psi + delta_tau;  // 计算视太阳黄经，单位为度

	// 计算格林威治子午线处的任意时刻的视恒星时，单位为度
	double upsilon0 = fmod((280.46061837 + 360.98564736629*(JD - 2451545) + 0.000387933*pow(JC, 2) - pow(JC, 3)\
		/ 38710000), 360);
	double upsilon = upsilon0 + delta_psi * cos(epsilon*M_PI / 180);     // 格林威治子午线上的视恒星时
	
	// 计算地心坐标系下的太阳赤经
	double alpha = fmod((atan2((sin(lambda2*M_PI / 180)*cos(epsilon*M_PI / 180) - tan(BETA*M_PI / 180)*sin(epsilon*M_PI / 180)), \
		cos(lambda2*M_PI / 180)) * 180 / M_PI), 360);
	
	// 计算地心坐标系下的太阳赤纬
	double delta = asin(sin(BETA*M_PI / 180)*cos(epsilon*M_PI / 180) + cos(BETA*M_PI / 180)*sin(epsilon*M_PI / 180)*sin(lambda2*M_PI\
		/ 180)) * 180 / M_PI;
	
	// 计算观察者的当地时角
	double H = fmod((upsilon + Longitude - alpha), 360);

	// FUNC[4] 视差对太阳赤经、赤纬、时角的修正
	// alpha_prime、delta_prime、H_prime分别为视差修正后的太阳赤经、赤纬、时角
	// R为地日距离（单位为天文单位）
	// Latitude为观察者地理纬度
	// E为观察者海拔高度
	// H为视差修正前的时角
	// delta为视差修正前的太阳赤纬
	double ksi = 8.794 / (3600 * R) * M_PI / 180;
	double mju = atan(0.99664719*tan(Latitude * M_PI / 180));
	double x = cos(mju) + (double)E / 6378140 * cos(Latitude * M_PI / 180);
	double y = 0.99664719 * sin(mju) + (double)E / 6378140 * sin(Latitude * M_PI / 180);
	double delta_alpha = atan2(-x*sin(ksi)*sin(H* M_PI / 180), (cos(delta * M_PI / 180) \
		- x*sin(ksi)*cos(H* M_PI / 180))) * 180 / M_PI;
	//double alpha_prime = alpha + delta_alpha;
	double H_prime = H - delta_alpha;

	// solar postion algorithms中的公式
	double delta_prime = atan2((sin(delta * M_PI / 180) - y*sin(ksi))*cos(delta_alpha * M_PI / 180), \
		(cos(delta * M_PI / 180) - x*sin(ksi)*cos(H* M_PI / 180))) * 180 / M_PI;

	// 计算地平坐标系下太阳的高度角
    // 不考虑大气折射下的太阳高度角
	double e0 = asin(sin(Latitude * M_PI / 180)*sin(delta_prime* M_PI / 180) + cos(Latitude * M_PI / 180) \
		*cos(delta_prime * M_PI / 180)*cos(H_prime* M_PI / 180)) * 180 / M_PI;

	// 大气折射引起的太阳高度角修正值
	double delta_e = (double)P / 1010 * 283 / (273 + (double)T) * 1.02 / (60 * tan((e0 + 10.3 / (e0 + 5.11))* M_PI / 180));

	double e = e0 + delta_e;      // 修正后的太阳高度角
	double theta = 90 - e;        // 修正后的太阳天顶角

	// 计算地平坐标系的太阳方位角
	// 方向为从南向西
	double TAU = fmod(((atan2(sin(H_prime* M_PI / 180), (cos(H_prime* M_PI / 180)*sin(Latitude* M_PI / 180) \
		- tan(delta_prime* M_PI / 180)*cos(Latitude* M_PI / 180)))) * 180 / M_PI), 360);

	// 方向为从北向东
	double PHI = fmod((TAU + 180), 360);

	// 重新用变量标记
	double At = 90 - theta;     // 太阳高度角
	double Az = PHI;            // 太阳方位角

	struct stu1 SP;
	SP.SAt = At;
	SP.SAz = Az;

	// 入射光矢量
	SP.RIx = -cos(At / 180 * M_PI)*cos(Az / 180 * M_PI);
	SP.RIy = cos(At / 180 * M_PI)*sin(Az / 180 * M_PI);
	SP.RIz = sin(At / 180 * M_PI);

	return SP;

}
#include <errHandle/errHandle.h>
#include <memory>
ERR_STA SHG(int Year, int Month, int Day, char fdir[], int HelioAdjTime, int HelioPreTime, int StartAngle, \
	double Longitude, double Latitude, int E, int P, int T, int Delta_T, double dTHI, double (&PEPs)[8])
{
#if 1
	FILE *sheetfile = fopen(fdir, "wb+");

	if (sheetfile == NULL)
	{
		fprintf(stderr, "%s open error", fdir);
		SN1V2_ERROR_CODE_RET(err_cannot_open_file);
	}
	std::shared_ptr<FILE> Psheet(sheetfile, fclose);
#else
#define sheetfile stdout
#endif
	
	double & EP1 = PEPs[0];
	double & EP2 = PEPs[1];
	double & EP3 = PEPs[2];
	double & EP4 = PEPs[3];
	double & EP5 = PEPs[4];
	double & EP6 = PEPs[5];
	double & EP7 = PEPs[6];
	double & EP8 = PEPs[7];

	struct stu1 tempSP;
	double ZxAng, YxAng, cosY, sinZ, cosZ, temp1;
	int Hour2, Minute2, Second2;
	int Hour, Minute, Second;
	int flag = 0;
	for (Hour = 4; Hour <= 20; Hour++)
	{
		for (Minute = 0; Minute < 60; Minute++)
		{
			for (Second = 0; Second < 60; Second = Second + HelioAdjTime)
			{
				Second2 = Second - HelioPreTime;
				Minute2 = Minute;
				Hour2 = Hour;
				if (Second2 >= 60)
				{
					Second2 = Second2 - 60;
					Minute2 = Minute2 + 1;
				}
				if (Minute2 >= 60)
				{
					Minute2 = Minute2 - 60;
					Hour2 = Hour2 + 1;
				}
	

				tempSP = SolarPos(Year, Month, Day, Hour2, Minute2, Second2, Delta_T, Longitude, Latitude, E, P, T);

				if (tempSP.SAt <= StartAngle)
				{
					flag = 0;
					continue;
				}
				else
				{
					flag = flag + 1;
				}

				cosY = (tempSP.RIz*cos(EP1)*cos(EP2) - tempSP.RIy*sin(EP1)*cos(EP2) + tempSP.RIx*sin(EP2) - pow(sin(EP5), 2))/pow(cos(EP5), 2);
				YxAng = acos(cosY);

				temp1 = pow((cos(EP5)*sin(EP5) - cos(EP5)*cosY*sin(EP5)), 2) + pow((cos(EP5)*sin(YxAng)), 2);
				sinZ = (tempSP.RIy*cos(EP1) + tempSP.RIz*sin(EP1))*cos(EP5)*sin(YxAng) - (tempSP.RIx*cos(EP2) - tempSP.RIz*cos(EP1)*sin(EP2) + tempSP.RIy*sin(EP1)*sin(EP2))*(cos(EP5)*sin(EP5) - cos(EP5)*cosY*sin(EP5));
				sinZ = sinZ / temp1;
				cosZ = (tempSP.RIx*cos(EP2) - tempSP.RIz*cos(EP1)*sin(EP2) + tempSP.RIy*sin(EP1)*sin(EP2))*cos(EP5)*sin(YxAng) + (tempSP.RIy*cos(EP1) + tempSP.RIz*sin(EP1))*(cos(EP5)*sin(EP5) - cos(EP5)*cosY*sin(EP5));
				cosZ = cosZ / temp1;

				//if (sinZ < 0) * (cosZ < 0) 
				//	ZxAng = 2 * M_PI + atan2(sinZ, cosZ);
				//else
				ZxAng = atan2(sinZ, cosZ);				
				if (ZxAng < 0) {
					ZxAng = ZxAng + M_PI * 2;
				}
				YxAng = round(((YxAng*(1 + EP7) - EP4) * 180 / M_PI) * 1000) / 1000;   // 弧度转成角度, 精确到小数点后面三位
				ZxAng = round(((ZxAng*(1 + EP6) - EP3) * 180 / M_PI) * 1000) / 1000;

				Second2 = Second2 + dTHI;
				if (Second2 >= 60)
				{
					Second2 = Second2 - 60;
					Minute2 = Minute2 + 1;
				}
				if (Minute2 >= 60)
				{
					Minute2 = Minute2 - 60;
					Hour2 = Hour2 + 1;
				}
				tempSP = SolarPos(Year, Month, Day, Hour2, Minute2, Second2, Delta_T, Longitude, Latitude, E, P, T);

				int i = fprintf(sheetfile, "%04d-%02d-%02d %02d:%02d:%02d,%.3f,%.3f,%.6f,%.6f,%.6f\r\n"
					, Year, Month, Day
					, Hour,Minute, Second
					, ZxAng, YxAng
					, tempSP.RIx, tempSP.RIy, tempSP.RIz
					);
				if (0 > i)
				{
					SN1V2_ERROR_CODE_RET(err_cannot_write_file);
				}
			}
		}
	}
	return err_ok;
}

#if 1
#include "SN1V2_com.h"
#include <fstream>
using namespace std;
ERR_STA SHG(int Year, int Month, int Day, int HelioAdjTime, int HelioPreTime, int StartAngle, \
	double Longitude, double Latitude, int E, int P, int T, int Delta_T, double dTHI, double (&PEPs)[8]
	,vector<timTableSet> & rts
	)
{
	double & EP1 = PEPs[0];
	double & EP2 = PEPs[1];
	double & EP3 = PEPs[2];
	double & EP4 = PEPs[3];
	double & EP5 = PEPs[4];
	double & EP6 = PEPs[5];
	double & EP7 = PEPs[6];
	double & EP8 = PEPs[7];

	struct stu1 tempSP;
	double ZxAng, YxAng, cosY, sinZ, cosZ, temp1;
	int Hour2, Minute2, Second2;
	int Hour, Minute, Second;
	int flag = 0;
	for (Hour = 4; Hour <= 20; Hour++)
	{
		for (Minute = 0; Minute < 60; Minute++)
		{
			for (Second = 0; Second < 60; Second = Second + HelioAdjTime)
			{
				Second2 = Second - HelioPreTime;
				Minute2 = Minute;
				Hour2 = Hour;
				if (Second2 >= 60)
				{
					Second2 = Second2 - 60;
					Minute2 = Minute2 + 1;
				}
				if (Minute2 >= 60)
				{
					Minute2 = Minute2 - 60;
					Hour2 = Hour2 + 1;
				}


				tempSP = SolarPos(Year, Month, Day, Hour2, Minute2, Second2, Delta_T, Longitude, Latitude, E, P, T);

				if (tempSP.SAt <= StartAngle)
				{
					flag = 0;
					continue;
				}
				else
				{
					flag = flag + 1;
				}

				cosY = (tempSP.RIz*cos(EP1)*cos(EP2) - tempSP.RIy*sin(EP1)*cos(EP2) + tempSP.RIx*sin(EP2) - pow(sin(EP5), 2)) / pow(cos(EP5), 2);
				YxAng = acos(cosY);

				temp1 = pow((cos(EP5)*sin(EP5) - cos(EP5)*cosY*sin(EP5)), 2) + pow((cos(EP5)*sin(YxAng)), 2);
				sinZ = (tempSP.RIy*cos(EP1) + tempSP.RIz*sin(EP1))*cos(EP5)*sin(YxAng) - (tempSP.RIx*cos(EP2) - tempSP.RIz*cos(EP1)*sin(EP2) + tempSP.RIy*sin(EP1)*sin(EP2))*(cos(EP5)*sin(EP5) - cos(EP5)*cosY*sin(EP5));
				sinZ = sinZ / temp1;
				cosZ = (tempSP.RIx*cos(EP2) - tempSP.RIz*cos(EP1)*sin(EP2) + tempSP.RIy*sin(EP1)*sin(EP2))*cos(EP5)*sin(YxAng) + (tempSP.RIy*cos(EP1) + tempSP.RIz*sin(EP1))*(cos(EP5)*sin(EP5) - cos(EP5)*cosY*sin(EP5));
				cosZ = cosZ / temp1;

				//if (sinZ < 0) * (cosZ < 0) 
				//	ZxAng = 2 * M_PI + atan2(sinZ, cosZ);
				//else
				ZxAng = atan2(sinZ, cosZ);
#if 0
				if (ZxAng < 0) {
					ZxAng = ZxAng + M_PI * 2;
				}
#endif

				YxAng = round(((YxAng*(1 + EP7) - EP4) * 180 / M_PI) * 1000) / 1000;   // 弧度转成角度, 精确到小数点后面三位
				ZxAng = round(((ZxAng*(1 + EP6) - EP3) * 180 / M_PI) * 1000) / 1000;

				Second2 = Second2 + dTHI;
				if (Second2 >= 60)
				{
					Second2 = Second2 - 60;
					Minute2 = Minute2 + 1;
				}
				if (Minute2 >= 60)
				{
					Minute2 = Minute2 - 60;
					Hour2 = Hour2 + 1;
				}
				tempSP = SolarPos(Year, Month, Day, Hour2, Minute2, Second2, Delta_T, Longitude, Latitude, E, P, T);

				try
				{
					rts.push_back(timTableSet(Hour, Minute, Second, ZxAng, YxAng
						, tempSP.RIx, tempSP.RIy, tempSP.RIz));
				}
				catch (std::bad_alloc & bd)
				{
					SN1V2_ERROR_CODE_RET(err_out_of_memory);
				}
			}
		}
	}
	return err_ok;
}

ERR_STA save_timTableSet(char * fdir, int year, int mon, int day, vector<timTableSet> & rts)
{
#if 1
	FILE *sheetfile = fopen(fdir, "wb+");

	if (sheetfile == NULL)
	{
		fprintf(stderr, "%s open error", fdir);
		SN1V2_ERROR_CODE_RET(err_cannot_open_file);
	}
	std::shared_ptr<FILE> Psheet(sheetfile, fclose);
#else
#define sheetfile stdout
#endif
	for (auto & p : rts)
	{
		int i = fprintf(sheetfile, "%04d-%02d-%02d %02d:%02d:%02d,%.3f,%.3f,%.6f,%.6f,%.6f\r\n"
			, year, mon, day
			, p.tm_hour, p.tm_min, p.tm_sec
			, p.ZxAng, p.YxAng
			, p.RIx, p.RIy, p.RIz
			);
		if (i <=0)
		{
			SN1V2_ERROR_CODE_RET(err_cannot_write_file);
		}
	}
	return (err_ok);
}



#endif