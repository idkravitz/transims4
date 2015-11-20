//*********************************************************
//	TOD_Turn_Data.hpp - turn restriction data classes
//*********************************************************

#ifndef TOD_TURN_DATA_HPP
#define TOD_TURN_DATA_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	TOD_Turn_Data class definition
//---------------------------------------------------------

class TOD_Turn_Data
{
public:
	TOD_Turn_Data (void);

	int  Start (void)            { return (start); }
	int  End (void)              { return (end); }
	int  Use (void)              { return (use); }
	int  Penalty (void)          { return (penalty); }
	int  TOD_List (void)         { return (tod_list); }
	
	void Start (int value)       { start = value; }
	void End (int value)	     { end = value; }
	void Use (int value)         { use = (unsigned short) value; }
	void Penalty (int value)     { penalty = (unsigned short) value; }
	void TOD_List (int value)    { tod_list = value; }

private:
	int start;
	int end;
	unsigned short use;
	unsigned short penalty;
	int tod_list;
};

//---------------------------------------------------------
//	TOD_Turn_Array
//---------------------------------------------------------

class TOD_Turn_Array : public Data_Array
{
public:
	TOD_Turn_Array (int max_records = 0);

	bool Add (TOD_Turn_Data *data)          { return (Data_Array::Add ((void *) data)); }

	TOD_Turn_Data * First (void)            { return ((TOD_Turn_Data *) Data_Array::First ()); }
	TOD_Turn_Data * Next (void)             { return ((TOD_Turn_Data *) Data_Array::Next ()); }

	TOD_Turn_Data * Last (void)             { return ((TOD_Turn_Data *) Data_Array::Last ()); }
	TOD_Turn_Data * Previous (void)         { return ((TOD_Turn_Data *) Data_Array::Previous ()); }

	TOD_Turn_Data * operator[] (int index)  { return ((TOD_Turn_Data *) Record (index)); }
};

#endif
