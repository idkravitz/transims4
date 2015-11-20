//*********************************************************
//	Node_Data.cpp - network node data classes
//*********************************************************

#include "Node_Data.hpp"

//---------------------------------------------------------
//	Node_Data constructor
//---------------------------------------------------------

Node_Data::Node_Data (int node) : Class_Index (node)
{
	X (0);
	Y (0);
	Z (0);
}

//---------------------------------------------------------
//	Node_Array constructor
//---------------------------------------------------------

Node_Array::Node_Array (int max_records) : 
	Class_Array (sizeof (Node_Data), max_records, true)
{
}
