#ifndef _PID_H
#define _PID_H	
#include "zf_common_headfile.h"
#include "math.h"


typedef struct 
{
    float Kp;
    float Ki;
    float Kd;
    float Error;
    float Last_Error;
    float Last_Last_Error;
    float ErrorSum;
    float error_sum_max;
    float error_sum_min;
    float Dalt;
    float Pout;
    float Iout;
    float Dout;
    float Out;
    float out_max;
    float out_min;
}PID;

void  Pid_init(PID* pid,float Kp,float Ki,float Kd,float out_max,float out_min);
void  Pos_Pid_Contrl(PID* pid,float now_val,float aim_val);
void  Ics_Pid_Contrl(PID* pid,float now_val,float aim_val);


#endif

