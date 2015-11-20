//*********************************************************
//	Area_Type.cpp - area type data class
//*********************************************************

#include "Area_Type.hpp"
#include "Network_Code.hpp"

//---------------------------------------------------------
//	Area_Type constructor
//---------------------------------------------------------

Area_Type::Area_Type (int type) : Class_Index (type)
{
	Primary (0);
	Secondary (0);
	Signal_Type (0);
	Ring_Type (0);
	Split_Len (0);
	All_Stop (Network_Code::LOCAL);
}

//---------------------------------------------------------
//	Area_Type_Array constructor
//---------------------------------------------------------

Area_Type_Array::Area_Type_Array (int max_records) : 
	Class_Array (sizeof (Area_Type), max_records)
{
}
