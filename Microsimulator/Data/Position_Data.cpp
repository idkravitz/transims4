//*********************************************************
//	Position_Data.cpp - vehicle position data classes
//*********************************************************

#include "Position_Data.hpp"

#include <string.h>

//---------------------------------------------------------
//	Position_Data constructor
//---------------------------------------------------------

Position_Data::Position_Data (void)
{
	num_cells = 0;
	data = NULL;
}

void Position_Data::Clear (void)
{
	if (data != NULL) {
		delete [] data;
		data = NULL;
	}
	num_cells = 0;
}

bool Position_Data::Num_Cells (int num)
{
	Clear ();

	if (num > 0) {
		data = new Position [num + 1];

		if (data == NULL) return (false);

		memset (data, '\0', (num+1) * sizeof (Position));
		num_cells = num;
	}
	return (true);
}

//---------------------------------------------------------
//	Position_Data_Array constructor
//---------------------------------------------------------

Position_Data_Array::Position_Data_Array (int max_records) : 
	Data_Array (sizeof (Position), max_records)
{
}
