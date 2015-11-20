//*********************************************************
//	Bin_Sum_Data.hpp - speed bin summary data classes
//*********************************************************

#ifndef BIN_SUM_DATA_HPP
#define BIN_SUM_DATA_HPP

#include "Class_Array.hpp"

//---------------------------------------------------------
//	Bin_Sum_Data class definition
//---------------------------------------------------------

class Bin_Sum_Data : public Class_Index
{
public:
	Bin_Sum_Data (void);

	int    Vehicle_Type (void)                    { return (ID () >> 24); }
	int    Road_Type (void)                       { return ((ID () & 0x00FF0000) >> 16); }
	int    Time_Bin (void)                        { return ((ID () & 0x0000FF00) >> 8); }
	int    Speed_Bin (void)                       { return (ID () & 0x000000FF); }

	double Meters (void)                          { return (meters); }
	double Seconds (void)                         { return (seconds); }

	void   Vehicle_Type (int value)               { ID ((ID () & 0x00FFFFFF) + (value << 24)); }
	void   Road_Type (int value)                  { ID ((ID () & 0xFF00FFFF) + (value << 16)); }
	void   Time_Bin (int value)                   { ID ((ID () & 0xFFFF00FF) + (value << 8)); }
	void   Speed_Bin (int value)                  { ID ((ID () & 0xFFFFFF00) + value); }    

	void   Meters (double value)                  { meters = value; }
	void   Seconds (double value)                 { seconds = value; }

	void   Add_Meters (double value)              { meters += value; }
	void   Add_Seconds (double value)             { seconds += value; }

	void   Sum (Bin_Sum_Data *data);
	void   Zero (Bin_Sum_Data *data = NULL);

private:
	double meters, seconds;
};

//---------------------------------------------------------
//	Bin_Sum_Array class definition
//---------------------------------------------------------

class Bin_Sum_Array : public Class_Array
{
public:
	Bin_Sum_Array (int max_records = 0);

	bool Add (Bin_Sum_Data *data = NULL)    { return (Class_Array::Add (data)); }
	
	Bin_Sum_Data * New_Record (bool clear = false, int number = 1)
	                                        { return ((Bin_Sum_Data *) Class_Array::New_Record (clear, number)); }

	Bin_Sum_Data * Record (int index)       { return ((Bin_Sum_Data *) Class_Array::Record (index)); }
	Bin_Sum_Data * Record (void)            { return ((Bin_Sum_Data *) Class_Array::Record ()); }

	Bin_Sum_Data * Get (int value)          { return ((Bin_Sum_Data *) Class_Array::Get (value)); }
	Bin_Sum_Data * Get (Bin_Sum_Data *data) { return ((Bin_Sum_Data *) Class_Array::Get (data)); }

	Bin_Sum_Data * First (void)             { return ((Bin_Sum_Data *) Class_Array::First ()); }
	Bin_Sum_Data * Next (void)              { return ((Bin_Sum_Data *) Class_Array::Next ()); }
	
	Bin_Sum_Data * First_Key (void)         { return ((Bin_Sum_Data *) Class_Array::First_Key ()); }
	Bin_Sum_Data * Next_Key (void)          { return ((Bin_Sum_Data *) Class_Array::Next_Key ()); }

	Bin_Sum_Data * operator[] (int index)   { return (Record (index)); }
};

#endif
