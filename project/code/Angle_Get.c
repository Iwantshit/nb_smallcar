#include "Angle_Get.h"

float mag_angle_out,icm_gyro_y;       //�شż�У׼��ĽǶ�
double angle;                //�����ںϺ�ĽǶ�
float mag_ratio =0.7;      //�شżƱ���
float gyro_ratio =0.3;    //�����Ǳ���
float dt =0.005;            //��������
int16 mxmax=0,mxmin=0,mymax=0,mymin=0;
int16 mxoffset=0,myoffset=0;



//�شż������˲�
float angle_calc(float mag_angle_m,float gyro_m)
{
    float temp_angle;
    float gyro_now;
    float error_angle;
    static float last_angle;
    static float last_mag_angle;
    static uint8 first_angle;
    if(!first_angle)//�ж��Ƿ�Ϊ��һ�����б�����
    {
        //����ǵ�һ�����У����ϴνǶ�ֵ����Ϊ��شżƽǶ�һ��
        first_angle =1;
        last_angle = mag_angle_m;
        last_mag_angle=mag_angle_m;
    }


    //���������ǵ�����ȥ���㳵ģ�����ϴ�ת���˶��ٽǶ�
    gyro_now = gyro_m* dt * gyro_ratio;
    //���ݲ������ĵشż�ֵת��Ϊ�Ƕ�֮�����ϴεĽǶ�ֵ��ƫ��
    error_angle =(mag_angle_m - last_angle) * mag_ratio;
    //����ƫ���������ǲ����õ��ĽǶ�ֵ���㵱ǰ�Ƕ�ֵ
    temp_angle = last_angle +error_angle+ gyro_now;

//    tft180_show_float(0,80,error_angle, 4,  2);
//    tft180_show_float(0,100,gyro_now, 4,  2);

    //���浱ǰ�Ƕ�ֵ
    last_angle = temp_angle;
    last_mag_angle=mag_angle_m;
    return temp_angle;

}

//��ȡ�Ƕ�
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

    angle = angle_calc(mag_angle_out, icm_gyro_y);  //һ�׻����˲�����Ƕ�ֵ

    return angle;
}




