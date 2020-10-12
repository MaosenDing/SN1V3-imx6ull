#include<stdio.h>
#include<math.h>
#include <eigen3/Eigen/Dense>  
#include <iostream>
#include "timeTableV2.h"
#include <chrono>
#include <memory>
#include "SunPosTable.h"
#define PI 3.1415926535
using namespace Eigen;
using namespace std;
void SP(double Year, double Month, double Day, double Hour, double Minute, double Second, double Delta_T, double  longitude, double latitude, double E, double P, double T,Vector2f & ret);


Vector3f calSunRayIn(Vector2f & sunAngle,Vector3f & helioPoint,Vector3f & aimPoint,float T, Vector2f & Helio_angle)
{
/*	
# 函数：计算太阳的三维入射矢量
# 输入： （1）太阳高度角和方位角：sunAngle=[高度角，方位角]，1行3列，单位角度；
#       （2）定日镜坐标XYZ：helioPoint=[x,y,z],1行3列，单位米；
#       （3）指向点坐标XYZ：aimPoint=[x,y,z],1行3列，单位米；
# 输出: 入射矢量XYZ:[x,y,z]，1行3列
# 备注: 单循环周期时间为 1s,太阳高度角大于 5°
# 报错：输出错误[null,null,null]，表示太阳高度角小于T°
*/
	if (sunAngle[0]<=T)
	{
		Vector3f Rayin_h_ZR(0, 0, 0);
		return Rayin_h_ZR;
	}

	sunAngle = sunAngle * PI / 180;

	Vector3f Rayin_f(-cos(sunAngle[0])*cos(sunAngle[1]),cos(sunAngle[0])*sin(sunAngle[1]),sin(sunAngle[0]));

	Vector3f Rayout_f_ZR = (aimPoint - helioPoint) / ((aimPoint - helioPoint).norm());
	Vector3f Normal_f_ZR = (Rayin_f + Rayout_f_ZR) / ((Rayin_f + Rayout_f_ZR).norm());

	float Az_ZR = atan2(Normal_f_ZR[1], Normal_f_ZR[0]);
	float At_ZR = PI / 2 - asin(Normal_f_ZR[2]);


	float helio_Az = Az_ZR / PI * 180; //方位角，角度
	float helio_At = At_ZR / PI * 180; //俯仰角，角度
	Helio_angle << helio_Az, helio_At;//方位角，俯仰角



	Matrix3f Ry_ZR;
	Ry_ZR << cos(At_ZR), 0, sin(At_ZR), 0, 1, 0, -sin(At_ZR), 0, cos(At_ZR);

	Matrix3f Rz_ZR;
	Rz_ZR << cos(Az_ZR), -sin(Az_ZR), 0, sin(Az_ZR), cos(Az_ZR), 0, 0, 0, 1;

	Matrix3f T_g2h_ZR = Rz_ZR * Ry_ZR;
	Vector3f Rayin_h_ZR = T_g2h_ZR.transpose()* Rayin_f;
	Rayin_h_ZR = Rayin_h_ZR.transpose();

	return Rayin_h_ZR;
}

Vector4f quatinv(Vector4f vec)
{
	/*
#    函数：求取四元数的逆
#    输入：四元数
#    输出：四元数的逆
	*/
	vec = vec / vec.norm();
	vec[1] = -vec[1];
	vec[2] = -vec[2];
	vec[3] = -vec[3];

	return vec;
}

Vector4f quatmultiply(Vector4f q1, Vector4f	q2)
{
	/*
#    函数：求取四元数的乘积
#    输入：两个四元数
#    输出：两个四元数的乘积
	*/
	float r1 = q1[0];
	float r2 = q2[0];

	Vector3f v1(q1[1], q1[2], q1[3]);
	Vector3f v2(q2[1], q2[2], q2[3]);

	float r = r1 * r2 - v1.transpose() * v2;
	Vector3f v = r1 * v2 + r2 * v1 + v1.cross(v2);
	Vector4f vec(r,v[0],v[1],v[2]);
	return vec;
}

Vector3f vectorRot3D(Vector3f rotAxis,float rotAng, Vector3f vector)
{
	/*
#   函数：三维旋转
#   输入：旋转轴，旋转角度（单位弧度），三维向量
#   输出：旋转后的三维向量
	*/
	Vector4f  vectors(0, vector[0], vector[1], vector[2]);
	Vector3f  temp = rotAxis * sin(0.5 * rotAng);
	Vector4f  rot_Ax(cos(0.5 * rotAng), temp[0], temp[1], temp[2]);

	Vector4f  rot_Ax2 = quatinv(rot_Ax);
	vectors = quatmultiply(quatmultiply(rot_Ax, vectors), rot_Ax2);

	Vector3f  out(vectors[1], vectors[2], vectors[3]);
	return out;
}

Vector2i calSunImgPoint(Vector3f RayIn,Vector2f helio_angle, Vector3f cam_rotAngle, Vector2f cam_Length, float cam_pixSize, Vector2f cam_imgSize, Vector2f cam_viewAngle, Vector4f FisheyeImgRointR, Vector2f centerUV)
{
	/*
	#函数：计算入射矢量在相机平面上的成像点坐标：cam_Imgxy=[x,y](列，行),单位像素
#输入：（1）入射矢量：RayIn=[x,y,z],三维矢量，3行1列
#	   (2) helio_angle: 定日镜的方位角和俯仰角，单位角度
#     （3）相机自身旋转角度：cam_rotAngle=[Z1,y1,Z2],1行3列，[自旋角，俯仰角，方位角]，
#            先绕Z轴自旋，后绕Y轴转俯仰，最后绕Z轴转方位，单位角度
#     （4）相机焦距：cam_Length=[fx,fy],fx和fy表示列行方向焦距，单位米
#     （5）像元尺寸：cam_pixSize ,单位米
#     （6）图像大小：cam_imgSize=[行，列]，单位像素
#     （7）相机视角大小：[水平最大视角，垂直最大视角]，单位角度
#     （8）鱼眼相机的旋转参数：FisheyeImgRointR=[k1,k2,k3,k4],1行4列
#输出：入射矢量在相机平面中的像素坐标：cam_imgXY=[x,y](列，行),单位像素
#报错：（1）错误：[3001,3001]-->太阳高度角小于T度,当发生时候跳入下一次计算
#     （2）错误：[3002,3002]-->目标超出相机视场
#     （3）错误：[3003,3003]-->目标超出图像大小
	*/
	if (RayIn[0]==0 && RayIn[1] == 0 && RayIn[2] == 0)
	{
		Vector2i flag(3001,3001);
		return flag;
	}

	cam_rotAngle = cam_rotAngle * PI / 180;     // 相机三轴角度
	helio_angle = helio_angle * PI / 180;      // [方位角，俯仰角]

	Vector3f R1(0,0,1);
	Vector3f R2(0,1,0);
	Vector3f R3(1,0,0);
	
	RayIn = RayIn / RayIn.norm();
	RayIn = vectorRot3D(R1, helio_angle[0],RayIn);   //将定日镜转平
	RayIn = vectorRot3D(R2, helio_angle[1], RayIn);
	// RayIn = vectorRot3D(R1, 90, RayIn);

	float temp0 = RayIn[0];
	RayIn[0] = RayIn[1];   //镜场坐标系xyz 转为 相机坐标系xyz ,xy交换
	RayIn[1] = -temp0;
	RayIn[2] = RayIn[2];

	RayIn = vectorRot3D(R1, -cam_rotAngle[2], RayIn); //将相机姿态转平整
	RayIn = vectorRot3D(R2, -cam_rotAngle[1], RayIn);
	RayIn = vectorRot3D(R3, -cam_rotAngle[0], RayIn);

	float Point_theta = acos(RayIn[2] / RayIn.norm()); //向量与光轴的夹角

	if (cam_viewAngle[0] != 0)
	{
		float angle1 = atan2(abs(RayIn[1]), RayIn[2]);
		float angle2 = atan2(abs(RayIn[0]), RayIn[2]);
		float angle3 = cam_viewAngle[0] / 2;
		float angle4 = cam_viewAngle[1] / 2;

		if (Point_theta>angle3 || angle1>angle3 || angle2>angle4)
		{
			Vector2i flag(3002,3002);
			return flag;
		}
	}

	float PinholeImgPointAng = atan2(RayIn[1], RayIn[0]);

	Vector4f theta_temp(pow(Point_theta,3), pow(Point_theta, 5), pow(Point_theta, 7), pow(Point_theta, 9));
	float Point_theta2 = Point_theta + FisheyeImgRointR.transpose() *theta_temp;

	Vector2f R(cam_Length[0] *Point_theta2,cam_Length[1]*Point_theta2); //改动1== 去掉 cam_pixSize
	//Vector2f imgPoint(R[1] * cos(PinholeImgPointAng), - 1 * R[0] * sin(PinholeImgPointAng));
	Vector2f imgPoint(R[0] * cos(PinholeImgPointAng), R[1] * sin(PinholeImgPointAng));

	Vector2i out(int(round(imgPoint[0] + centerUV[0])), int(round(imgPoint[1] + centerUV[1])));  //列行

	float abs_u = abs(out[0]);
	float abs_v = abs(out[1]);

	if (abs_u > cam_imgSize[1]  || abs_v > cam_imgSize[0])
	{
		Vector2i flag(3003, 3003);
		return flag;
	}
	return out;
}

int a1232132131 = 0;
shared_ptr< vector < SUNPOS> > GenerateSunTable(
	int year, int mon, int day, int startHour, int endHour,
	double lati, double longti,
	double temperature, double pressure, double delta_T, double elevation,
	double focus_x , double focus_y, double cam_pixSize,//焦距 相元大小
	double cam_rotAnglex, double cam_rotAngley, double cam_rotAnglez,//相机安装角度
	double heliopoint_x, double heliopoint_y, double heliopoint_z,//定日镜坐标
	double aimpoint_x, double aimpoint_y, double aim_point_z,//指向点坐标
	double aimpoint1_x, double aimpoint1_y, double aim_point1_z,//指向点坐标
	double Coef1, double Coef2, double Coef3, double Coef4,//矫正系数
	int viewAnglev, int viewAngleh,//行列
	double cam_viewAngle_h, double cam_viewAngle_v,
	float centerU,float centerV
)
{

	Vector3f b(heliopoint_x, heliopoint_y, heliopoint_z);
	Vector3f c1(aimpoint_x, aimpoint_y, aim_point_z);
	Vector3f c2(aimpoint1_x, aimpoint1_y, aim_point1_z);

	Vector3f d;
	//T4参数表获得
	Vector3f e(cam_rotAnglex, cam_rotAngley, cam_rotAnglez);
	Vector2f f(focus_x, focus_y);//焦距 

	Vector2f g(viewAnglev, viewAngleh);
	Vector2f h(cam_viewAngle_h, cam_viewAngle_v);
	Vector4f MappingCoefficients(Coef1, Coef2, Coef3, Coef4);

#define  SHOW_PAR 0
#if SHOW_PAR
	cout << "b" << endl << b << endl;
	cout << "c1" << endl << c1 << endl;
	cout << "c2" << endl << c2 << endl;
	cout << "e" << endl << e << endl;
	cout << "f" << endl << f << endl;
	cout << "g" << endl << g << endl;
	cout << "h" << endl << h << endl;
	cout << "MappingCoefficients" << endl << MappingCoefficients << endl;
#endif

	Vector2f angle;
	Vector2i  ZR;
	Vector2f centerUV(centerU, centerV);
	auto ret = make_shared< vector < SUNPOS> >();

	for (int hour = startHour; hour < endHour; hour++) {
		for (int min = 0; min < 60; min++) {
			for (int sec = 0; sec < 60; sec += 5) {

				if(hour == 15){
					a1232132131 += 10;
				}


				SUNPOS tmppos;
				tmppos.tt = hour * 3600 + min * 60 + sec;

				SP(year, mon, day, hour, min, sec, delta_T, longti, lati, elevation, pressure, temperature, angle);
				Vector2f angle2 = angle;

				Vector2f Helio_angle0;
				d = calSunRayIn(angle, b, c1, 5, Helio_angle0);
				ZR = calSunImgPoint(d,Helio_angle0, e, f, cam_pixSize, g, h, MappingCoefficients, centerUV);


				tmppos.ZR_u = ZR[0];
				tmppos.ZR_v = ZR[1];
				tmppos.a0 = Helio_angle0[0];
				tmppos.a1 = Helio_angle0[1];
				tmppos.d[0][0] = d[0];
				tmppos.d[0][1] = d[1];
				tmppos.d[0][2] = d[2];
				tmppos.test[0] = Helio_angle0[0];
				tmppos.test[1] = Helio_angle0[1];

				Vector2f Helio_angle1;
				d = calSunRayIn(angle2, b, c2, 5, Helio_angle1);
				ZR = calSunImgPoint(d, Helio_angle1,e, f, cam_pixSize, g, h, MappingCoefficients, centerUV);

				tmppos.SD_u = ZR[0];
				tmppos.SD_v = ZR[1];
				tmppos.a2 = Helio_angle1[0];
				tmppos.a3 = Helio_angle1[1];
				tmppos.d[1][0] = d[0];
				tmppos.d[1][1] = d[1];
				tmppos.d[1][2] = d[2];

				ret->emplace_back(tmppos);
			}
		}
	}


	vector<SUNPOS> & grp = *ret;
	if (grp.size()) {
		grp[0].ZR_At = 0;
		grp[0].ZR_Az = 0;
		grp[0].SD_At = 0;
		grp[0].SD_Az = 0;
		for (size_t i = 1; i < grp.size(); i++) {
			SUNPOS & pre = grp[i - 1];
			SUNPOS & itr = grp[i];

			itr.ZR_At = itr.a1 - pre.a1;
			itr.ZR_Az = itr.a0 - pre.a0;
			itr.SD_At = itr.a3 - pre.a3;
			itr.SD_Az = itr.a2 - pre.a2;
		}
	}

	return ret;
}


int getidealmain(int argc, char* argv[])
{
	Vector3f b(-100, 100, 3);
	Vector3f c(0, 0, 98);
	Vector3f Rayin(0.197, -0.357, 0.913);
	//T4参数表获得
	Vector3f angleXYZ(0.04761203743457128, -0.6556470204718018, -0.17746299323266249 + 90);
	Vector2f fxfy(684.818893, 681.489169);//焦距 
	Vector2f imgSize(1080, 1920);
	Vector2f viewAngle(170, 95);

	Vector4f MappingCoefficients(-0.014484, -0.136218, 0.478277, -0.657337);
	Vector2f centerUV(971.6, 531.9);

	Vector2f angle;
	Vector2f Helio_angle;//[方位角，俯仰角]，单位角度
	Vector2f helio_angle(20,30); //假设的定日镜方位角和俯仰角，单位角度

	for (size_t i = 0; i < 1; i++) {
		auto start = std::chrono::system_clock::now();
		SP(2020, 8, 12, 10, 0, i, 70, 97.277, 40.329, 1415.5, 847.2, 7.8, angle);

		//angle 太阳高度角  方位角
		//b 定日镜坐标
		//c1 指向点坐标1 c2 指向点坐标1
		//15 开场角度
		//太阳入射矢量 xyz
		Vector2i  ZR = calSunImgPoint(Rayin, helio_angle,angleXYZ, fxfy, 2.7e-6, imgSize, viewAngle, MappingCoefficients, centerUV);

		auto end = std::chrono::system_clock::now();
		cout << std::chrono::duration_cast<std::chrono::duration<float>>(end - start).count() << endl;
		std::cout << ZR << std::endl;
	}
	return 0;
}


