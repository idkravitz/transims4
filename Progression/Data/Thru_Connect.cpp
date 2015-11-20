//*********************************************************
//	Thru_Connect.cpp - thru connections to upstream link
//*********************************************************

#include "Thru_Connect.hpp"

//---------------------------------------------------------
//	Thru_Connect constructor
//---------------------------------------------------------

Thru_Connect::Thru_Connect (int link_dir) : Class_Index (link_dir)
{
	Thru_Link_Dir (0);
}

//---------------------------------------------------------
//	Thru_Connect_Array constructor
//---------------------------------------------------------

Thru_Connect_Array::Thru_Connect_Array (int max_records) : 
	Class_Array (sizeof (Thru_Connect), max_records)
{
}
