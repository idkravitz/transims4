//*********************************************************
//	Route_Node.cpp - route node list
//*********************************************************

#include "Route_Node.hpp"

//---------------------------------------------------------
//	Route_Node constructor
//---------------------------------------------------------

Route_Node::Route_Node (void)
{
	Node (0);
	Dwell (0);
	TTime (0);
	Dir (0);
}

//---------------------------------------------------------
//	Route_Node_Array constructor
//---------------------------------------------------------

Route_Node_Array::Route_Node_Array (int max_records) : 
	Data_Array (sizeof (Route_Node), max_records)
{
}
