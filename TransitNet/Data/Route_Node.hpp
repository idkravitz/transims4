//*********************************************************
//	Route_Node.hpp - route node data
//*********************************************************

#ifndef ROUTE_NODE_HPP
#define ROUTE_NODE_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Route_Node class definition
//---------------------------------------------------------

class Route_Node
{
public:
	Route_Node (void);

	int  Node (void)              { return (node); }
	int  Dwell (void)             { return (dwell); }
	int  TTime (void)             { return (ttime); }
	int  Dir (void)               { return (dir); }

	void Node (int value)         { node = value; }
	void Dwell (int value)        { dwell = value; }
	void TTime (int value)        { ttime = value; }
	void Dir (int value)          { dir = value; }

	void Add_Dwell (int value)    { dwell += value; }
	void Add_TTime (int value)    { ttime += value; }

private:
	int node;
	int dwell;
	int ttime;
	int dir;
};

//---------------------------------------------------------
//	Route_Node_Array class definition
//---------------------------------------------------------

class Route_Node_Array : public Data_Array
{
public:
	Route_Node_Array (int max_records = 0);
	virtual ~Route_Node_Array (void)       { Clear (); }

	bool Add (Route_Node *data = NULL)     { return (Data_Array::Add (data)); }
	
	Route_Node * New_Record (bool clear = false, int number = 1)
	                                       { return ((Route_Node *) Data_Array::New_Record (clear, number)); }

	Route_Node * Record (int index)        { return ((Route_Node *) Data_Array::Record (index)); }
	Route_Node * Record (void)             { return ((Route_Node *) Data_Array::Record ()); }

	Route_Node * First (void)              { return ((Route_Node *) Data_Array::First ()); }
	Route_Node * Next (void)               { return ((Route_Node *) Data_Array::Next ()); }
	
	Route_Node * Last (void)               { return ((Route_Node *) Data_Array::Last ()); }
	Route_Node * Previous (void)           { return ((Route_Node *) Data_Array::Previous ()); }

	Route_Node * operator[] (int index)    { return (Record (index)); }
};

#endif
