//*********************************************************
//	Path_Node.cpp - path node list
//*********************************************************

#include "Path_Node.hpp"

//---------------------------------------------------------
//	Path_Node constructor
//---------------------------------------------------------

Path_Node::Path_Node (void)
{
	Node (0);
	Dir (0);
}

//---------------------------------------------------------
//	Path_Node_Array constructor
//---------------------------------------------------------

Path_Node_Array::Path_Node_Array (int max_records) : 
	Data_Array (sizeof (Path_Node), max_records)
{
}
