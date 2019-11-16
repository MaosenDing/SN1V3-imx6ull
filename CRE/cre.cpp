#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <vector>
using namespace std;
char ER[200];

struct ppp{
	double p[5];
	double & operator[] (int index) { return p[index]; }
};


extern "C" char *CRE(char ResDir[], double SPS, double SFL, double PEP[8])
{
	memset(ER, 0, sizeof(ER));
	FILE *ResFile;
	if (NULL == (ResFile = fopen(ResDir, "r"))) {
		sprintf(ER, "%d", -1);
		return ER;
	}//出错处理

	vector<ppp> ResData;
	ResData.resize(4000);

	vector<int> SS;
	SS.resize(4000);

	double DH, DL, RI[3], AYR, AZR, AZR2, TMR[3], TMN[3], AY, AZ, SL ,SR;
	double SSmax = 0;
	int dnum = 0, SumSS = 0, i, j, k;

	// 读取偏差数据
	char tempR[20];
	while (!feof(ResFile)) {
		fscanf(ResFile, "%[^,]%*c", tempR);		// 时间，跳过
		fscanf(ResFile, "%[^,]%*c", tempR);		// 行偏差
		DH = atof(tempR);
		fscanf(ResFile, "%[^,]%*c", tempR);		// 列偏差
		DL = atof(tempR);
		fscanf(ResFile, "%[^,]%*c", tempR);		// 单个面积
		SS[dnum] = atof(tempR);
		SumSS = SumSS + SS[dnum];				// 总面积，求平均值用
		fscanf(ResFile, "%[^,]%*c", tempR);		// 方位轴转角，弧度
		ResData[dnum][0] = atof(tempR) / 180 * M_PI;
		fscanf(ResFile, "%[^,]%*c", tempR);		// 俯仰轴转角，弧度
		ResData[dnum][1] = atof(tempR) / 180 * M_PI;
		fscanf(ResFile, "%[^,]%*c", tempR);		// 入射矢量，rx
		RI[0] = atof(tempR);
		fscanf(ResFile, "%[^,]%*c", tempR);		// 入射矢量，ry
		RI[1] = atof(tempR);
		fscanf(ResFile, "%[^\n]%*c", tempR);		// 入射矢量，rz
		RI[2] = atof(tempR);

		if ((RI[0] == RI[1]) && (RI[0] == RI[2]) && (RI[2] == RI[1]))
			continue;

		TMR[0] = DH * SPS; 		// 实际法线,nx
		TMR[1] = -DL * SPS; 		// 实际法线,ny
		TMR[2] = SFL;			// 实际法线,nz
		TMR[0] = TMR[0] / sqrt(pow(DH * SPS, 2) + pow(DL * SPS, 2) + pow(SFL, 2));	// 归一化
		TMR[1] = TMR[1] / sqrt(pow(DH * SPS, 2) + pow(DL * SPS, 2) + pow(SFL, 2));
		TMR[2] = TMR[2] / sqrt(pow(DH * SPS, 2) + pow(DL * SPS, 2) + pow(SFL, 2));

		// 计算俯仰角
		if (TMR[0] == 0)
			AYR = M_PI / 2 - asin(RI[2] / sqrt(1 - pow(TMR[1], 2)));
		else
			AYR = M_PI / 2 - asin(RI[2] / sqrt(1 - pow(TMR[1], 2))) - TMR[0] / fabs(TMR[0])*acos(TMR[2] / sqrt(1 - pow(TMR[1], 2)));
		// 绕俯仰轴旋转
		TMR[0] = TMR[0] * cos(AYR) + TMR[2] * sin(AYR);
		TMR[2] = TMR[2] * cos(AYR) - TMR[0] * sin(AYR);

		TMN[0] = sin(AYR);
		TMN[1] = 0;
		TMN[2] = cos(AYR);

		// 计算方位角
		AZR = atan2(RI[1], RI[0]) - atan2(TMR[1], TMR[0]);
		// 绕方位轴旋转
		// 由图像算得的实际光轴
		ResData[dnum][2] = TMN[0] * cos(AZR) - TMN[1] * sin(AZR);		// Ix
		ResData[dnum][3] = TMN[1] * cos(AZR) + TMN[0] * sin(AZR);		// Iy
		ResData[dnum][4] = TMN[2];										// Iz
		dnum = dnum + 1;
	}
	fclose(ResFile);


	// 面积数据筛选
	TMR[0] = (double)SumSS / dnum;	// 面积平均值
	DL = 0;
	for (i = 0; i < dnum; i = i + 1) {
		if (SSmax < (double)SS[i])
			SSmax = (double)SS[i];
		TMR[1] = pow((double)SS[i] - TMR[0], 2) + TMR[1];
	}
	if (SSmax == TMR[0]) {
		SL = TMR[0] - 5;
		SR = TMR[0] + 5;
	} else {
		TMR[2] = sqrt(TMR[1] / (dnum - 1));	// 面积标准差
		SL = TMR[0] - TMR[2] * 4;		// 面积下限	
		SR = TMR[0] + TMR[2] * 4;		// 面积下限	
	}




	double H11 = 0, H12 = 0, H13 = 0, H14 = 0, H15 = 0, H16 = 0, H17 = 0, H18 = 0, H21 = 0, H22 = 0, H23 = 0, H24 = 0, H25 = 0, H26 = 0, H27 = 0, H28 = 0;
	double SumH[8], SumH2[8][8];

	for (SumSS = 0; SumSS <= 10; SumSS++) {
		memset(SumH, 0, sizeof(SumH));
		memset(SumH2, 0, sizeof(SumH2));

		//FILE *sheetfile = fopen("D://SN//SN1test//sr.txt", "wb+");
		// 面积筛选
		for (i = 0; i < dnum; i = i + 1) {

			if ((SS[i] >= SL )&&(SS[i] <= SR))	// 面积太小的不计算
			{

				// 代入运动模型
				AY = (ResData[i][1] + PEP[3]) / (1 + PEP[6]);
				AZ = (ResData[i][0] + PEP[2]) / (1 + PEP[5]);

				TMR[0] = sin(PEP[1])*(cos(AY)*cos(PEP[4])*cos(PEP[7]) - sin(PEP[4])*sin(PEP[7])) + cos(PEP[1])*(cos(AZ)*cos(PEP[7])*sin(AY) - sin(AZ)*(-cos(AY)*cos(PEP[7])*sin(PEP[4]) - cos(PEP[4])*sin(PEP[7])));
				TMR[1] = cos(PEP[0])*(cos(PEP[7])*sin(AY)*sin(AZ) + cos(AZ)*(-cos(AY)*cos(PEP[7])*sin(PEP[4]) - cos(PEP[4])*sin(PEP[7]))) - sin(PEP[0])*(cos(PEP[1])*(cos(AY)*cos(PEP[4])*cos(PEP[7]) - sin(PEP[4])*sin(PEP[7])) - sin(PEP[1])*(cos(AZ)*cos(PEP[7])*sin(AY) - sin(AZ)*(-cos(AY)*cos(PEP[7])*sin(PEP[4]) - cos(PEP[4])*sin(PEP[7]))));
				TMR[2] = sin(PEP[0])*(cos(PEP[7])*sin(AY)*sin(AZ) + cos(AZ)*(-cos(AY)*cos(PEP[7])*sin(PEP[4]) - cos(PEP[4])*sin(PEP[7]))) + cos(PEP[0])*(cos(PEP[1])*(cos(AY)*cos(PEP[4])*cos(PEP[7]) - sin(PEP[4])*sin(PEP[7])) - sin(PEP[1])*(cos(AZ)*cos(PEP[7])*sin(AY) - sin(AZ)*(-cos(AY)*cos(PEP[7])*sin(PEP[4]) - cos(PEP[4])*sin(PEP[7]))));

				//fprintf(sheetfile, "%.6f", TMR[0]);
				//fprintf(sheetfile, "%s", ",");
				//fprintf(sheetfile, "%.6f", TMR[1]);
				//fprintf(sheetfile, "%s", ",");
				//fprintf(sheetfile, "%.6f", TMR[2]);
				//fprintf(sheetfile, "%s", ",");

				// 偏差角计算
				AYR = acos(TMR[2]) - acos(ResData[i][4]);		// 俯仰偏差角

				if ((ResData[i][2] >= 0) && (ResData[i][3] >= 0))
					AZR = atan2(fabs(ResData[i][3]), fabs(ResData[i][2]));
				else if ((ResData[i][2] < 0) && (ResData[i][3] >= 0))
					AZR = M_PI - atan2(fabs(ResData[i][3]), fabs(ResData[i][2]));
				else if ((ResData[i][2] >= 0) && (ResData[i][3] < 0))
					AZR = -atan2(fabs(ResData[i][3]), fabs(ResData[i][2]));
				else
					AZR = -M_PI + atan2(fabs(ResData[i][3]), fabs(ResData[i][2]));

				if ((TMR[0] >= 0) && (TMR[1] >= 0))
					AZR2 = atan2(fabs(TMR[1]), fabs(TMR[0]));
				else if ((TMR[0] < 0) && (TMR[1] >= 0))
					AZR2 = M_PI - atan2(fabs(TMR[1]), fabs(TMR[0]));
				else if ((TMR[0] >= 0) && (TMR[1] < 0))
					AZR2 = -atan2(fabs(TMR[1]), fabs(TMR[0]));
				else
					AZR2 = -M_PI + atan2(fabs(TMR[1]), fabs(TMR[0]));
				AZR = AZR2 - AZR;		// 方位偏差角


				//fprintf(sheetfile, "%.6f", AZR);
				//fprintf(sheetfile, "%s", ",");
				//fprintf(sheetfile, "%.6f", AYR);
				//fprintf(sheetfile, "%s", ",");
				//fprintf(sheetfile, "%s", "\r\n");


				if (fabs(AZR) > 0.1)
					continue;



				H11 = sin(AZ);
				H12 = -cos(AZ);
				H13 = 0.0;
				H14 = -1.0;
				H15 = 0.0;
				H16 = 0.0;
				H17 = AY;
				H18 = 0.0;
				H21 = cos(AZ) / tan(AY);
				H22 = sin(AZ) / tan(AY);
				H23 = -1.0;
				H24 = 0.0;
				H25 = (cos(AY)) / sin(AY);
				H26 = AZ;
				H27 = 0.0;
				H28 = 1 / sin(AY);

				SumH[0] = SumH[0] + AYR * H11 + AZR * H21;
				SumH[1] = SumH[1] + AYR * H12 + AZR * H22;
				SumH[2] = SumH[2] + AYR * H13 + AZR * H23;
				SumH[3] = SumH[3] + AYR * H14 + AZR * H24;
				SumH[4] = SumH[4] + AYR * H15 + AZR * H25;
				SumH[5] = SumH[5] + AYR * H16 + AZR * H26;
				SumH[6] = SumH[6] + AYR * H17 + AZR * H27;
				SumH[7] = SumH[7] + AYR * H18 + AZR * H28;

				SumH2[0][0] = SumH2[0][0] + H11 * H11 + H21 * H21;
				SumH2[0][1] = SumH2[0][1] + H11 * H12 + H21 * H22;
				SumH2[0][2] = SumH2[0][2] + H11 * H13 + H21 * H23;
				SumH2[0][3] = SumH2[0][3] + H11 * H14 + H21 * H24;
				SumH2[0][4] = SumH2[0][4] + H11 * H15 + H21 * H25;
				SumH2[0][5] = SumH2[0][5] + H11 * H16 + H21 * H26;
				SumH2[0][6] = SumH2[0][6] + H11 * H17 + H21 * H27;
				SumH2[0][7] = SumH2[0][7] + H11 * H18 + H21 * H28;

				SumH2[1][0] = SumH2[1][0] + H12 * H11 + H22 * H21;
				SumH2[1][1] = SumH2[1][1] + H12 * H12 + H22 * H22;
				SumH2[1][2] = SumH2[1][2] + H12 * H13 + H22 * H23;
				SumH2[1][3] = SumH2[1][3] + H12 * H14 + H22 * H24;
				SumH2[1][4] = SumH2[1][4] + H12 * H15 + H22 * H25;
				SumH2[1][5] = SumH2[1][5] + H12 * H16 + H22 * H26;
				SumH2[1][6] = SumH2[1][6] + H12 * H17 + H22 * H27;
				SumH2[1][7] = SumH2[1][7] + H12 * H18 + H22 * H28;

				SumH2[2][0] = SumH2[2][0] + H13 * H11 + H23 * H21;
				SumH2[2][1] = SumH2[2][1] + H13 * H12 + H23 * H22;
				SumH2[2][2] = SumH2[2][2] + H13 * H13 + H23 * H23;
				SumH2[2][3] = SumH2[2][3] + H13 * H14 + H23 * H24;
				SumH2[2][4] = SumH2[2][4] + H13 * H15 + H23 * H25;
				SumH2[2][5] = SumH2[2][5] + H13 * H16 + H23 * H26;
				SumH2[2][6] = SumH2[2][6] + H13 * H17 + H23 * H27;
				SumH2[2][7] = SumH2[2][7] + H13 * H18 + H23 * H28;

				SumH2[3][0] = SumH2[3][0] + H14 * H11 + H24 * H21;
				SumH2[3][1] = SumH2[3][1] + H14 * H12 + H24 * H22;
				SumH2[3][2] = SumH2[3][2] + H14 * H13 + H24 * H23;
				SumH2[3][3] = SumH2[3][3] + H14 * H14 + H24 * H24;
				SumH2[3][4] = SumH2[3][4] + H14 * H15 + H24 * H25;
				SumH2[3][5] = SumH2[3][5] + H14 * H16 + H24 * H26;
				SumH2[3][6] = SumH2[3][6] + H14 * H17 + H24 * H27;
				SumH2[3][7] = SumH2[3][7] + H14 * H18 + H24 * H28;

				SumH2[4][0] = SumH2[4][0] + H15 * H11 + H25 * H21;
				SumH2[4][1] = SumH2[4][1] + H15 * H12 + H25 * H22;
				SumH2[4][2] = SumH2[4][2] + H15 * H13 + H25 * H23;
				SumH2[4][3] = SumH2[4][3] + H15 * H14 + H25 * H24;
				SumH2[4][4] = SumH2[4][4] + H15 * H15 + H25 * H25;
				SumH2[4][5] = SumH2[4][5] + H15 * H16 + H25 * H26;
				SumH2[4][6] = SumH2[4][6] + H15 * H17 + H25 * H27;
				SumH2[4][7] = SumH2[4][7] + H15 * H18 + H25 * H28;

				SumH2[5][0] = SumH2[5][0] + H16 * H11 + H26 * H21;
				SumH2[5][1] = SumH2[5][1] + H16 * H12 + H26 * H22;
				SumH2[5][2] = SumH2[5][2] + H16 * H13 + H26 * H23;
				SumH2[5][3] = SumH2[5][3] + H16 * H14 + H26 * H24;
				SumH2[5][4] = SumH2[5][4] + H16 * H15 + H26 * H25;
				SumH2[5][5] = SumH2[5][5] + H16 * H16 + H26 * H26;
				SumH2[5][6] = SumH2[5][6] + H16 * H17 + H26 * H27;
				SumH2[5][7] = SumH2[5][7] + H16 * H18 + H26 * H28;

				SumH2[6][0] = SumH2[6][0] + H17 * H11 + H27 * H21;
				SumH2[6][1] = SumH2[6][1] + H17 * H12 + H27 * H22;
				SumH2[6][2] = SumH2[6][2] + H17 * H13 + H27 * H23;
				SumH2[6][3] = SumH2[6][3] + H17 * H14 + H27 * H24;
				SumH2[6][4] = SumH2[6][4] + H17 * H15 + H27 * H25;
				SumH2[6][5] = SumH2[6][5] + H17 * H16 + H27 * H26;
				SumH2[6][6] = SumH2[6][6] + H17 * H17 + H27 * H27;
				SumH2[6][7] = SumH2[6][7] + H17 * H18 + H27 * H28;

				SumH2[7][0] = SumH2[7][0] + H18 * H11 + H28 * H21;
				SumH2[7][1] = SumH2[7][1] + H18 * H12 + H28 * H22;
				SumH2[7][2] = SumH2[7][2] + H18 * H13 + H28 * H23;
				SumH2[7][3] = SumH2[7][3] + H18 * H14 + H28 * H24;
				SumH2[7][4] = SumH2[7][4] + H18 * H15 + H28 * H25;
				SumH2[7][5] = SumH2[7][5] + H18 * H16 + H28 * H26;
				SumH2[7][6] = SumH2[7][6] + H18 * H17 + H28 * H27;
				SumH2[7][7] = SumH2[7][7] + H18 * H18 + H28 * H28;
			}
		}

		//fclose(sheetfile);

		// 矩阵求逆
		int is[8], js[8];
		for (k = 0; k <= 7; k++) {
			DH = 0;
			//从第k行，第k列开始，找出绝对值最大的项
			for (i = k; i <= 7; i++) {
				for (j = k; j <= 7; j++) {
					DL = fabs(SumH2[i][j]);
					if (DL > DH) {
						DH = DL;
						is[k] = i;
						js[k] = j;
					}
				}
			}

			//若当前绝对值最大的项不在第K行，则将最大值所在行的元素与第K行的元素进行对调
			if (is[k] != k) {
				for (j = 0; j <= 7; j++) {
					DL = SumH2[k][j];
					SumH2[k][j] = SumH2[is[k]][j];
					SumH2[is[k]][j] = DL;
				}
			}
			//若当前绝对值最大的项不在第K列，则将最大值所在列的元素与第K列的元素进行对调
			if (js[k] != k) {
				for (i = 0; i <= 7; i++) {
					DL = SumH2[i][k];
					SumH2[i][k] = SumH2[i][js[k]];
					SumH2[i][js[k]] = DL;
				}
			}

			//将交换后的第K行归一化(第K行所有元素分别除以当前行的最大值
			SumH2[k][k] = 1.0 / SumH2[k][k];

			for (j = 0; j <= 7; j++) {
				if (j != k) {
					SumH2[k][j] = SumH2[k][j] * SumH2[k][k];
				}
			}

			//第j列中，第(k+1)行以下(包括第(k+1)行)所有元素都减去最大值
			for (i = 0; i <= 7; i++) {
				if (i != k) {
					for (j = 0; j <= 7; j++) {
						if (j != k) {
							SumH2[i][j] -= SumH2[i][k] * SumH2[k][j];
						}
					}
				}

			}
			for (i = 0; i <= 7; i++) {
				if (i != k) {
					SumH2[i][k] = -SumH2[i][k] * SumH2[k][k];
				}
			}
		}
		for (k = 7; k >= 0; k--) {
			if (js[k] != k) {
				for (j = 0; j <= 7; j++) {
					DL = SumH2[k][j];
					SumH2[k][j] = SumH2[js[k]][j];
					SumH2[js[k]][j] = DL;
				}
			}

			if (is[k] != k) {
				for (i = 0; i <= 7; i++) {
					DL = SumH2[i][k];
					SumH2[i][k] = SumH2[i][is[k]];
					SumH2[i][is[k]] = DL;
				}
			}
		}

		// 求校正参数
		for (i = 0; i < 8; i++) {
			DH = 0;
			for (j = 0; j < 8; j++) {
				DH = SumH2[i][j] * SumH[j] + DH;
			}
			//printf("%13.8f", PEP[i]);
			//printf(",");
			PEP[i] = DH + PEP[i];
			//printf("%13.8f",PEP[i]);
			//printf("\n");
		}
	}

	// 输出成字符串
	for (i = 0; i < 7; i++) {
		sprintf(tempR, "%f,", PEP[i]);
		strcat(ER, tempR);
	}
	sprintf(tempR, "%f", PEP[7]);
	strcat(ER, tempR);
	return ER;
}

/*
void main()
{
	char rdir[50] = "D://SN//SN1test//SD1.csv";
	double SPS = 1.4e-6;
	double SFL = 6e-3;
	double PEP[8] = { 0, 0, 0, 0, 0, 0, 0 ,0};
	char *EP = CRE(rdir, SPS, SFL, PEP);
}
*/