//*********************************************************
//	Node_List.cpp - extended node data class
//*********************************************************

#include "Node_List.hpp"

//---------------------------------------------------------
//	Node_List constructor
//---------------------------------------------------------

Node_List::Node_List (void) : Node_Data (0)
{
	Type (0);
	From_Dir (0);
	To_Dir (0);
}

//---------------------------------------------------------
//	Node_List_Array constructor
//---------------------------------------------------------

Node_List_Array::Node_List_Array (int max_records) : 
	Node_Array (max_records)
{
	Data_Size (sizeof (Node_List));
}

