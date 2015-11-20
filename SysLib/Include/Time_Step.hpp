//*********************************************************
//	Time_Step.hpp - Time Step conversion utility
//*********************************************************

#ifndef TIME_STEP_HPP
#define TIME_STEP_HPP

#include "Static_Service.hpp"

//---------------------------------------------------------
//	Time_Step Class definition
//---------------------------------------------------------

class Time_Step: public Static_Service
{
public:
	enum Time_Type {SECONDS, CLOCK24, CLOCK12, HOURS, TIME_CODE};

	Time_Step (int steps_per_second, char *format);
	Time_Step (int steps_per_second = 1, Time_Type format = SECONDS);

	bool Initialize (int steps_per_second, char *format);

	int Step_Size (int steps_per_second);
	int Step_Size (void)					{ return (time_step); }

	bool Format (char *format);
	void Format (Time_Type format);

	Time_Type Format (void)					{ return (time_format); }

	double Seconds (int step)				{ return ((double) step / time_step); }

	int  Step (char *string)				{ return (Time_String (string, false)); }	
	int  Step (double hour)					{ return ((int) (hour * (3600 * time_step) + 0.5)); }
	int  Step (int second)					{ return (second * time_step); }

	int  Duration (char *string)			{ return (Time_String (string, true)); }
	int  Duration (double hour)				{ return (Step (hour)); }
	int  Duration (int second)				{ return (Step (second)); }

	char * Format_Step (int step, char *buffer = NULL);
	char * Format_Duration (int step, char *buffer = NULL);

    char * Format_Time (int second)			{ return (Format_Step (Step (second))); }
	char * Format_Time (double hour)		{ return (Format_Step (Step (hour))); }

	Time_Type Time_Code (char *text);
	char * Time_Code (Time_Type code);
	char * Time_Code (void)					{ return (Time_Code (time_format)); }

	int    Time_Label (char *string);
	char * Time_Label (int step, bool pad_flag = false);

private:
	int time_step;
	Time_Type time_format;
	bool pm_flag;

	int Time_String (char *string, bool duration);

	static char *time_text [];
	static Time_Type time_code [];

	static char buffer [2*TIME_BUFFER];
};
#endif
