#include <math.h>

int ConAlg(float ut, float vt, float ZR_u, float ZR_v, float ZR_At, float ZR_Az, float r1, float r2,
	   float q, float fx, float fy, float x_pos_2, float y_pos_2, float *At, float *Az,
	   int *SpdSig_At, int *SpdSig_Az, int *Slpflg)
{
	//ut,vt：当前时刻图片像素点
	//ZR_u, ZR_v：下一时刻对应的目标像素点，根据时间查T0_目标轨迹表
	//ZR_At, ZR_Az：下一时刻理论增量角度表，根据时间查T0_目标轨迹表
	//r1,r2：比例控制参数
	//q:像元尺寸cam_pixSize ,单位米
	//fx,fy:相机焦距：cam_Length=[fx,fy],fx和fy表示列行方向焦距，单位米
#if 0
	float *At = 0;//俯仰轴增量角度、弧度
	float *Az = 0;//方位轴增量角度、弧度
	int *SpdSig_At = 0;//转速：1表示实际速度转动，0表示最大速度转动，2表示正常速度转动(0.03),3来云时转速)
	int *SpdSig_Az = 0;
#endif
	if (ut > 2999 || vt > 2999) {
		*At = ZR_At; //At:俯仰增量角度
		*Az = ZR_Az; // Az:方位增量角度
		*SpdSig_At = 3; //俯仰转速信号
		*SpdSig_Az = 3; //方位转速信号
		*Slpflg = 2;
	} else {
		float del_x = vt - ZR_v; //当前俯仰方向/v方向像素差
		float del_y = ut - ZR_u; //当前方位方向/u方向像素差
		float nt = sqrt(del_x * del_x + del_y * del_y);
		if (nt < 5) //判断像素差是否大于阈值K
		{
			float JJ11 = -(fx * q * ut) /
				     (vt * (fx * fx + q * q * ut * ut + q * q * vt * vt));
			float JJ12 = -(fx * q) / (fx * fx + q * q * ut * ut + q * q * vt * vt);
			float JJ21 = (fy * fy + q * q * vt * vt) /
				     (vt * (fy * fy + q * q * ut * ut + q * q * vt * vt));
			float JJ22 = -(q * q * ut) / (fy * fy + q * q * ut * ut + q * q * vt * vt);
			*At = -(JJ11 * del_y + JJ12 * del_x) * r1 * 180 /
			      3.1415926; //At:俯仰增量角度
			*Az = -(JJ21 * del_y + JJ22 * del_x) * r2 * 180 /
			      3.1415926; // Az:方位增量角度
			//*At = atan2(q*del_x, fy)*r1 * 180 / 3.1415926;//At:俯仰增量角度
			//*Az = -atan2(del_y, del_x)*r2 * 180 / 3.1415926;// Az:方位增量角度
			//*Az = atan2(q*(del_y), fx)*r2 * 180 / 3.1415926;
			*SpdSig_At = 1; //俯仰转速信号
			*SpdSig_Az = 1; //方位转速信号
			*Slpflg = 0;
		} else if (nt >= 5 & nt < 10) {
			if (x_pos_2 > 0) {
				*Az = atan2(q * del_x, fy) * r1 * 180 / 3.1415926; //At:俯仰增量角度
				//*Az = -atan2(del_y, del_x)*r2 * 180 / 3.1415926;// Az:方位增量角度
				*At = atan2(q * (del_y), fx) * r2 * 180 / 3.1415926;
			} else {
				*At = atan2(q * del_x, fy) * r1 * 180 / 3.1415926; //At:俯仰增量角度
				//*Az = -atan2(del_y, del_x)*r2 * 180 / 3.1415926;// Az:方位增量角度
				*Az = -atan2(q * (del_y), fx) * r2 * 180 / 3.1415926;
			}
			*SpdSig_At = 2; //俯仰转速信号
			*SpdSig_Az = 2; //方位转速信号
			*Slpflg = 0;
			*At = *At > 0 ? 0.03 : -0.03;
			*Az = *Az > 0 ? 0.03 : -0.03;
		} else if (nt < 50) {
			if (x_pos_2 > 0) {
				*Az = atan2(q * del_x, fy) * r1 * 180 / 3.1415926; //At:俯仰增量角度
				//*Az = -atan2(del_y, del_x)*r2 * 180 / 3.1415926;// Az:方位增量角度
				*At = atan2(q * (del_y), fx) * r2 * 180 / 3.1415926;
			} else {
				*At = atan2(q * del_x, fy) * r1 * 180 / 3.1415926; //At:俯仰增量角度
				//*Az = -atan2(del_y, del_x)*r2 * 180 / 3.1415926;// Az:方位增量角度
				*Az = -atan2(q * (del_y), fx) * r2 * 180 / 3.1415926;
			}
			*SpdSig_At = 0; //俯仰转速信号
			*SpdSig_Az = 0; //方位转速信号 最大速度
			*Slpflg = 0;
			*At = *At > 0 ? 1 : -1;
			*Az = *Az > 0 ? 1 : -1;
		}

		else {
			if (x_pos_2 > 0) {
				*Az = atan2(q * del_x, fy) * r1 * 180 / 3.1415926; //At:俯仰增量角度
				//*Az = -atan2(del_y, del_x)*r2 * 180 / 3.1415926;// Az:方位增量角度
				*At = atan2(q * (del_y), fx) * r2 * 180 / 3.1415926;
			} else {
				*At = atan2(q * del_x, fy) * r1 * 180 / 3.1415926; //At:俯仰增量角度
				//*Az = -atan2(del_y, del_x)*r2 * 180 / 3.1415926;// Az:方位增量角度
				*Az = -atan2(q * (del_y), fx) * r2 * 180 / 3.1415926;
			}
			*SpdSig_At = 0; //俯仰转速信号
			*SpdSig_Az = 0; //方位转速信号 最大速度
			*Slpflg = 1;
			*At = *At > 0 ? 5 : -5;
			*Az = *Az > 0 ? 5 : -5;
		}
	}
	return 0;
}