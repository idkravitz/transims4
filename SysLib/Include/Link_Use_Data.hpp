//*********************************************************
//	Link_Use_Data.hpp - network link use data
//*********************************************************

#ifndef LINK_USE_DATA_HPP
#define LINK_USE_DATA_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Link_Use_Data class definition
//---------------------------------------------------------

class Link_Use_Data
{
public:
	Link_Use_Data (void);

	int  Use (void)                 { return (use); }
	int  Lanes (void)               { return (lane1); }
	int  HOV_Lanes (void)			{ return (lane2); }
	int  Start (void)               { return (start); }
	int  End (void)                 { return (end); }
	int  Offset (void)              { return (offset); }
	int  Length (void)              { return (length); }
	int  TOD_List (void)			{ return (tod_list); }
	int  TOD_Use (void)				{ return (tod_use); }
	
	void Use (int value)            { use = (unsigned short) value; }
	void Lanes (int value)          { lane1 = (unsigned char) value; }
	void HOV_Lanes (int value)		{ lane2 = (unsigned char) value; }
	void Start (int value)          { start = value; }
	void End (int value)            { end = value; }
	void Offset (int value)         { offset = value; }
	void Length (int value)         { length = value; }
	void TOD_List (int value)       { tod_list = value; }
	void TOD_Use (int value)		{ tod_use = value; }

	//---- lane-based overloaded methods ----

	int  Low_Lane (void)            { return (lane1); }
	int  High_Lane (void)           { return (lane2); }

	void Low_Lane (int value)       { lane1 = (unsigned char) value; }
	void High_Lane (int value)      { lane2 = (unsigned char) value; }

	int  Low_Cell (void)            { return (offset); }
	int  High_Cell (void)           { return (length); }

	void Low_Cell (int value)       { offset = value; }
	void High_Cell (int value)      { length = value; }

private:
	unsigned short use;
	unsigned char  lane1;			//---- num_lanes or low_lane ----
	unsigned char  lane2;			//---- hov_lanes or high_lane ----
	int            start;			//---- rounded ----
	int            end;				//---- rounded ----
	int            offset;			//---- rounded ----
	int            length;          //---- rounded ----
	int            tod_list;
	int            tod_use;
};

//---------------------------------------------------------
//	Link_Use_Array class definition
//---------------------------------------------------------

class Link_Use_Array : public Data_Array
{
public:
	Link_Use_Array (int max_records = 0);

	bool Add (Link_Use_Data *data = NULL)       { return (Data_Array::Add (data)); }

	Link_Use_Data * First (void)                { return ((Link_Use_Data *) Data_Array::First ()); }
	Link_Use_Data * Next (void)                 { return ((Link_Use_Data *) Data_Array::Next ()); }

	Link_Use_Data * operator[] (int index)      { return ((Link_Use_Data *) Record (index)); }
};

#endif
