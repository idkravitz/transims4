//*********************************************************
//	Node_Path_Data.cpp - node data class for path building
//*********************************************************

#include "Node_Path_Data.hpp"

//---------------------------------------------------------
//	Node_Path_Data constructor
//---------------------------------------------------------

Node_Path_Data::Node_Path_Data (void) : Node_Data (0)
{
	From_Dir (0);
	To_Dir (0);
	Distance (0);
	Next_List (0);
	From_Node (0);
	Link_Dir (0);
}

//---------------------------------------------------------
//	Node_Path_Array constructor
//---------------------------------------------------------

Node_Path_Array::Node_Path_Array (int max_records) : 
	Node_Array (max_records)
{
	Data_Size (sizeof (Node_Path_Data));
}

