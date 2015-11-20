//********************************************************* 
//	Date_Time.cpp - date and time functions
//*********************************************************

#include "Date_Time.hpp"

#include "Utility.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char *day_text [] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
#ifdef _MSC_EXTENSIONS
struct tm dt;
#endif

//---------------------------------------------------------
//	local_time - safe local time function
//---------------------------------------------------------

struct tm *local_time (time_t *tim)
{
#ifdef _MSC_EXTENSIONS
	localtime_s (&dt, tim);
	return (&dt);
#else
	return (localtime (tim));
#endif
}

//---------------------------------------------------------
//	c_time - safe time string function
//---------------------------------------------------------

char * c_time (time_t *tim)
{
	static char buffer [40];
#ifdef _MSC_EXTENSIONS
	ctime_s (buffer, sizeof (buffer), tim);
	buffer [24] = '\0';
#else
	sprintf (buffer, "%.24s", ctime (tim));
#endif
	return (buffer);
}

//----------------------------------------------------------
//	Day_of_Week
//----------------------------------------------------------

int Day_of_Week (time_t tim)
{
	struct tm *dt = local_time (&tim);

	return (dt->tm_wday);
}

//----------------------------------------------------------
//	Hour_of_Day
//----------------------------------------------------------

double Hour_of_Day (time_t tim)
{
	struct tm *dt = local_time (&tim);

	double hour = dt->tm_hour + (dt->tm_min / 60.0) + (dt->tm_sec / 3600.0);

	return (hour);
}

//---------------------------------------------------------
//	Day_Time_String
//---------------------------------------------------------

char * Day_Time_String (double day_time)
{
	int day, hour, minute;
	double second;
	static char buffer [40];
		
	Convert_Day_Time (day_time, &day, &hour, &minute, &second);

	return (str_fmt (buffer, sizeof (buffer), "%3.3s %2d:%02d:%04.1lf", day_text [day], hour, minute, second));
}

//---------------------------------------------------------
//	Time_String
//---------------------------------------------------------

char * Time_String (time_t tim)
{
	static char buffer [TIME_BUFFER];

	struct tm *dt = local_time (&tim);
	
	return (str_fmt (buffer, sizeof (buffer), "%2d:%02d:%02d", dt->tm_hour, dt->tm_min, dt->tm_sec));
}

//---------------------------------------------------------
//	Date_String
//---------------------------------------------------------

char * Date_String (time_t tim)
{
	static char buffer [TIME_BUFFER];

	struct tm *dt = local_time (&tim);
	
	return (str_fmt (buffer, sizeof (buffer), "%2d/%02d/%04d", dt->tm_mon+1, dt->tm_mday, dt->tm_year+1900));
}

//---------------------------------------------------------
//	Date_Time_String
//---------------------------------------------------------

char * Date_Time_String (time_t tim)
{
	static char buffer [40];

	struct tm *dt = local_time (&tim);
	
	return (str_fmt (buffer, sizeof (buffer), "%3.3s %2d/%02d/%04d %2d:%02d:%02d", 
		day_text [dt->tm_wday], 
		dt->tm_mon+1, dt->tm_mday, dt->tm_year+1900,
		dt->tm_hour, dt->tm_min, dt->tm_sec));
}

//---------------------------------------------------------
//	Convert_Day_Time - converts Day Time to time components
//---------------------------------------------------------

void  Convert_Day_Time (double time, int *day, int *hour, int *minute, double *second)
{
	*second = time;
	*day = (int) (*second / 86400);
	*second -= (*day * 86400);
	*hour = (int) (*second / 3600);
	*second -= (*hour * 3600);
	*minute = (int) (*second / 60.0);
	*second -= (*minute * 60.0);
}

//---------------------------------------------------------
//	Make_Day_Time - converts time componenets to Day Time
//---------------------------------------------------------

double  Make_Day_Time (int day, int hour, int minute, double second)
{
	second += minute * 60.0 + hour * 3600 + day * 86400; 
	if (second < 0.0) {
		second += 604800;		//---- seconds per week ----
	} else if (second > 604800) {
		second -= 604800;
	}
	return (second);
}

//---------------------------------------------------------
//	Convert_Day_Time_String - converts a string to Day Time
//---------------------------------------------------------

double Convert_Day_Time_String (char *string)
{
	//---- day hh:mm:ss.s -----
	//---- day hh:mm:ss -----
	//---- day hh:mm -----
	//---- hh:mm:ss.s -----
	//---- hh:mm:ss -----
	//---- hh:mm -----

	int day, hour, minute;
	double second;
	char *begin, *end, buffer [40];

	str_cpy (buffer, sizeof (buffer), string);

	begin = buffer;
	while (*begin == ' ' || *begin == '\t') begin++;
	
	end = begin;

	//---- day of the week ----

	if (!isdigit (*begin)) {
		while (*end != ' ' && *end != '\0') end++;
        *end = '\0';

		for (day=0; day < 7; day++) {
			if (str_cmp (day_text [day], begin, strlen (day_text [day])) == 0) break;
		}
		if (day == 7) goto time_error;
		begin = end + 1;
		
		while (*begin == ' ') begin++;

		if (!isdigit (*begin)) goto time_error;
		end = begin;
	} else {
		day = 0;
	}

	//---- hour of the day ----
	
	while (*end != ':' && *end != ' ' && *end != '\0') end++;
	*end = '\0';

	hour = atoi (begin);
	if (hour < 0 || hour > 23) goto time_error;

	begin = end + 1;
		
	while (*begin == ' ') begin++;

	if (!isdigit (*begin)) goto time_error;
	end = begin;

	//---- minute of the hour ----

	while (*end != ':' && *end != ' ' && *end != '\0') end++;
	*end = '\0';

	minute = atoi (begin);
	if (minute < 0 || minute > 59) goto time_error;

	begin = end + 1;
		
	while (*begin == ' ') begin++;

	//---- second of the minute ----
	
	if (!isdigit (*begin)) {
		second = 0.0;
	} else {
		end = begin;
	
		while (*end != ' ' && *end != '\0') end++;
		*end = '\0';

		second = atof (begin);
		if (second < 0.0 || second >= 60.0) goto time_error;
	}
	return (Make_Day_Time (day, hour, minute, second));

time_error:
	return (0.0);
}

//---------------------------------------------------------
//	Convert_Time_String - converts a string to a time stamp
//---------------------------------------------------------

time_t Convert_Time_String (char *string)
{
	//---- day mm/dd/yyyy hh:mm:ss ----
	//---- mm/dd/yyyy hh:mm:ss ----
	//---- mm/dd/yyyy hh:mm ----
	//---- mm/dd/yyyy ----

	//---- day mm/dd/yy hh:mm:ss ----
	//---- mm/dd/yy hh:mm:ss ----
	//---- mm/dd/yy hh:mm ----
	//---- mm/dd/yy ----

	//---- day hh:mm:ss ----
	//---- hh:mm:ss ----
	//---- hh:mm ----

	int i;
	char *begin, *end, buffer [40];
	struct tm *dt;
	time_t tvalue;

	time (&tvalue);
	dt = local_time (&tvalue);

	str_cpy (buffer, sizeof (buffer), string);

	begin = buffer;
	while (*begin == ' ' || *begin == '\t') begin++;

	if (*begin == '\0') {
		return ((time_t) 0);
	}
	end = begin;

	//---- day of the week ----

	if (!isdigit (*begin)) {
		while (*end != ' ' && *end != '\0') end++;
		*end = '\0';

		for (i=0; i < 7; i++) {
			if (str_cmp (day_text [i], begin, strlen (day_text [i])) == 0) break;
		}
		if (i == 7) goto time_error;
		begin = end + 1;
		
		while (*begin == ' ') begin++;

		if (!isdigit (*begin)) goto time_error;
		end = begin;
	}

	//---- month or hour ----
	
	while (*end != '/' && *end != ':' && *end != ' ' && *end != '\0') end++;

	if (*end != ':' && *end != '/') {
		return ((time_t) (atoi (begin)));
	}

	if (*end != ':') {

		//---- month ----
		
		*end = '\0';
		
		i = atoi (begin);
		if (i < 1 || i > 12) goto time_error;
		dt->tm_mon = i - 1;

		begin = end + 1;
		
		while (*begin == ' ') begin++;
		end = begin;

		//---- day of the month ----

		while (*end != '/' && *end != ' ' && *end != '\0') end++;
		*end = '\0';

		i = atoi (begin);
		if (i < 1 || i > 31) goto time_error;
		dt->tm_mday = i;

		begin = end + 1;
		
		while (*begin == ' ') begin++;

		if (!isdigit (*begin)) goto time_error;

		//---- year ----
	
		end = begin;
		
		while (*end != ' ' && *end != '\0') end++;
		*end = '\0';

		i = atoi (begin);
		if (i < 0 || i > 2100) goto time_error;
		if (i < 70) {
			i += 2000;
		} else if (i < 100) {
			i += 1900;
		}
		dt->tm_year = i - 1900;

		begin = end + 1;

		while (*begin == ' ') begin++;

		if (!isdigit (*begin)) {
			return (mktime (dt));
		}
		end = begin;
		
		while (*end != ':' && *end != ' ' && *end != '\0') end++;
	}

	//---- hour of the day ----

	*end = '\0';

	if (*begin != '\0') {
		i = atoi (begin);
		if (i < 0 || i > 23) goto time_error;
		dt->tm_hour = i;
	}

	begin = end + 1;
		
	while (*begin == ' ') begin++;

	if (!isdigit (*begin)) goto time_error;
	end = begin;

	//---- minute of the hour ----


	while (*end != ':' && *end != ' ' && *end != '\0') end++;
	*end = '\0';

	if (*begin != '\0') {
		i = atoi (begin);
		if (i < 0 || i > 59) goto time_error;
		dt->tm_min = i;
	}

	begin = end + 1;
		
	while (*begin == ' ') begin++;

	//---- second of the minute ----
	
	if (isdigit (*begin)) {
		end = begin;
	
		while (*end != ' ' && *end != '\0') end++;
		*end = '\0';

		if (*begin != '\0') {
			i = atoi (begin);
			if (i < 0 || i > 59) goto time_error;
			dt->tm_sec = i;
		}
	}

time_error:
	tvalue = mktime (dt);

	return (tvalue);
}
