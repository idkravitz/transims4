//*********************************************************
//	Trip_End_Data.hpp - trip end data classes
//*********************************************************

#ifndef TRIP_END_DATA_HPP
#define TRIP_END_DATA_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Trip_End_Data class definition
//---------------------------------------------------------

class Trip_End_Data
{
public:
	Trip_End_Data (void);

	int  Rec (void)             { return (rec); }
	int  ID (void)              { return (id); }
	int  Type (void)            { return (type); }
	int  Link (void)            { return (link); }
	int  Offset (void)          { return (offset); }
	unsigned  Cum (void)        { return (cum); }
	int  TOD (void)             { return (tod); }
	int  Cost (void)            { return (cost); }
	int  Time (void)            { return (time); }
	int  Prev (void)            { return (prev); }
	int  Layer (void)           { return (layer); }
	int  Path (void)            { return (path); }

	void Rec (int value)        { rec = value; }
	void ID (int value)         { id = value; }
	void Type (int value)       { type = value; }
	void Link (int value)       { link = value; }
	void Offset (int value)     { offset = value; }
	void Cum (unsigned value)   { cum = value; }
	void TOD (int value)	    { tod = value; }
	void Cost (int value)       { cost = value; }
	void Time (int value)       { time = value; }
	void Prev (int value)       { prev = value; }
	void Layer (int value)      { layer = value; }
	void Path (int value)       { path = value; }

private:
	int rec;
	int id;
	int type;
	int link;
	int offset;
	unsigned cum;
	int tod;
	int cost;
	int time;
	int prev;
	int layer;
	int path;
};

//---------------------------------------------------------
//	Trip_End_Array
//---------------------------------------------------------

class Trip_End_Array : public Data_Array
{
public:
	Trip_End_Array (int max_records = 0);

	bool Add (Trip_End_Data *data)          { return (Data_Array::Add ((void *) data)); }

	Trip_End_Data * First (void)            { return ((Trip_End_Data *) Data_Array::First ()); }
	Trip_End_Data * Next (void)             { return ((Trip_End_Data *) Data_Array::Next ()); }

	Trip_End_Data * Last (void)             { return ((Trip_End_Data *) Data_Array::Last ()); }
	Trip_End_Data * Previous (void)         { return ((Trip_End_Data *) Data_Array::Previous ()); }

	Trip_End_Data * operator[] (int index)  { return ((Trip_End_Data *) Record (index)); }
};

#endif
