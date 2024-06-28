#include "zf_common_headfile.h"
#include "string.h"
#include <stdlib.h>
#include "Location.h"
#include "Angle_Get.h"
#include "Control.h"
#include "Pid.h"
#define PIT_NUM                 (PIT_CH0 )                                     // 使用的周期中断编号
#define PIT_NUM2                (PIT_CH1 )
#define LED1                    (P19_0)

#define KEY1                    (P20_0)
#define KEY2                    (P20_1)
#define KEY3                    (P20_2)
#define KEY4                    (P20_3)

//停止标志位
extern int stop_flag;
extern int location_point;
extern double Angle;
extern double gps_angle_error;
extern uint32 setduty;
extern double carlocation[50][2];
extern uint32 set_z;

//开关标志位
extern uint8 key1_flag;
extern uint8 key2_flag;
extern uint8 key3_flag;
extern uint8 key4_flag;

//无刷
#define MAX_DUTY            (30 )                                              // 最大占空比输出限制
#define DIR_CH1             (P10_3)                                            // 电机方向输出端口
#define PWM_CH1             (TCPWM_CH30_P10_2)                                 // PWM输出端口

//PD
extern double P1, P2, P3, P4;
extern double D;

int first_read_flag=0;

double flatitude,flongitude;
//大树30,315185   120.34718
//30.314865  120.347.23
uint16 duty;
//开关状态变量 
uint8 key1_status = 1;
uint8 key2_status = 1;
uint8 key3_status = 1;
uint8 key4_status = 1;

/* 定义计时器 */
uint32 S_time;

uint8 point_flag=0;
uint8 read_flag=0;

 
//#define FLASH_SECTION_INDEX       (0)                                 // 存储数据用的扇区
//#define FLASH_PAGE_INDEX          (9)                                // 存储数据用的页码 倒数第一个页码
PID DC_PID;

#define S_MOTOR_PIN   TCPWM_CH13_P00_3       //定义舵机引脚

 int main(void)
{  
    int abcd=0;
    S_time=0;
    
    clock_init(SYSTEM_CLOCK_250M); 	// 时钟配置及系统初始化<务必保留>
    debug_info_init();                  // 调试串口信息初始化
    gpio_init(LED1, GPO, GPIO_LOW, GPO_PUSH_PULL);          // 初始化 LED1 输出 默认高电平 推挽输出模式
    
    
    P1 = 0;
    P2 = 1.88;
    P3 = 2.5;
    P4 = 2.5;

    tft180_init();                                                               //显示屏
    system_delay_ms(2000);
    
    /*wireless_uart_init();                                                       //无线串口
    system_delay_ms(2000);*/

    //无刷电机初始化
    pwm_init(PWM_CH1, 1000, 0);                                                 // PWM 通道1 初始化频率1KHz 占空比初始为0
    gpio_init(DIR_CH1, GPO, GPIO_HIGH, GPO_PUSH_PULL);                          // 初始化电机方向输出引脚

    pwm_init(S_MOTOR_PIN, 50, 680);                                                    //舵机居中
    
    imu963ra_init();                                                                  //磁力计
    system_delay_ms(2000);

    gnss_init(1);
    system_delay_ms(2000);
    
    // 周期中断初始化
    pit_ms_init(PIT_NUM, 5);
    pit_ms_init(PIT_NUM2, 5);
    
    location_init();
    key1_flag=0;
    key2_flag=0;
    key3_flag=0;
    key4_flag=0;
    int GPS_flag=0;
    point_flag=0;

    gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY1 输入 默认高电平 上拉输入
    gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY2 输入 默认高电平 上拉输入
    gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY3 输入 默认高电平 上拉输入
    gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY4 输入 默认高电平 上拉输入
    
    set_z = 30;
    
    //tft180_show_string(0,40,"Angle");
   // tft180_show_float(70,40,Angle, 3,  2);                    //我写的
   // tft180_show_int(70,40,stop_flag,1);
    
            
    while (true)
    {
        tft180_show_int(0,80, 0,3);

        GPS_flag=0;
                    
        
        if(gnss_flag)
        {
            GPS_flag=1;
            gnss_flag=0;
            int num = gnss_data_parse();//GPS数据解析

            //数据传回
            tft180_show_float(30,0,(float)gnss.latitude, 3,6);
            tft180_show_float(30,20,(float)gnss.longitude,3,6);

        }
        
        key1_status = gpio_get_level(KEY1);
        key2_status = gpio_get_level(KEY2);
        key3_status = gpio_get_level(KEY3);
        key4_status = gpio_get_level(KEY4);

        if(!key1_status)    key1_flag = 1;
        if(!key2_status)    key2_flag = 1;
        if(!key3_status)    key3_flag = 1;
        if(!key4_status)    key4_flag = 1;

        //按键1 车辆起跑
        if(key1_flag==1)
        {
          
          //原定代码
            //判断是否过点
            judge_pass_location();
            tft180_show_string(0,60,"angle_err");
            tft180_show_float(70,60,(float)gps_angle_error,3,2);
            
            //电机控制
            speed_control();

            //获取地磁计角度
            tft180_show_string(0,40,"Angle");
            tft180_show_float(70,40,Angle, 3,  2);
            
            tft180_show_int(70,40,stop_flag,1);
            if(stop_flag==1)
            {
                //舵机居中
                pwm_init(S_MOTOR_PIN, 50, 680);
                //电机
                setduty=30;
               // pwm_set_duty(MOTOR2_PWM, setduty);
                pwm_set_duty(PWM_CH1, setduty * (PWM_DUTY_MAX / 100));                 // 计算占空比
                gpio_set_level(DIR_CH1, 1);
                key1_flag=0;
                   

            }         
        }
        
        
        if(key2_flag==1)
        {
            key2_flag=0;
            /*//原定的是调速
            set_z=set_z+500;
            if(set_z>=6000)
            {
                set_z=6000;
            }
            //tft180_show_int(30,20,set_z,4);
            ips200_show_int(30,20,set_z,4);
            system_delay_ms(1000);
            */
            
            //现改为调整P
            P1 += 0.5;
            P2 += 0.5;
            P3 += 0.5;
            P4 += 0.5;
            tft180_show_float(0,100,P1,1,3);
            tft180_show_float(0,120,P2,1,3);
            tft180_show_float(0,140,P3,1,3);
            tft180_show_float(0,160,P4,1,3);
            system_delay_ms(1000);
        }
        
        
        //按键3用于获取当前坐标位置
        if(key3_flag==1)
        {
          abcd++;
          tft180_show_int(0,80, abcd,3);
            key3_flag=0;
            if(GPS_flag==1)
            {
                if(point_flag<0)point_flag =0;
                else if(point_flag>=0)
                {
                    carlocation[point_flag][0]=gnss.latitude;
                    carlocation[point_flag][1]=gnss.longitude;
                    
                    tft180_show_int(20, 80,point_flag,1);
                    point_flag++;
                }
                system_delay_ms(500);
            }
            else
            {
                  tft180_show_int(20, 80,273,1);
            }

        }

        if(key4_flag==1)
        {
            key4_flag=0;
            stop_flag=0;
            location_point=0;
//            flash_buffer_clear();
//            flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);
//            if(flash_union_buffer[2*read_flag].float_type!=-1.0
//                    && flash_union_buffer[2*read_flag+1].float_type!=-1.0)
                if(carlocation[read_flag][0]!=-1)
                {
                    char strdd[21];
                   /* memset(strdd,0,sizeof(strdd));
                    sprintf(strdd,"%.7lf",carlocation[read_flag][0]-30);
                    wireless_uart_send_string("3");
                    wireless_uart_send_string(strdd);
                    wireless_uart_send_string(",");
                    memset(strdd,0,sizeof(strdd));
                    sprintf(strdd,"%.7lf",carlocation[read_flag][1]-120);
                    wireless_uart_send_string("12");
                    wireless_uart_send_string(strdd);
                    wireless_uart_send_string("\n");*/
                    tft180_show_float(0,100, flash_union_buffer[2*read_flag].float_type, 3,  6);
                    tft180_show_float(0,120,flash_union_buffer[2*read_flag+1].float_type, 3,  6);
                    read_flag++;
                }
                system_delay_ms(500);
        }


        
    }
}

