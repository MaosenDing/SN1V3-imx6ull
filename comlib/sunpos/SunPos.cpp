#include<stdio.h>
#include<math.h>
#include "Eigen/Dense"  
#include <iostream>
#define PI 3.1415926535897932
using namespace Eigen;

MatrixXd L0_T(64, 4);
MatrixXd L1_T(34, 4);
MatrixXd L2_T(20, 4);
MatrixXd L3_T(7, 4);
MatrixXd L4_T(3, 4);
MatrixXd L5_T(1, 4);

MatrixXd R0_T(40, 4);
MatrixXd R1_T(10, 4);
MatrixXd R2_T(6, 4);
MatrixXd R3_T(2, 4);
MatrixXd R4_T(1, 4);

MatrixXd B0_T(5, 4);
MatrixXd B1_T(2, 4);

MatrixXd TA43(63, 9);


void init()
{
	static int i = 0;
	if (i == 0) {
		i++;
	} else {
		return;
	}


    L0_T << 0, 175347046, 0, 0,
        1, 3341656, 4.6692568, 6283.07585,
        2, 34894, 4.6261, 12566.1517,
        3, 3497, 2.7441, 5753.3849,
        4, 3418, 2.8289, 3.5231,
        5, 3136, 3.6277, 77713.7715,
        6, 2676, 4.4181, 7860.4194,
        7, 2343, 6.1352, 3930.2097,
        8, 1324, 0.7425, 11506.7698,
        9, 1273, 2.0371, 529.691,
        10, 1199, 1.1096, 1577.3435,
        11, 990, 5.233, 5884.927,
        12, 902, 2.045, 26.298,
        13, 857, 3.508, 398.149,
        14, 780, 1.179, 5223.694,
        15, 753, 2.533, 5507.553,
        16, 505, 4.583, 18849.228,
        17, 492, 4.205, 775.523,
        18, 357, 2.92, 0.067,
        19, 317, 5.849, 11790.629,
        20, 284, 1.899, 796.298,
        21, 271, 0.315, 10977.079,
        22, 243, 0.345, 5486.778,
        23, 206, 4.806, 2544.314,
        24, 205, 1.869, 5573.143,
        25, 202, 2.458, 6069.777,
        26, 156, 0.833, 213.299,
        27, 132, 3.411, 2942.463,
        28, 126, 1.083, 20.775,
        29, 115, 0.645, 0.98,
        30, 103, 0.636, 4694.003,
        31, 102, 0.976, 15720.839,
        32, 102, 4.267, 7.114,
        33, 99, 6.21, 2146.17,
        34, 98, 0.68, 155.42,
        35, 86, 5.98, 161000.69,
        36, 85, 1.3, 6275.96,
        37, 85, 3.67, 71430.7,
        38, 80, 1.81, 17260.15,
        39, 79, 3.04, 12036.46,
        40, 75, 1.76, 5088.63,
        41, 74, 3.5, 3154.69,
        42, 74, 4.68, 801.82,
        43, 70, 0.83, 9437.76,
        44, 62, 3.98, 8827.39,
        45, 61, 1.82, 7084.9,
        46, 57, 2.78, 6286.6,
        47, 56, 4.39, 14143.5,
        48, 56, 3.47, 6279.55,
        49, 52, 0.19, 12139.55,
        50, 52, 1.33, 1748.02,
        51, 51, 0.28, 5856.48,
        52, 49, 0.49, 1194.45,
        53, 41, 5.37, 8429.24,
        54, 41, 2.4, 19651.05,
        55, 39, 6.17, 10447.39,
        56, 37, 6.04, 10213.29,
        57, 37, 2.57, 1059.38,
        58, 36, 1.71, 2352.87,
        59, 36, 1.78, 6812.77,
        60, 33, 0.59, 17789.85,
        61, 30, 0.44, 83996.85,
        62, 30, 2.74, 1349.87,
        63, 25, 3.16, 4690.48;

    L1_T <<0, 628331966747, 0, 0 ,
         1, 206059, 2.678235, 6283.07585 ,
         2, 4303, 2.6351, 12566.1517 ,
         3, 425, 1.59, 3.523 ,
         4, 119, 5.796, 26.298 ,
         5, 109, 2.966, 1577.344 ,
         6, 93, 2.59, 18849.23 ,
         7, 72, 1.14, 529.69 ,
         8, 68, 1.87, 398.15 ,
         9, 67, 4.41, 5507.55 ,
         10, 59, 2.89, 5223.69 ,
         11, 56, 2.17, 155.42 ,
         12, 45, 0.4, 796.3 ,
         13, 36, 0.47, 775.52 ,
         14, 29, 2.65, 7.11 ,
         15, 21, 5.34, 0.98 ,
         16, 19, 1.85, 5486.78 ,
         17, 19, 4.97, 213.3 ,
         18, 17, 2.99, 6275.96 ,
         19, 16, 0.03, 2544.31 ,
         20, 16, 1.43, 2146.17 ,
         21, 15, 1.21, 10977.08 ,
         22, 12, 2.83, 1748.02 ,
         23, 12, 3.26, 5088.63 ,
         24, 12, 5.27, 1194.45 ,
         25, 12, 2.08, 4694 ,
         26, 11, 0.77, 553.57 ,
         27, 10, 1.3, 6286.6 ,
         28, 10, 4.24, 1349.87 ,
         29, 9, 2.7, 242.73 ,
         30, 9, 5.64, 951.72 ,
         31, 8, 5.3, 2352.87 ,
         32, 6, 2.65, 9437.76 ,
         33, 6, 4.67, 4690.48 ;

    L2_T << 0, 52919, 0, 0 ,
         1, 8720, 1.0721, 6283.0758 ,
         2, 309, 0.867, 12566.152 ,
         3, 27, 0.05, 3.52 ,
         4, 16, 5.19, 26.3 ,
         5, 16, 3.68, 155.42 ,
         6, 10, 0.76, 18849.23 ,
         7, 9, 2.06, 77713.77 ,
         8, 7, 0.83, 775.52 ,
         9, 5, 4.66, 1577.34 ,
         10, 4, 1.03, 7.11 ,
         11, 4, 3.44, 5573.14 ,
         12, 3, 5.14, 796.3 ,
         13, 3, 6.05, 5507.55 ,
         14, 3, 1.19, 242.73 ,
         15, 3, 6.12, 529.69 ,
         16, 3, 0.31, 398.15 ,
         17, 3, 2.28, 553.57 ,
         18, 2, 4.38, 5223.69 ,
         19, 2, 3.75, 0.98 ;

    L3_T << 0, 289, 5.844, 6283.076 ,
         1, 35, 0, 0 ,
         2, 17, 5.49, 12566.15 ,
         3, 3, 5.2, 155.42 ,
         4, 1, 4.72, 3.52 ,
         5, 1, 5.3, 18849.23 ,
         6, 1, 5.97, 242.73 ;

    L4_T <<0, 114, 3.142, 0 ,
         1, 8, 4.13, 6283.08 ,
         2, 1, 3.84, 12566.15 ;

    L5_T << 0, 1, 3.14, 0;


    B0_T << 0, 280, 3.199, 84334.662 ,
         1, 102, 5.422, 5507.553 ,
         2, 80, 3.88, 5223.69 ,
         3, 44, 3.7, 2352.87 ,
         4, 32, 4, 1577.34 ;

    B1_T <<  0, 9, 3.9, 5507.55 ,
         1, 6, 1.73, 5223.69 ;

    R0_T << 0, 100013989, 0, 0 ,
         1, 1670700, 3.0984635, 6283.07585 ,
         2, 13956, 3.05525, 12566.1517 ,
         3, 3084, 5.1985, 77713.7715 ,
         4, 1628, 1.1739, 5753.3849 ,
         5, 1576, 2.8469, 7860.4194 ,
         6, 925, 5.453, 11506.77 ,
         7, 542, 4.564, 3930.21 ,
         8, 472, 3.661, 5884.927 ,
         9, 346, 0.964, 5507.553 ,
         10, 329, 5.9, 5223.694 ,
         11, 307, 0.299, 5573.143 ,
         12, 243, 4.273, 11790.629 ,
         13, 212, 5.847, 1577.344 ,
         14, 186, 5.022, 10977.079 ,
         15, 175, 3.012, 18849.228 ,
         16, 110, 5.055, 5486.778 ,
         17, 98, 0.89, 6069.78 ,
         18, 86, 5.69, 15720.84 ,
         19, 86, 1.27, 161000.69 ,
         20, 65, 0.27, 17260.15 ,
         21, 63, 0.92, 529.69 ,
         22, 57, 2.01, 83996.85 ,
         23, 56, 5.24, 71430.7 ,
         24, 49, 3.25, 2544.31 ,
         25, 47, 2.58, 775.52 ,
         26, 45, 5.54, 9437.76 ,
         27, 43, 6.01, 6275.96 ,
         28, 39, 5.36, 4694 ,
         29, 38, 2.39, 8827.39 ,
         30, 37, 0.83, 19651.05 ,
         31, 37, 4.9, 12139.55 ,
         32, 36, 1.67, 12036.46 ,
         33, 35, 1.84, 2942.46 ,
         34, 33, 0.24, 7084.9 ,
         35, 32, 0.18, 5088.63 ,
         36, 32, 1.78, 398.15 ,
         37, 28, 1.21, 6286.6 ,
         38, 28, 1.9, 6279.55 ,
         39, 26, 4.59, 10447.39 ;

    R1_T << 0, 103019, 1.10749, 6283.07585 ,
         1, 1721, 1.0644, 12566.1517 ,
         2, 702, 3.142, 0 ,
         3, 32, 1.02, 18849.23 ,
         4, 31, 2.84, 5507.55 ,
         5, 25, 1.32, 5223.69 ,
         6, 18, 1.42, 1577.34 ,
         7, 10, 5.91, 10977.08 ,
         8, 9, 1.42, 6275.96 ,
         9, 9, 0.27, 5486.78 ;


    R2_T << 0, 4359, 5.7846, 6283.0758 ,
         1, 124, 5.579, 12566.152 ,
         2, 12, 3.14, 0 ,
         3, 9, 3.63, 77713.77 ,
         4, 6, 1.87, 5573.14 ,
         5, 3, 5.47, 18849.23 ;

    R3_T <<  0, 145, 4.273, 6283.076 ,
         1, 7, 3.92, 12566.15 ;

    R4_T <<0, 4, 2.56, 6283.08 ;

    TA43 <<0, 0, 0, 0, 1, -171996, -174.2, 92025, 8.9 ,
         -2, 0, 0, 2, 2, -13187, -1.6, 5736, -3.1 ,
         0, 0, 0, 2, 2, -2274, -0.2, 977, -0.5 ,
         0, 0, 0, 0, 2, 2062, 0.2, -895, 0.5 ,
         0, 1, 0, 0, 0, 1426, -3.4, 54, -0.1 ,
         0, 0, 1, 0, 0, 712, 0.1, -7, 0 ,
         -2, 1, 0, 2, 2, -517, 1.2, 224, -0.6 ,
         0, 0, 0, 2, 1, -386, -0.4, 200, 0 ,
         0, 0, 1, 2, 2, -301, 0, 129, -0.1 ,
         -2, -1, 0, 2, 2, 217, -0.5, -95, 0.3 ,
         -2, 0, 1, 0, 0, -158, 0, 0, 0 ,
         -2, 0, 0, 2, 1, 129, 0.1, -70, 0 ,
         0, 0, -1, 2, 2, 123, 0, -53, 0 ,
         2, 0, 0, 0, 0, 63, 0, 0, 0 ,
         0, 0, 1, 0, 1, 63, 0.1, -33, 0 ,
         2, 0, -1, 2, 2, -59, 0, 26, 0 ,
         0, 0, -1, 0, 1, -58, -0.1, 32, 0 ,
         0, 0, 1, 2, 1, -51, 0, 27, 0 ,
         -2, 0, 2, 0, 0, 48, 0, 0, 0 ,
         0, 0, -2, 2, 1, 46, 0, -24, 0 ,
         2, 0, 0, 2, 2, -38, 0, 16, 0 ,
         0, 0, 2, 2, 2, -31, 0, 13, 0 ,
         0, 0, 2, 0, 0, 29, 0, 0, 0 ,
         -2, 0, 1, 2, 2, 29, 0, -12, 0 ,
         0, 0, 0, 2, 0, 26, 0, 0, 0 ,
         -2, 0, 0, 2, 0, -22, 0, 0, 0 ,
         0, 0, -1, 2, 1, 21, 0, -10, 0 ,
         0, 2, 0, 0, 0, 17, -0.1, 0, 0 ,
         2, 0, -1, 0, 1, 16, 0, -8, 0 ,
         -2, 2, 0, 2, 2, -16, 0.1, 7, 0 ,
         0, 1, 0, 0, 1, -15, 0, 9, 0 ,
         -2, 0, 1, 0, 1, -13, 0, 7, 0 ,
         0, -1, 0, 0, 1, -12, 0, 6, 0 ,
         0, 0, 2, -2, 0, 11, 0, 0, 0 ,
         2, 0, -1, 2, 1, -10, 0, 5, 0 ,
         2, 0, 1, 2, 2, -8, 0, 3, 0 ,
         0, 1, 0, 2, 2, 7, 0, -3, 0 ,
         -2, 1, 1, 0, 0, -7, 0, 0, 0 ,
         0, -1, 0, 2, 2, -7, 0, 3, 0 ,
         2, 0, 0, 2, 1, -7, 0, 3, 0 ,
         2, 0, 1, 0, 0, 6, 0, 0, 0 ,
         -2, 0, 2, 2, 2, 6, 0, -3, 0 ,
         -2, 0, 1, 2, 1, 6, 0, -3, 0 ,
         2, 0, -2, 0, 1, -6, 0, 3, 0 ,
         2, 0, 0, 0, 1, -6, 0, 3, 0 ,
         0, -1, 1, 0, 0, 5, 0, 0, 0 ,
         -2, -1, 0, 2, 1, -5, 0, 3, 0 ,
         -2, 0, 0, 0, 1, -5, 0, 3, 0 ,
         0, 0, 2, 2, 1, -5, 0, 3, 0 ,
         -2, 0, 2, 0, 1, 4, 0, 0, 0 ,
         -2, 1, 0, 2, 1, 4, 0, 0, 0 ,
         0, 0, 1, -2, 0, 4, 0, 0, 0 ,
         -1, 0, 1, 0, 0, -4, 0, 0, 0 ,
         -2, 1, 0, 0, 0, -4, 0, 0, 0 ,
         1, 0, 0, 0, 0, -4, 0, 0, 0 ,
         0, 0, 1, 2, 0, 3, 0, 0, 0 ,
         0, 0, -2, 2, 2, -3, 0, 0, 0 ,
         -1, -1, 1, 0, 0, -3, 0, 0, 0 ,
         0, 1, 1, 0, 0, -3, 0, 0, 0 ,
         0, -1, 1, 2, 2, -3, 0, 0, 0 ,
         2, -1, -1, 2, 2, -3, 0, 0, 0 ,
         0, 0, 3, 2, 2, -3, 0, 0, 0 ,
         2, -1, 0, 2, 2, -3, 0, 0, 0;

}
/*
符号函数
*/
double sign(double num)
{
    if (num>0){ return 1;}
    if (num<0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

void SP(double Year,double Month, double Day, double Hour, double Minute, double Second,double Delta_T,double  longitude,double latitude, double E,double P,double T, Vector2f & ret)
{
    init();

    double TimeZone = sign(longitude) * (floor((abs(longitude) - 7.5) / 15) + 1);
    double Y = 0;
    double M = 0;

    if (Month>2)
    {
        Y = Year;
        M = Month;
    }
    else
    {
        Y = Year-1;
        M = Month+12;
    }

    double D = Day + (Hour - TimeZone + Minute / 60 + Second / 3600) / 24;
    double A = double(int(Y/100));

    double  B = 0;
    if ((Y<1582)||(Y ==1582 && M<10)||((Y==1582) && (M ==10) &&( D<4)))
    {
        B = 0;
    }
    else
    {
        B = 2.0 - A + double(int(A / 4));
    }
    double JD = double(int(365.25 * (double(Y) + 4716))) + double(int(30.6001 * (double(M) + 1))) + D + B - 1524.5;
    double JDE = JD + Delta_T / 86400; 
    double JC = (JD - 2451545) / 36525; 
    double JCE = (JDE - 2451545) / 36525; 
    double JME = JCE / 10;
    /*
    MatrixXd a = L0_T.block(0, 1, L0_T.rows(), 1).array();
    
    MatrixXd b = L0_T.block(0, 2, L0_T.rows(), 1).array() + JME * L0_T.block(0, 3, L0_T.rows(), 1).array();
 
    MatrixXd c = b.array().cos();

    MatrixXd d = a.array() * c.array();
    std::cout << d<< std::endl;
    double ee = d.sum();
    std::cout << ee << std::endl;
    */
    double L0 = (L0_T.block(0, 1, L0_T.rows(), 1).array() * ((L0_T.block(0, 2, L0_T.rows(), 1).array() + JME * L0_T.block(0, 3, L0_T.rows(), 1).array()).array().cos()).array()).sum();
    double L1 = (L1_T.block(0, 1, L1_T.rows(), 1).array() * ((L1_T.block(0, 2, L1_T.rows(), 1).array() + JME * L1_T.block(0, 3, L1_T.rows(), 1).array()).array().cos()).array()).sum();
    double L2 = (L2_T.block(0, 1, L2_T.rows(), 1).array() * ((L2_T.block(0, 2, L2_T.rows(), 1).array() + JME * L2_T.block(0, 3, L2_T.rows(), 1).array()).array().cos()).array()).sum();
    double L3 = (L3_T.block(0, 1, L3_T.rows(), 1).array() * ((L3_T.block(0, 2, L3_T.rows(), 1).array() + JME * L3_T.block(0, 3, L3_T.rows(), 1).array()).array().cos()).array()).sum();
    double L4 = (L4_T.block(0, 1, L4_T.rows(), 1).array() * ((L4_T.block(0, 2, L4_T.rows(), 1).array() + JME * L4_T.block(0, 3, L4_T.rows(), 1).array()).array().cos()).array()).sum();
    double L5 = (L5_T.block(0, 1, L5_T.rows(), 1).array() * ((L5_T.block(0, 2, L5_T.rows(), 1).array() + JME * L5_T.block(0, 3, L5_T.rows(), 1).array()).array().cos()).array()).sum();
    
    double L_in_Radians = (L0 + L1 * JME + L2 *pow(JME ,2) + L3 * pow(JME, 3) + L4 * pow(JME, 4) + L5 * pow(JME, 5)) / pow(10, 8);
    double L_in_Degrees = L_in_Radians * 180 / PI;
    double L = fmod(L_in_Degrees,360);

    double B0 = (B0_T.block(0, 1, B0_T.rows(), 1).array() * ((B0_T.block(0, 2, B0_T.rows(), 1).array() + JME * B0_T.block(0, 3, B0_T.rows(), 1).array()).array().cos()).array()).sum();
    double B1 = (B1_T.block(0, 1, B1_T.rows(), 1).array() * ((B1_T.block(0, 2, B1_T.rows(), 1).array() + JME * B1_T.block(0, 3, B1_T.rows(), 1).array()).array().cos()).array()).sum();
    double B2 = 0;
    double B3 = 0;
    double B4 = 0;
    double B5 = 0;
    double B_in_Radians = (B0 + B1 * JME + B2 * pow(JME, 2) + B3 * pow(JME, 3) + B4 * pow(JME, 4) + B5 * pow(JME, 5)) / pow(10, 8);
    B = B_in_Radians * 180 / PI;

    double R0 = (R0_T.block(0, 1, R0_T.rows(), 1).array() * ((R0_T.block(0, 2, R0_T.rows(), 1).array() + JME * R0_T.block(0, 3, R0_T.rows(), 1).array()).array().cos()).array()).sum();
    double R1 = (R1_T.block(0, 1, R1_T.rows(), 1).array() * ((R1_T.block(0, 2, R1_T.rows(), 1).array() + JME * R1_T.block(0, 3, R1_T.rows(), 1).array()).array().cos()).array()).sum();
    double R2 = (R2_T.block(0, 1, R2_T.rows(), 1).array() * ((R2_T.block(0, 2, R2_T.rows(), 1).array() + JME * R2_T.block(0, 3, R2_T.rows(), 1).array()).array().cos()).array()).sum();
    double R3 = (R3_T.block(0, 1, R3_T.rows(), 1).array() * ((R3_T.block(0, 2, R3_T.rows(), 1).array() + JME * R3_T.block(0, 3, R3_T.rows(), 1).array()).array().cos()).array()).sum();
    double R4 = (R4_T.block(0, 1, R4_T.rows(), 1).array() * ((R4_T.block(0, 2, R4_T.rows(), 1).array() + JME * R4_T.block(0, 3, R4_T.rows(), 1).array()).array().cos()).array()).sum();
    double R5 = 0;
    double R = (R0 + R1 * JME + R2 * pow(JME, 2) + R3 * pow(JME, 3) + R4 * pow(JME, 4) + R5 * pow(JME, 5)) / pow(10, 8);

    double THETA = fmod(L + 180, 360);
    double beta = -B;

    double X0 = 297.85036 + 445267.111480 * JCE - 0.0019142 * pow(JCE, 2) + pow(JCE, 3) / 189474;
    double X1 = 357.52772 + 35999.050340 * JCE - 0.0001603 * pow(JCE, 2) - pow(JCE, 3) / 300000;
    double X2 = 134.96298 + 477198.867398 * JCE + 0.0086972 * pow(JCE, 2) + pow(JCE, 3) / 56250;
    double X3 = 93.27191 + 483202.017538 * JCE - 0.0036825 * pow(JCE, 2) + pow(JCE, 3) / 327270;
    double X4 = 125.04452 - 1934.136261 * JCE + 0.0020708 * pow(JCE, 2) + pow(JCE, 3) / 450000;

    MatrixXd temp0 = ((X0 * TA43.block(0, 0, TA43.rows(), 1) + X1 * TA43.block(0, 1, TA43.rows(), 1) + X2 * TA43.block(0, 2, TA43.rows(), 1) + X3 * TA43.block(0, 3, TA43.rows(), 1) + X4 * TA43.block(0, 4, TA43.rows(), 1)) * PI / 180).array().sin();
    MatrixXd temp1 = TA43.block(0, 5, TA43.rows(), 1).array() + JCE * TA43.block(0, 6, TA43.rows(), 1).array();
    double delta_psi = (temp1.array() * temp0.array()).array().sum()/36000000;


    MatrixXd temp2 = ((X0 * TA43.block(0, 0, TA43.rows(), 1) + X1 * TA43.block(0,1, TA43.rows(), 1) + X2 * TA43.block(0, 2, TA43.rows(), 1) + X3 * TA43.block(0, 3, TA43.rows(), 1) + X4 * TA43.block(0, 4, TA43.rows(), 1)) * PI / 180).array().cos();
    MatrixXd temp3 = TA43.block(0, 7, TA43.rows(), 1).array() + JCE * TA43.block(0, 8, TA43.rows(), 1).array();
    double delta_epsilon = (temp3.array() * temp2.array()).array().sum() / 36000000;

    double U = JME / 10; 
    double epsilon0 = 84381.448 - 4680.93 * U - 1.55 * pow(U,2) + 1999.25 * pow(U, 3) - 51.38 * pow(U, 4) - 249.67 * pow(U, 5) - 39.05 * pow(U, 6) + 7.12 * pow(U, 7) + 27.87 * pow(U, 8) + 5.79 * pow(U, 9) + 2.45 * pow(U, 10);
    double epsilon = epsilon0 / 3600 + delta_epsilon;
    double delta_tau = -20.4898 / (3600 * R); 

    double lamba = THETA + delta_psi + delta_tau;

    double upsilon0 = 280.46061837 + 360.98564736629 * (JD - 2451545) + 0.000387933 *pow(JC,2)- pow(JC,3) / 38710000;
    upsilon0 = fmod(upsilon0, 360);
    double upsilon = upsilon0 + delta_psi * cos(epsilon *PI / 180);  

    double alpha_in_Radians = atan2((sin(lamba * PI/ 180) * cos(epsilon * PI / 180) - tan(beta * PI / 180) * sin(epsilon * PI / 180)), cos(lamba * PI / 180));
    double alpha_in_Degrees = alpha_in_Radians * 180 / PI;
    double alpha = fmod(alpha_in_Degrees, 360);

    double delta_in_Radians = asin(sin(beta * PI / 180) * cos(epsilon * PI / 180) + cos(beta * PI / 180) * sin(epsilon * PI / 180) * sin(lamba * PI / 180));
    double delta = delta_in_Radians * 180 / PI;
    double H = upsilon + longitude - alpha;
    H = fmod(H, 360);

    double ksi = 8.794 / (3600 * R);
    double mju = atan(0.99664719 * tan(latitude * PI/ 180));
    double  x =cos(mju) + E / 6378140 * cos(latitude * PI / 180);
    double y = 0.99664719 * sin(mju) + E / 6378140 * sin(latitude * PI / 180);
    double delta_alpha_in_radians = atan2(-x * sin(ksi * PI / 180) * sin(H * PI/ 180), (cos(delta * PI/ 180) - x * sin(ksi * PI/ 180) * cos(H * PI / 180)));
    double  delta_alpha = delta_alpha_in_radians * 180 / PI;

    double  H_prime = H - delta_alpha;
    double  delta_prime = atan2((sin(delta * PI/ 180) - y * sin(ksi * PI / 180)) * cos(delta_alpha *PI/ 180), (cos(delta * PI / 180) - x * sin(ksi * PI/ 180) * cos(H * PI/ 180))) * 180 / PI;

    double e0_in_Radians = asin(sin(latitude * PI/ 180) * sin(delta_prime * PI/ 180) + cos(latitude * PI/ 180) * cos(delta_prime * PI / 180) * cos(H_prime * PI/ 180)); 
    double e0 = e0_in_Radians * 180 /PI;

    double sun_radius = 0.26667;
    double atmos_refract = 0.5667;

    double delta_e = 0;
    if (e0 >= -1 * (sun_radius + atmos_refract))
    {
        delta_e = P / 1010 * 283 / (273 + T) * 1.02 / (60 * tan((e0 + 10.3 / (e0 + 5.11)) * PI / 180));
    }
    else
    {
        delta_e = 0;
    }

   double e = e0 + delta_e; 
   double theta = 90 - e;   
   double TAU = atan2(sin(H_prime * PI / 180), (cos(H_prime * PI/ 180) * sin(latitude * PI / 180) - tan(delta_prime * PI / 180) * cos(latitude * PI / 180)));
   TAU = fmod(TAU * 180 / PI, 360);  
   double PHI = fmod(TAU + 180, 360); 

   double At = 90 - theta;
   double Az = PHI;

   Vector2f angle(At, Az);

   ret = angle;
}

