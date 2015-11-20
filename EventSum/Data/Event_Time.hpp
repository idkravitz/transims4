//*********************************************************
//	Event_Time.hpp - Event Time Period data
//*********************************************************

#ifndef EVENT_TIME_HPP
#define EVENT_TIME_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Event_Time class definition
//---------------------------------------------------------

class Event_Time
{
public:
	Event_Time (void);

	int    Period (void)                 { return (period); }
	int    Trip_Start (void)             { return (trip_start); }
	int    Started (void)                { return (started); }
	int    Trip_End (void)               { return (trip_end); }
	int    Ended (void)                  { return (ended); }
	int    Mid_Trip (void)               { return (mid_trip); }
	int    Sum_Trips (void)              { return (sum_trip); }
	double Travel_Time (void)            { return (ttime); }
	double Start_Diff (void)             { return (start_diff); }
	double End_Diff (void)               { return (end_diff); }
	double TTime_Diff (void)             { return (ttime_diff); }
	double Start_Error (void)            { return (start_abs); }
	double End_Error (void)              { return (end_abs); }
	double TTime_Error (void)            { return (ttime_abs); }
	double Sum_Error (void)              { return (sum_error); }

	void   Period (int value)            { period = value; }
	void   Trip_Start (int value)        { trip_start = value; }
	void   Started (int value)           { started = value; }
	void   Trip_End (int value)          { trip_end = value; }
	void   Ended (int value)             { ended = value; }
	void   Mid_Trip (int value)          { mid_trip = value; }
	void   Sum_Trips (int value)         { sum_trip = value; }
	void   Travel_Time (double value)    { ttime = value; }
	void   Start_Diff (double value)     { start_diff = value; }
	void   End_Diff (double value)       { end_diff = value; }
	void   TTime_Diff (double value)     { ttime_diff = value; }
	void   Start_Error (double value)    { start_abs = value; }
	void   End_Error (double value)      { end_abs = value; }
	void   TTime_Error (double value)    { ttime_abs = value; }
	void   Sum_Error (double value)      { sum_error = value; }

	void   Add_Trip_Start (void)         { trip_start++; }
	void   Add_Start_Diff (int value)    { started++; start_diff += value; start_abs += abs (value); }
	void   Add_Trip_End (void)           { trip_end++; }
	void   Add_End_Diff (int value)      { ended++; end_diff += value; end_abs += abs (value); }
	void   Add_Travel_Time (int value, int diff) { mid_trip++; ttime += value; ttime_diff += diff; ttime_abs += abs (diff); }
	void   Add_Sum_Error (int value)     { sum_trip++; sum_error += value; }

private:
	int  period;
	int  trip_start;
	int  started;
	int  trip_end;
	int  ended;
	int  mid_trip;
	int  sum_trip;
	double ttime;
	double start_diff;
	double end_diff;
	double ttime_diff;
	double start_abs;
	double end_abs;
	double ttime_abs;
	double sum_error;
};

//---------------------------------------------------------
//	Event_Time_Array class definition
//---------------------------------------------------------

class Event_Time_Array : public Data_Array
{
public:
	Event_Time_Array (int max_records = 0);

	bool Add (Event_Time *data = NULL)     { return (Data_Array::Add (data)); }
	
	Event_Time * New_Record (bool clear = false, int number = 1)
	                                       { return ((Event_Time *) Data_Array::New_Record (clear, number)); }

	Event_Time * Record (int index)        { return ((Event_Time *) Data_Array::Record (index)); }
	Event_Time * Record (void)             { return ((Event_Time *) Data_Array::Record ()); }

	Event_Time * First (void)              { return ((Event_Time *) Data_Array::First ()); }
	Event_Time * Next (void)               { return ((Event_Time *) Data_Array::Next ()); }

	Event_Time * operator[] (int index)    { return (Record (index)); }
};

#endif
