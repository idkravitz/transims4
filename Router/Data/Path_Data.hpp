//*********************************************************
//	Path_Data.hpp - path data classes
//*********************************************************

#ifndef PATH_DATA_HPP
#define PATH_DATA_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Path_Data class definition
//---------------------------------------------------------

class Path_Data
{
public:
	Path_Data (void);

	unsigned Cum (void)         { return (cum); }
	int  TOD (void)             { return (tod); }
	int  Cost (void)            { return (cost); }
	int  Time (void)            { return (time); }
	int  Prev (void)            { return (prev); }
	int  Next (void)            { return (next); }
	int  Type (void)            { return (type); }
	int  Mode (void)            { return (mode); }
	int  Xfer (void)            { return (layer >> 4); }
	int  Layer (void)           { return (layer & 0x0F); }
	int  Path (void)            { return (path); }
	
	void Cum (unsigned value)   { cum = value; }
	void TOD (int value)	    { tod = value; }
	void Cost (int value)       { cost = value; }
	void Time (int value)       { time = value; }
	void Prev (int value)       { prev = value; }
	void Next (int value)       { next = value; }
	void Type (int value)       { type = (char) value; }
	void Mode (int value)       { mode = (char) value; }
	void Xfer (int value)       { layer = (char) ((layer & 0x0F) + (value << 4)); }
	void Layer (int value)      { layer = (char) ((layer & 0xF0) + value); }
	void Path (int value)       { path = (char) value; }

private:
	unsigned cum;
	int tod;
	int cost;
	int time;
	int prev;
	int next;
	char type;
	char mode;
	char layer;
	char path;
};

//---------------------------------------------------------
//	Path_Array
//---------------------------------------------------------

class Path_Array : public Data_Array
{
public:
	Path_Array (int max_records = 0);

	bool Add (Path_Data *data)          { return (Data_Array::Add ((void *) data)); }

	Path_Data * First (void)            { return ((Path_Data *) Data_Array::First ()); }
	Path_Data * Next (void)             { return ((Path_Data *) Data_Array::Next ()); }

	Path_Data * Last (void)             { return ((Path_Data *) Data_Array::Last ()); }
	Path_Data * Previous (void)         { return ((Path_Data *) Data_Array::Previous ()); }

	Path_Data * operator[] (int index)  { return ((Path_Data *) Record (index)); }
};

#endif
