//*********************************************************
//	Link_Detail_Data.cpp - network link detail data classes
//*********************************************************

#include "Link_Detail_Data.hpp"

//---------------------------------------------------------
//	Link_Detail_Data constructor
//---------------------------------------------------------

Link_Detail_Data::Link_Detail_Data (int link_dir) : Class_Index (link_dir)
{
	Type (0);
	Control (0);
	Left (0);
	Left_Thru (0);
	Thru (0);
	Right_Thru (0);
	Right (0);
	Use (0);
	Lanes (0);
	Period (NULL);
}

//---------------------------------------------------------
//	Link_Detail_Array constructor
//---------------------------------------------------------

Link_Detail_Array::Link_Detail_Array (int max_records) : 
	Class_Array (sizeof (Link_Detail_Data), max_records, true)
{
}

