//*********************************************************
//	Date_Time.hpp - date-time formating functions
//*********************************************************

#ifndef DATE_TIME_HPP
#define DATE_TIME_HPP

#include "System_Defines.hpp"

#include <time.h>

struct tm *local_time (time_t *tim);
char * c_time (time_t *tim);

int    Day_of_Week (time_t tim);
double Hour_of_Day (time_t tim);
char * Day_Time_String (double gps_time);
char * Time_String (time_t tim);
char * Date_String (time_t tim);
char * Date_Time_String (time_t tim);
void   Convert_Day_Time (double time, int *day, int *hour, int *minute, double *second);
double Make_Day_Time (int day, int hour, int minute, double second);
double Convert_Day_Time_String (char *string);
time_t Convert_Time_String (char *string);

#endif
