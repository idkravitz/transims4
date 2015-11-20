//*********************************************************
//	Grid_Data.cpp - network grid data class
//*********************************************************

#include "Grid_Data.hpp"

#include <string.h>

//---------------------------------------------------------
//	Grid_Data constructor
//---------------------------------------------------------

Grid_Data::Grid_Data (void) : Dir_Data (0)
{
	data = NULL;
	cells = cell_in = cell_out = 0;
	lanes = type = 0;
}

void Grid_Data::Clear (void)
{
	if (cells > 0) {
		delete [] data;
	}
	cells = 0;
	lanes = 0;
}

int  Grid_Data::index = 0;
bool Grid_Data::grid = true;

bool Grid_Data::Index (int cell, int lane)
{ 
	if (cell > 0 && cell <= cells && lane > 0 && lane <= lanes) {
		index = (grid) ? 0 : cells * lanes;
		index += cell + (lane - 1) * cells - 1;
		return (true);
	} else {
		return (false);
	}
}

bool Grid_Data::Index_To (int cell, int lane)
{ 
	if (cell > 0 && cell <= cells && lane > 0 && lane <= lanes) {
		index = (grid) ? cells * lanes : 0;
		index += cell + (lane - 1) * cells - 1;
		return (true);
	} else {
		return (false);
	}
}
bool Grid_Data::Index_Dir (int lane)
{ 
	if (lane > 0 && lane <= lanes) {
		index = 2 * cells * lanes + lane - 1;
		return (true);
	} else {
		return (false);
	}
}

bool Grid_Data::Index_Lane (int lane)
{ 
	if (lane > 0 && lane <= lanes) {
		index = 2 * cells * lanes + lanes + lane - 1;
		return (true);
	} else {
		return (false);
	}
}

bool Grid_Data::Index_Use (int lane)
{ 
	if (lane > 0 && lane <= lanes) {
		index = 2 * cells * lanes + 2 * lanes + lane - 1;
		return (true);
	} else {
		return (false);
	}
}

bool Grid_Data::Index_TOD (int lane)
{ 
	if (lane > 0 && lane <= lanes) {
		index = 2 * cells * lanes + 3 * lanes + lane - 1;
		return (true);
	} else {
		return (false);
	}
}

bool Grid_Data::Data_Size (int ncells, int nlane)
{
	Clear ();

	if (ncells > 0 && ncells < 32500 && nlane > 0 && nlane < 125) {
		int size = 2 * nlane * ncells + 4 * nlane;

		data = new int [size];
		if (data != NULL) {
			memset (data, '\0', size * sizeof (int));
			cells = (short) ncells;
			lanes = (char) nlane;
			return (true);
		}
	}
	return (false);
}

void Grid_Data::Zero_Grid (void)
{ 
	if (data != NULL) {
		int i, size, *ptr;

		size = cells * lanes;
		ptr = data + ((grid) ? 0 : size);

		for (i=0; i < size; i++, ptr++) {
			if (*ptr != -1) *ptr = 0;
		}
	}
}

//---------------------------------------------------------
//	Grid_Array constructor
//---------------------------------------------------------

Grid_Array::Grid_Array (int max_records) : 
	Dir_Array (max_records)
{
	Data_Size (sizeof (Grid_Data));
}

void Grid_Array::Clear (void)
{
	for (Grid_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Clear ();
	}
	Data_Array::Clear ();
}

void Grid_Array::Zero_Grids (void)
{
	for (Grid_Data *ptr = First (); ptr != NULL; ptr = Next ()) {
		ptr->Zero_Grid ();
	}
}

