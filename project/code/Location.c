#include"Location.h"

//��¼·������
double carlocation[50][2];

int location_point=0;
//����
extern double angle;
double distance=-1,distance_next=-1;
double gps_angle_error,gps_angle_error_next;
double azimuth=0,azimuth_next=0,distance_atob=-1,distance_atoctob=-1,distance_error=-1;
//Ŀ����������һ��������
double latitude_target,longitude_target,latitude_next,longitude_next;
//ֹͣ��ʶ
int stop_flag=0;

extern int S_point_start,S_point_end ,rotate_start,rotate_end;

int first=0;
extern uint32 S_time;
extern int change_to_straight;
//�ж��ǵ��ﵱǰĿ���
void location_init(void)
{
    for(int i=0;i<50;i++)
    {
        carlocation[i][0]=-1;
        carlocation[i][1]=-1;
    }
}
void judge_pass_location(void)
{
    latitude_target = carlocation[location_point][0];
    longitude_target = carlocation[location_point][1];
    if(latitude_target==-1&&longitude_target==-1)
    {
        stop_flag=1;
    }
    distance = get_two_points_distance (gnss.latitude, gnss.longitude, latitude_target, longitude_target);

    if((distance < 1.5)&& distance!=-1)
    {
      distance=-1;
      location_point++;
    }
    else if(carlocation[location_point+1][0]!=-1&&carlocation[location_point+1][1]!=-1)
    {
        latitude_next = carlocation[location_point+1][0];
        longitude_next = carlocation[location_point+1][1];
        distance_next = get_two_points_distance (gnss.latitude, gnss.longitude, latitude_next, longitude_next);
        distance = get_two_points_distance (gnss.latitude, gnss.longitude, latitude_target, longitude_target);
        if(distance_next<distance&&distance_next<1&&distance>1)
        {
            location_point++;
            latitude_target = carlocation[location_point][0];
            longitude_target = carlocation[location_point][1];

        }
    }

}

void servomotor_angle(void)
{
    latitude_target = carlocation[location_point][0];
    longitude_target = carlocation[location_point][1];
    if(distance==-1)
    {

    }
    else
    {
        azimuth = get_two_points_azimuth(gnss.latitude, gnss.longitude, latitude_target, longitude_target);
    }
    //���㳵��ǰ�������Ŀ���֮��ļнǣ�gps_angle_error>0Ŀ����ʾ����ߣ���֮���ұ�
    gps_angle_error=angle-azimuth;
    if(gps_angle_error>180)
        gps_angle_error=gps_angle_error-360;
    if(gps_angle_error<-180)
        gps_angle_error=gps_angle_error+360;
}


