//*********************************************************
//	Route_Stop.hpp - route stop data
//*********************************************************

#ifndef ROUTE_STOP_HPP
#define ROUTE_STOP_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Route_Stop class definition
//---------------------------------------------------------

class Route_Stop
{
public:
	Route_Stop (void);

	int  Stop (void)              { return (stop); }
	int  Time (void)              { return (time); }
	int  Zone (void)              { return (zone); }

	void Stop (int value)         { stop = value; }
	void Time (int value)         { time = value; }
	void Zone (int value)         { zone = value; }

	void Add_Time (int value)     { time += value; }

private:
	int stop;
	int time;
	int zone;
};

//---------------------------------------------------------
//	Route_Stop_Array class definition
//---------------------------------------------------------

class Route_Stop_Array : public Data_Array
{
public:
	Route_Stop_Array (int max_records = 0);
	virtual ~Route_Stop_Array (void)       { Clear (); }

	bool Add (Route_Stop *data = NULL)     { return (Data_Array::Add (data)); }
	
	Route_Stop * New_Record (bool clear = false, int number = 1)
	                                       { return ((Route_Stop *) Data_Array::New_Record (clear, number)); }

	Route_Stop * Record (int index)        { return ((Route_Stop *) Data_Array::Record (index)); }
	Route_Stop * Record (void)             { return ((Route_Stop *) Data_Array::Record ()); }

	Route_Stop * First (void)              { return ((Route_Stop *) Data_Array::First ()); }
	Route_Stop * Next (void)               { return ((Route_Stop *) Data_Array::Next ()); }
	
	Route_Stop * Last (void)               { return ((Route_Stop *) Data_Array::Last ()); }
	Route_Stop * Previous (void)           { return ((Route_Stop *) Data_Array::Previous ()); }

	Route_Stop * operator[] (int index)    { return (Record (index)); }
};

#endif
