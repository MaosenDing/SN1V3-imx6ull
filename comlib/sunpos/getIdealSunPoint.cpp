#include<stdio.h>
#include<math.h>
#include <Eigen/Dense>  
#include <iostream>
#include "timeTableV2.h"
#include <chrono>
#define PI 3.1415926535
using namespace Eigen;
using namespace std;
Vector2f SP(double Year, double Month, double Day, double Hour, double Minute, double Second, double Delta_T, double  longitude, double latitude, double E, double P, double T);


Vector3f calSunRayIn(Vector2f & sunAngle,Vector3f & helioPoint,Vector3f & aimPoint,float T)
{
/*	
# 函数：计算太阳的三维入射矢量
# 输入： （1）太阳高度角和方位角：sunAngle=[高度角，方位角]，1行3列，单位角度；
#       （2）定日镜坐标XYZ：helioPoint=[x,y,z],1行3列，单位米；
#       （3）指向点坐标XYZ：aimPoint=[x,y,z],1行3列，单位米；
# 输出: 入射矢量XYZ:[x,y,z]，1行3列
# 备注: 单循环周期时间为 1s,太阳高度角大于 15°
# 报错：输出错误[null,null,null]，表示太阳高度角小于T°
*/
	if (sunAngle[0]<=T)
	{	
		Vector3f Rayin_h_ZR(NULL,NULL,NULL);
		return Rayin_h_ZR;
	}

	sunAngle = sunAngle * PI / 180;

	Vector3f Rayin_f(-cos(sunAngle[0])*cos(sunAngle[1]),cos(sunAngle[0])*sin(sunAngle[1]),sin(sunAngle[0]));

	Vector3f Rayout_f_ZR = (aimPoint - helioPoint) / ((aimPoint - helioPoint).norm());
	Vector3f Normal_f_ZR = (Rayin_f + Rayout_f_ZR) / ((Rayin_f + Rayout_f_ZR).norm());

	float Az_ZR = atan2(Normal_f_ZR[1], Normal_f_ZR[0]);
	float At_ZR = PI / 2 - asin(Normal_f_ZR[2]);

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

Vector2i calSunImgPoint(Vector3f RayIn,Vector3f cam_rotAngle,Vector2f cam_Length,float cam_pixSize,Vector2f cam_imgSize,Vector2f cam_viewAngle,Vector4f FisheyeImgRointR)
{
	/*
	#函数：计算入射矢量在相机平面上的成像点坐标：cam_Imgxy=[x,y](列，行),单位像素
#输入：（1）入射矢量：RayIn=[x,y,z],三维矢量，3行1列
#     （2）相机自身旋转角度：cam_rotAngle=[Z1,y1,Z2],1行3列，[自旋角，俯仰角，方位角]，
#            先绕Z轴自旋，后绕Y轴转俯仰，最后绕Z轴转方位，单位角度
#     （3）相机焦距：cam_Length=[fx,fy],fx和fy表示列行方向焦距，单位米
#     （4）像元尺寸：cam_pixSize ,单位米
#     （5）图像大小：cam_imgSize=[行，列]，单位像素
#     （6）相机视角大小：[水平最大视角，垂直最大视角]，单位角度
#     （7）鱼眼相机的旋转参数：FisheyeImgRointR=[k1,k2,k3,k4],1行4列
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

	cam_rotAngle = cam_rotAngle * PI / 180;

	Vector3f R1(0,0,1);
	Vector3f R2(0,1,0);
	Vector3f R3(0,0,1);

	RayIn = vectorRot3D(R1, -cam_rotAngle[2], RayIn);
	RayIn = vectorRot3D(R2, -cam_rotAngle[1], RayIn);
	RayIn = vectorRot3D(R3, -cam_rotAngle[0], RayIn);

	float Point_theta = acos(RayIn[2] / RayIn.norm());

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

	Vector2f R(cam_Length[0]*Point_theta2,cam_Length[1]*Point_theta2);
	Vector2f imgPoint(R[1] * cos(PinholeImgPointAng) / cam_pixSize, - 1 * R[0] * sin(PinholeImgPointAng) / cam_pixSize);

	float abs_x = abs(imgPoint[0]);
	float abs_y = abs(imgPoint[1]);

	if (abs_y > cam_imgSize[0] / 2 || abs_x > cam_imgSize[1] / 2)
	{
		Vector2i flag(3003, 3003);
		return flag;
	}

	imgPoint =  imgPoint + cam_imgSize/2;
	Vector2i out(int(round(imgPoint[1])), int(round(imgPoint[0])));
	return out;
}

	//测试主要函数
#if 0
int testsunpos(int argc,char* argv[])
{
	Vector3f b(1, 2, 3);
	Vector3f c(3,4,5);
	Vector3f d;
	//T4参数表获得
	Vector3f e(-1 / PI * 180, 1 / PI * 180, 1 / PI * 180);
	Vector2f f(1.37e-3, 1.37e-3);//焦距 
	Vector2f g(1080, 1920);
	Vector2f h(150, 75);

	Vector4f MappingCoefficients(-0.032804, 0.014267, -0.031747, 0.015501);


	for (size_t i = 0; i < 100; i++) {
		auto start = std::chrono::system_clock::now();


		Vector2f angle;
		angle = SP(2020, 7, 27, 6, 1, 20, 70, 121.506, 31.088, 14, 1014, 17);
		//angle 太阳高度角  方位角
		//b 定日镜坐标
		//c1 指向点坐标1 c2 指向点坐标1
		//15 开场角度
		d1 = calSunRayIn(angle, b, c1, 15);
		d2 = calSunRayIn(angle, b, c2, 15);
		//太阳入射矢量 xyz

/*
#函数：计算入射矢量在相机平面上的成像点坐标：cam_Imgxy=[x,y](列，行),单位像素
#输入：（1）入射矢量：RayIn=[x,y,z],三维矢量，3行1列
#     （2）相机自身旋转角度：cam_rotAngle=[Z1,y1,Z2],1行3列，[自旋角，俯仰角，方位角]，
#            先绕Z轴自旋，后绕Y轴转俯仰，最后绕Z轴转方位，单位角度
#     （3）相机焦距：cam_Length=[fx,fy],fx和fy表示列行方向焦距，单位米
#     （4）像元尺寸：cam_pixSize ,单位米
#     （5）图像大小：cam_imgSize=[行，列]，单位像素
#     （6）相机视角大小：[水平最大视角，垂直最大视角]，单位角度
#     （7）鱼眼相机的旋转参数：FisheyeImgRointR=[k1,k2,k3,k4],1行4列
#输出：入射矢量在相机平面中的像素坐标：cam_imgXY=[x,y](列，行),单位像素
#报错：（1）错误：[3001,3001]-->太阳高度角小于T度,当发生时候跳入下一次计算
#     （2）错误：[3002,3002]-->目标超出相机视场
#     （3）错误：[3003,3003]-->目标超出图像大小
*/
		Vector2i  ZR = calSunImgPoint(d1, e, f, 2.8e-6, g, h, MappingCoefficients);
		Vector2i  DR = calSunImgPoint(d2, e, f, 2.8e-6, g, h, MappingCoefficients);
		auto end = std::chrono::system_clock::now();
		cout << std::chrono::duration_cast<std::chrono::duration<float>>(end - start).count() << endl;
	}

	//std::cout << angle << std::endl;
	//std::cout << "-----------------" << std::endl;
	//std::cout << pp << std::endl;


	return 0;
}
#else
int testsunpos(int argc, char* argv[])
{
	return 0;
}
#endif












