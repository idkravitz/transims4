//*********************************************************
//	Dir_List.cpp - directional link list
//*********************************************************

#include "Dir_List.hpp"

//---------------------------------------------------------
//	Dir_List constructor
//---------------------------------------------------------

Dir_List::Dir_List (int link_dir) : Dir_Data (link_dir), List_Data ()
{
}

//---------------------------------------------------------
//	Dir_List_Array constructor
//---------------------------------------------------------

Dir_List_Array::Dir_List_Array (int max_records) : 
	Dir_Array (max_records)
{
	Data_Size (sizeof (Dir_List));
}

