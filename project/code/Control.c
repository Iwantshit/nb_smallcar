#include"Control.h"

float servo_motor_duty = 1000;                                                          // 舵机动作角度
float servo_motor_dir = 1;                                                          // 舵机动作状态
float gps_angle_error2=0;
float target_duty = 25.0;
int16 encoder=0;
float speed=0;                      //定义编码器获取速度
float PID_speed = 0.0f;    //PID计算后输出速度
/* 定义PID参数 */
#define KP 1.0f
#define KI 0.2f
#define KD 0.1f
float PID_error;
float integral;    //积分项
float derivative;   //微分项
float last_error=0.0f;

float angle_P = 5 ,angle_D = 6 ,state_d=1,mag_p=0.5,distance_p=5,state_r=1;; //D 0.85 2.5
float direction_pid;
int change_to_straight=0;
extern double gps_angle_error;
extern int location_point;

double P1, P2, P3, P4, P;
double D;

//
int S_point_start=  13,S_point_end=18 ,rotate_start=7,rotate_end=10,turn_around=5,podao=2;//特殊元素标志位
//这里的S_point_start是S弯的第二个桶的右侧点，rotate_start是指绕桶上方的点，S_point_end是指最后一个桶的点，rotate_end是指绕桶下方的点
//
//无刷电机参数
uint8 carCommamd,setdir=0;
extern uint32 setduty=1;
uint32 set_z;
void speed_control(void)
{
//    if(location_point>=S_point_start-1&&location_point<=S_point_end)//设置s弯道时的速度，在进入s弯前减速
//    {
//        setduty=100;
//    }
//    else if(location_point>=rotate_start-1 &&location_point<=rotate_end)//设置绕桶时的速度，在进入绕桶轨道前减速
//    {
//        setduty=100;
//    }
//    else if(location_point==podao)
//    {
//        setduty=100;
//    }
//    else//设置直道速度
//    {
//        setduty=set_z;
//    }
    setduty=set_z;
}
void gps_direction_control(void)
{

    //计算与目标点偏离角
    servomotor_angle();
    if(change_to_straight==1)
    {
        gps_angle_error2=0;
        change_to_straight=0;
    }
    if(gps_angle_error<=20&&gps_angle_error>=-20){ angle_P=-1;angle_D=3;} //1.5
//    else if(gps_angle_error<=30&&gps_angle_error>=-30){angle_P=-1.5;angle_D=3.5;} //3.5
    else if(gps_angle_error<=40&&gps_angle_error>=-40){angle_P=-2;angle_D=4;} //3.5  5.0
//    else if(gps_angle_error<=50&&gps_angle_error>=-50){angle_P=-2.5;angle_D=4.5;}
    else if(gps_angle_error<=60&&gps_angle_error>=-60){angle_P=-3;angle_D=5;}
    else {angle_P=-4;angle_D=5.5;} //4.7
    
    //P的范围在0-2.5
    if (gps_angle_error<=15&&gps_angle_error>=-15){P=0;D=0;}
    else if(gps_angle_error<=25&&gps_angle_error>=-25){P=0.8;D=0;}
    else if(gps_angle_error<=35&&gps_angle_error>=-35){P=1.3;D=0;}
    else if(gps_angle_error<=45&&gps_angle_error>=-45){P=1.6;D=0;}
    else if(gps_angle_error<=55&&gps_angle_error>=-55){P=1.85;D=0;}
    else if(gps_angle_error<=65&&gps_angle_error>=-65){P=2.2;D=0.5;}
    else {P=2.4;D=0.8;}
    


    //direction_pid = angle_P * gps_angle_error + angle_D * (gps_angle_error - gps_angle_error2); //p=3,d=0.05
    
    direction_pid = P * gps_angle_error + D * (gps_angle_error - gps_angle_error2);

    
    
//    tft180_show_float(30,130,(float)(700-direction_pid),3,2);
    if(direction_pid > 150 )
    {
        direction_pid =  150;
    }
    else if(direction_pid < -150)
    {
        direction_pid =  -150;
    }
//        tft180_show_int(0, 120,direction_pid,3);

    
    pwm_set_duty(S_MOTOR_PIN, 680+direction_pid);
    //system_delay_ms(50);
    gps_angle_error2 = gps_angle_error;

}
/*char strdd[21];//不会啦   不会啦要发疯啦怎么这么难啊 这还比什么啊 老子不干了 谁爱干谁干 反正我不干 拿了奖给我就行 
    memset(strdd,0,sizeof(strdd));
    sprintf(strdd,"%.7lf",690+direction_pid);
    wireless_uart_send_string(strdd);
    wireless_uart_send_string(",");
    memset(strdd,0,sizeof(strdd));
    sprintf(strdd,"%.7lf",gps_angle_error);
    wireless_uart_send_string(strdd);
    wireless_uart_send_string("\n");*/
