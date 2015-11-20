//*********************************************************
//	Link_Data.cpp - network link data classes
//*********************************************************

#include "Link_Data.hpp"

//---------------------------------------------------------
//	Link_Data constructor
//---------------------------------------------------------

Link_Data::Link_Data (int link) : Class_Index (link)
{
	Anode (0);
	Bnode (0);
	Length (0);
	Aoffset (0);
	Boffset (0);
	Use (0);
	Type (0);
	AB_Dir (0);
	BA_Dir (0);
	Shape (0);
}

//---------------------------------------------------------
//	Link_Array constructor
//---------------------------------------------------------

Link_Array::Link_Array (int max_records) : 
	Class_Array (sizeof (Link_Data), max_records, true)
{
}
