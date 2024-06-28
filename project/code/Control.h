#ifndef Control_H
#define Control_H
#include "zf_common_headfile.h"
#include "math.h"
#include "time.h"
#include "stdlib.h"
#include "Location.h"
#include "Pid.h"

#define S_MOTOR_PIN   TCPWM_CH13_P00_3       //定义舵机引脚

void speed_control(void);
void gps_direction_control(void);
//void pid_compute(void);
//void kalman_init(double* P, double* x);
//double kalman_filter(double z, double* P, double* x);
#endif

