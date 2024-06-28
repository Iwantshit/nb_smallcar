#include "Pid.h"                 

void  Pid_init(PID* pid,float Kp,float Ki,float Kd,float out_max,float out_min)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;

    pid->Error = 0;
    pid->Last_Error = 0;
    pid->Last_Last_Error = 0;
    pid->Dalt = 0;
    pid->ErrorSum = 0;
    
    pid->error_sum_max = 0;
    pid->error_sum_min = 0;
    pid->out_max = out_max; 
    pid->out_min = out_min;  
}

//位置式PID
void  Pos_Pid_Contrl(PID* pid,float now_val,float aim_val)
{
    pid->Error = aim_val-now_val;
    pid->ErrorSum += pid->Error;
    pid->Dalt = pid->Error - pid->Last_Error;
    pid->Pout = pid->Kp*pid->Error;
    pid->Iout = pid->Ki*pid->ErrorSum;
    pid->Dout = pid->Kd*pid->Dalt;
    pid->Out = pid->Pout+pid->Iout+pid->Dout;
    if(pid->Out>pid->out_max)pid->Out= pid->out_max;
    if(pid->Out<pid->out_min)pid->Out = pid->out_min;
}

//增量式PID
void  Ics_Pid_Contrl(PID* pid,float now_val,float aim_val)
{
    pid->Error = -0.1*(aim_val - now_val);
    pid->Pout = pid->Kp*(pid->Error - pid->Last_Error);
    pid->Iout = pid->Ki*(pid->Error);
    pid->Dout = pid->Kd*(pid->Error - 2*pid->Last_Error+pid->Last_Last_Error);
    pid->Out  += pid->Pout+pid->Iout+pid->Dout; 
    pid->Last_Last_Error = pid->Last_Error;
    pid->Last_Error =  pid->Error;
    if(pid->Out > pid->out_max) pid->Out = pid->out_max;
    if(pid->Out < pid->out_min) pid->Out = pid->out_min;
}
