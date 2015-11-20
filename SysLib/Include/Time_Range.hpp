//*********************************************************
//	Time_Range.hpp - time of day range class
//*********************************************************

#ifndef TIME_RANGE_HPP
#define TIME_RANGE_HPP

#include "Range_Data.hpp"
#include "Time_Step.hpp"
#include "System_Defines.hpp"

//---------------------------------------------------------
//	Time_Range Class definition
//---------------------------------------------------------

class Time_Range : public Range_Array, public Time_Step
{
public:
	Time_Range (int steps_per_second, char *format);
	Time_Range (int steps_per_second = 1, Time_Type format = SECONDS);
	
	int  In_Increment (int step)		{ return ((increment > 0) ? step / increment + 1 : 1); }
	bool At_Increment (int step)		{ return ((increment > 0) ? ((step % increment) == 0) : false); }

	int  Period (int step)				{ return (In_Index (step)); }

	int  Increment (void)				{ return (increment); }

	void Increment (char *string)		{ increment = Duration (string); }
	void Increment (double hour)		{ increment = Duration (hour); }
	void Increment (int second)			{ increment = Duration (second); }

	bool Period_Flag (void)				{ return (period_flag); }
	void Period_Flag (bool flag)		{ period_flag = flag; }

	bool Add_Label (char *label);
	bool Add_Ranges (char *ranges);
	bool Add_Breaks (char *breaks);

	char *Range_Format (int num);
	char *Range_Label (int num, bool pad_flag = false);

private:
	int increment;
	bool period_flag;

	static char buffer [FIELD_BUFFER];
};

#endif
