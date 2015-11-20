//*********************************************************
//	Turn_Data.cpp - network turn penality data classes
//*********************************************************

#include "Turn_Data.hpp"

//---------------------------------------------------------
//	Turn_Data constructor
//---------------------------------------------------------

Turn_Data::Turn_Data (int in_link_dir, int out_link_dir) : 
	Class2_Index (in_link_dir, out_link_dir)
{
	Start (0);
	End (0);
	Use (0);
	Penalty (0);
}

//---------------------------------------------------------
//	Turn_Array constructor
//---------------------------------------------------------

Turn_Array::Turn_Array (int max_records) : 
	Class2_Array (sizeof (Turn_Data), max_records, false)
{
}
