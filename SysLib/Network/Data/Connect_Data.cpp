//*********************************************************
//	Connect_Data.cpp - network lane connectivity classes
//*********************************************************

#include "Connect_Data.hpp"

//---------------------------------------------------------
//	Connect_Data constructor
//---------------------------------------------------------

Connect_Data::Connect_Data (int in_link_dir, int out_link_dir) : 
	Class2_Index (in_link_dir, out_link_dir)
{
	In_Lane_Low (0);
	In_Lane_High (0);
	Out_Lane_Low (0);
	Out_Lane_High (0);
	In_Thru_Low (0);
	In_Thru_High (0);
	Out_Thru_Low (0);
	Out_Thru_High (0);
	Type (0);
	Penalty (0);
	Speed (0);
	Capacity (0);
}

//---------------------------------------------------------
//	Connect_Array constructor
//---------------------------------------------------------

Connect_Array::Connect_Array (int max_records) : 
	Class2_Array (sizeof (Connect_Data), max_records, false)
{
}
