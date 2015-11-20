//*********************************************************
//	Group_Link.hpp - directional network link class
//*********************************************************

#ifndef GROUP_LINK_HPP
#define GROUP_LINK_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Group_Link class definition
//---------------------------------------------------------

class Group_Link
{
public:
	Group_Link (void);

	int  Node (void)                { return (node); }
	int  Signal (void)              { return (signal); }
	int  Offset (void)              { return (offset); }
	int  Green (void)               { return (green); }
	int  TTime (void)               { return (ttime); }
	int  Length (void)              { return (length); }
	int  VHT (void)                 { return (vht); }
	
	void Node (int value)           { node = value; }
	void Signal (int value)         { signal = value; }
	void Offset (int value)         { offset = value; }
	void Green (int value)          { green = value; }
	void TTime (int value)          { ttime = value; }
	void Length (int value)         { length = value; }
	void VHT (int value)            { vht = value; }

private:
	int   node;
	int   signal;
	int   offset;
	int   green;
	int   ttime;
	int   length;
	int   vht;
};

//---------------------------------------------------------
//	Group_Link_Array class definition
//---------------------------------------------------------

class Group_Link_Array : public Data_Array
{
public:
	Group_Link_Array (int max_records = 0);
	virtual ~Group_Link_Array (void)        { Clear (); }

	bool Add (Group_Link *data = NULL)      { return (Data_Array::Add (data)); }
	
	Group_Link * New_Record (bool clear = false, int number = 1)
	                                        { return ((Group_Link *) Data_Array::New_Record (clear, number)); }

	Group_Link * Record (int index)         { return ((Group_Link *) Data_Array::Record (index)); }
	Group_Link * Record (void)              { return ((Group_Link *) Data_Array::Record ()); }

	Group_Link * First (void)               { return ((Group_Link *) Data_Array::First ()); }
	Group_Link * Next (void)                { return ((Group_Link *) Data_Array::Next ()); }

	Group_Link * operator[] (int index)     { return (Record (index)); }
};
#endif

