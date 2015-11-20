//*********************************************************
//	Dir_Path_Data.cpp - dir data class for path building
//*********************************************************

#include "Dir_Path_Data.hpp"

//---------------------------------------------------------
//	Dir_Path_Data constructor
//---------------------------------------------------------

Dir_Path_Data::Dir_Path_Data (void) : Dir_Data (0)
{
	From_List (0);
	To_List (0);
	Length (0);
	Bnode (0);
	Type (0);
	Use (0);
	Aoffset (0);
	Boffset (0);
	Offset (0);
	Zone (0);
}

//---------------------------------------------------------
//	Dir_Path_Array constructor
//---------------------------------------------------------

Dir_Path_Array::Dir_Path_Array (int max_records) : 
	Dir_Array (max_records)
{
	Data_Size (sizeof (Dir_Path_Data));
}

