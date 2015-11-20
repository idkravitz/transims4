//*********************************************************
//	Link_Time.hpp - link and time period data classes
//*********************************************************

#ifndef Link_Time_HPP
#define Link_Time_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Link_Time class definition
//---------------------------------------------------------

class Link_Time 
{
public:
	Link_Time (void);

	int  Dir_Index (void)               { return (dir_index); }
	int  Time_Period (void)             { return (time_period); }

	void Dir_Index (int value)          { dir_index = value; }
	void Time_Period (int value)        { time_period = value; }
	
private:
	int dir_index;
	int time_period;
};

//---------------------------------------------------------
//	Link_Time_Array class definition
//---------------------------------------------------------

class Link_Time_Array : public Data_Array
{
public:
	Link_Time_Array (void);

	bool Add (Link_Time *data = NULL)       { return (Data_Array::Add (data)); }
	
	Link_Time * New_Record (bool clear = false, int number = 1)
	                                        { return ((Link_Time *) Data_Array::New_Record (clear, number)); }

	Link_Time * Record (int index)          { return ((Link_Time *) Data_Array::Record (index)); }
	Link_Time * Record (void)               { return ((Link_Time *) Data_Array::Record ()); }

	Link_Time * First (void)                { return ((Link_Time *) Data_Array::First ()); }
	Link_Time * Next (void)                 { return ((Link_Time *) Data_Array::Next ()); }
	
	Link_Time * Last (void)                 { return ((Link_Time *) Data_Array::Last ()); }
	Link_Time * Previous (void)             { return ((Link_Time *) Data_Array::Previous ()); }

	Link_Time * operator[] (int index)      { return (Record (index)); }
};

#endif
