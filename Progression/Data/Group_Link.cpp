//*********************************************************
//	Group_Link.cpp - network grid data class
//*********************************************************

#include "Group_Link.hpp"

//---------------------------------------------------------
//	Group_Link constructor
//---------------------------------------------------------

Group_Link::Group_Link (void)
{
	Node (0);
	Signal (0);
	Offset (0);
	Green (0);
	TTime (0);
	Length (0);
	VHT (0);
}

//---------------------------------------------------------
//	Group_Link_Array constructor
//---------------------------------------------------------

Group_Link_Array::Group_Link_Array (int max_records) : 
	Data_Array (sizeof (Group_Link), max_records)
{
}
