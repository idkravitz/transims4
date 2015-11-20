//*********************************************************
//	Travel_Leg_Data.hpp - travel leg data classes
//*********************************************************

#ifndef TRAVEL_LEG_DATA_HPP
#define TRAVEL_LEG_DATA_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Travel_Leg_Data class definition
//---------------------------------------------------------

class Travel_Leg_Data
{
public:
	Travel_Leg_Data (void);

	int  Dir (void)                  { return (dir); }
	int  In_Lane_Low (void)          { return (in_lane_low); }
	int  In_Lane_High (void)         { return (in_lane_high); }
	int  In_Best_Low (void)          { return (in_best_low); }
	int  In_Best_High (void)         { return (in_best_high); }
	int  Out_Lane_Low (void)         { return (out_lane_low); }
	int  Out_Lane_High (void)        { return (out_lane_high); }
	int  Out_Best_Low (void)         { return (out_best_low); }
	int  Out_Best_High (void)        { return (out_best_high); }
	int  Max_Speed (void)            { return (max_speed); }
	int  Stop_Number (void)          { return (stop_num); }
	int  Control (void)              { return (control); }
	int  Connect (void)              { return (connect); }

	void Dir (int value)             { dir = value; }
	void In_Lane_Low (int value)     { in_lane_low = (char) value; }
	void In_Lane_High (int value)    { in_lane_high = (char) value; }
	void In_Best_Low (int value)     { in_best_low = (char) value; }
	void In_Best_High (int value)    { in_best_high = (char) value; }
	void Out_Lane_Low (int value)    { out_lane_low = (char) value; }
	void Out_Lane_High (int value)   { out_lane_high = (char) value; }
	void Out_Best_Low (int value)    { out_best_low = (char) value; }
	void Out_Best_High (int value)   { out_best_high = (char) value; }
	void Max_Speed (int value)       { max_speed = (short) value; }
	void Stop_Number (int value)     { stop_num = (short) value; }
	void Control (int value)         { control = value; }
	void Connect (int value)         { connect = value; }

private:
	int   dir;				//---- dir_data index ----
	char  in_lane_low;
	char  in_lane_high;
	char  in_best_low;
	char  in_best_high;
	char  out_lane_low;
	char  out_lane_high;
	char  out_best_low;
	char  out_best_high;
	short max_speed;		//---- rounded ----
	short stop_num;			//---- line data stop number ----
	int   control;			//---- control_data index ----
	int   connect;			//---- connect_data index ---- 
};

//---------------------------------------------------------
//	Travel_Leg_Array
//---------------------------------------------------------

class Travel_Leg_Array : public Data_Array
{
public:
	Travel_Leg_Array (int max_records = 0);

	bool Add (Travel_Leg_Data *data)			{ return (Data_Array::Add ((void *) data)); }

	Travel_Leg_Data * Record (int index)        { return ((Travel_Leg_Data *) Data_Array::Record (index)); }
	Travel_Leg_Data * Record (void)             { return ((Travel_Leg_Data *) Data_Array::Record ()); }

	Travel_Leg_Data * First (void)				{ return ((Travel_Leg_Data *) Data_Array::First ()); }
	Travel_Leg_Data * Next (void)				{ return ((Travel_Leg_Data *) Data_Array::Next ()); }

	Travel_Leg_Data * Last (void)				{ return ((Travel_Leg_Data *) Data_Array::Last ()); }
	Travel_Leg_Data * Previous (void)			{ return ((Travel_Leg_Data *) Data_Array::Previous ()); }

	Travel_Leg_Data * operator[] (int index)	{ return (Record (index)); }
};

#endif
