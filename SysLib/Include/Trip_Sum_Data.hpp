//*********************************************************
//	Trip_Sum_Data.hpp - trip summary data class
//*********************************************************

#ifndef TRIP_SUM_DATA_HPP
#define TRIP_SUM_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"
#include "Static_Service.hpp"

//---------------------------------------------------------
//	Trip_Sum_Data class definition
//---------------------------------------------------------

class Trip_Sum_Data : public Class_Index
{
public:
	Trip_Sum_Data (int group = 0);

	int    Group (void)                { return (ID ()); }
	int    Count (void)                { return (count); }
	double Distance (void)             { return (distance); }
	double Time (void)                 { return (time); }
	double Speed (void)                { return (speed); }
	double Turns (void)                { return (turns); }
	double Distance_Sq (void)          { return (distance2); }
	double Time_Sq (void)              { return (time2); }
	double Speed_Sq (void)             { return (speed2); }
	double Turn_Sq (void)              { return (turns2); }
	double Min_Distance (void)         { return (min_distance); }
	double Max_Distance (void)         { return (max_distance); }
	double Min_Time (void)             { return (min_time); }
	double Max_Time (void)             { return (max_time); }
	double Min_Speed (void)            { return (min_speed); }
	double Max_Speed (void)            { return (max_speed); }
	int    Min_Turns (void)            { return (min_turns); }
	int    Max_Turns (void)            { return (max_turns); }

	void   Group (int value)           { ID (value); }
	void   Count (int value)           { count = value; }
	void   Distance (double value)     { distance = value; }
	void   Time (double value)         { time = value; }
	void   Speed (double value)        { speed = value; }
	void   Turns (double value)        { turns = value; }
	void   Distance_Sq (double value)  { distance2 = value; }
	void   Time_Sq (double value)      { time2 = value; }
	void   Speed_Sq (double value)     { speed2 = value; }
	void   Turns_Sq (double value)     { turns2 = value; }
	void   Min_Distance (double value) { min_distance = value; }
	void   Max_Distance (double value) { max_distance = value; }
	void   Min_Time (double value)     { min_time = value; }
	void   Max_Time (double value)     { max_time = value; }
	void   Min_Speed (double value)    { min_speed = value; }
	void   Max_Speed (double value)    { max_speed = value; }
	void   Min_Turns (int value)       { min_turns = value; }
	void   Max_Turns (int value)       { max_turns = value; }

	void   Add_Trip (double distance, double time, int turns = 0, int count = 1);

	double Average_Distance (void)    { return ((count > 0) ? distance / count : 0.0); }
	double Average_Time (void)        { return ((count > 0) ? time / count : 0.0); }
	double Average_Speed (void)       { return ((count > 0) ? speed / count : 0.0); }
	double Average_Turns (void)       { return ((count > 0) ? turns / count : 0.0); }

	double StdDev_Distance (void);
	double StdDev_Time (void);
	double StdDev_Speed (void);
	double StdDev_Turns (void);

private:
	int count;
	double distance;
	double distance2;
	double min_distance;
	double max_distance;
	double time;
	double time2;
	double min_time;
	double max_time;
	double speed;
	double speed2;
	double min_speed;
	double max_speed;
	double turns;
	double turns2;
	int    min_turns;
	int    max_turns;
};

//---------------------------------------------------------
//	Trip_Sum_Array class definition
//---------------------------------------------------------

class Trip_Sum_Array : public Class_Array, public Static_Service
{
public:
	Trip_Sum_Array (int max_records = 20);
	virtual ~Trip_Sum_Array (void);

	bool Add (Trip_Sum_Data *data = NULL)     { return (Class_Array::Add (data)); }
	
	Trip_Sum_Data * New_Record (bool clear = false, int number = 1)
	                                          { return ((Trip_Sum_Data *) Class_Array::New_Record (clear, number)); }

	Trip_Sum_Data * Get (int id)              { return ((Trip_Sum_Data *) Class_Array::Get (id)); }

	Trip_Sum_Data * Record (int index)        { return ((Trip_Sum_Data *) Class_Array::Record (index)); }
	Trip_Sum_Data * Record (void)             { return ((Trip_Sum_Data *) Class_Array::Record ()); }

	Trip_Sum_Data * First (void)              { return ((Trip_Sum_Data *) Class_Array::First ()); }
	Trip_Sum_Data * Next (void)               { return ((Trip_Sum_Data *) Class_Array::Next ()); }
	
	Trip_Sum_Data * First_Key (void)          { return ((Trip_Sum_Data *) Class_Array::First_Key ()); }
	Trip_Sum_Data * Next_Key (void)           { return ((Trip_Sum_Data *) Class_Array::Next_Key ()); }

	Trip_Sum_Data * operator[] (int index)    { return (Record (index)); }

	//---- standard processing methods -----

	bool Add_Trip (int group, double distance, double time, int turns = 0, int count = 1);
	void Report (int number, char *title, char *key1, char *key2 = NULL);
	void Header ();

private:
	int keys;
	char *title, *key1, *key2;
};

#endif
