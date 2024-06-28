#include "Angle_Get.h"

float mag_angle_out,icm_gyro_y;       //地磁计校准后的角度
double angle;                //数据融合后的角度
float mag_ratio =0.7;      //地磁计比例
float gyro_ratio =0.3;    //陀螺仪比例
float dt =0.005;            //采样周期
int16 mxmax=0,mxmin=0,mymax=0,mymin=0;
int16 mxoffset=0,myoffset=0;



//地磁极互补滤波
float angle_calc(float mag_angle_m,float gyro_m)
{
    float temp_angle;
    float gyro_now;
    float error_angle;
    static float last_angle;
    static float last_mag_angle;
    static uint8 first_angle;
    if(!first_angle)//判断是否为第一次运行本函数
    {
        //如果是第一次运行，则将上次角度值设置为与地磁计角度一致
        first_angle =1;
        last_angle = mag_angle_m;
        last_mag_angle=mag_angle_m;
    }


    //根据陀螺仪的数据去估算车模距离上次转动了多少角度
    gyro_now = gyro_m* dt * gyro_ratio;
    //根据测量到的地磁计值转换为角度之后与上次的角度值求偏差
    error_angle =(mag_angle_m - last_angle) * mag_ratio;
    //根据偏差与陀螺仪测量得到的角度值计算当前角度值
    temp_angle = last_angle +error_angle+ gyro_now;

//    tft180_show_float(0,80,error_angle, 4,  2);
//    tft180_show_float(0,100,gyro_now, 4,  2);

    //保存当前角度值
    last_angle = temp_angle;
    last_mag_angle=mag_angle_m;
    return temp_angle;

}

//获取角度
float angle_get()
{
    imu963ra_get_mag();
    imu963ra_get_acc();

    if(imu963ra_mag_x>mxmax) mxmax=imu963ra_mag_x;
    if(imu963ra_mag_y>mymax) mymax=imu963ra_mag_y;
    if(imu963ra_mag_x<mxmin) mxmin=imu963ra_mag_x;
    if(imu963ra_mag_y<mymin) mymin=imu963ra_mag_y;
    mxoffset=(mxmax+mxmin)/2;
    myoffset=(mymax+mymin)/2;

    mag_angle_out = atan2(imu963ra_mag_y-myoffset,imu963ra_mag_x-mxoffset)*(180/PI);
    mag_angle_out = ((mag_angle_out > 0) ? mag_angle_out : (mag_angle_out + 360)) + 90;
    mag_angle_out = ((mag_angle_out > 360) ? (mag_angle_out - 360) : (mag_angle_out)) ;

    icm_gyro_y=imu963ra_gyro_transition(imu963ra_gyro_y);

    angle = angle_calc(mag_angle_out, icm_gyro_y);  //一阶互补滤波计算角度值

    return angle;
}




