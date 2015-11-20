//*********************************************************
//	Route_Stop_Data.hpp - transit route stop data classes
//*********************************************************

#ifndef ROUTE_STOP_DATA_HPP
#define ROUTE_STOP_DATA_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Route_Stop_Data class definition
//---------------------------------------------------------

class Route_Stop_Data
{
public:
	Route_Stop_Data (void);

	int  Route (void)            { return (route); }
	int  Stop (void)             { return (stop); }
	int  List (void)             { return (list); }
	
	void Route (int value)       { route = (short) value; }
	void Stop (int value)	     { stop = (short) value; }
	void List (int value)        { list = value; }

private:
	short route;
	short stop;
	int   list;
};

//---------------------------------------------------------
//	Route_Stop_Array
//---------------------------------------------------------

class Route_Stop_Array : public Data_Array
{
public:
	Route_Stop_Array (int max_records = 0);

	bool Add (Route_Stop_Data *data)          { return (Data_Array::Add ((void *) data)); }

	Route_Stop_Data * First (void)            { return ((Route_Stop_Data *) Data_Array::First ()); }
	Route_Stop_Data * Next (void)             { return ((Route_Stop_Data *) Data_Array::Next ()); }

	Route_Stop_Data * Last (void)             { return ((Route_Stop_Data *) Data_Array::Last ()); }
	Route_Stop_Data * Previous (void)         { return ((Route_Stop_Data *) Data_Array::Previous ()); }

	Route_Stop_Data * operator[] (int index)  { return ((Route_Stop_Data *) Record (index)); }
};

#endif
