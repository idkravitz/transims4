//*********************************************************
//	Green_Phase.cpp - signal green phase movements data
//*********************************************************

#include "Green_Phase.hpp"

//---------------------------------------------------------
//	Green_Phase constructor
//---------------------------------------------------------

Green_Phase::Green_Phase (int in_link_dir, int out_link_dir) : 
	Class2_Index (in_link_dir, out_link_dir)
{
	Green_Time (0);
	Phase_Offset (0);
	Movement (0);
}

//---------------------------------------------------------
//	Green_Phase_Array constructor
//---------------------------------------------------------

Green_Phase_Array::Green_Phase_Array (int max_records) : 
	Class2_Array (sizeof (Green_Phase), max_records, false)
{
}
