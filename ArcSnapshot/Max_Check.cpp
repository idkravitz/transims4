//*********************************************************
//	Max_Check.cpp - check the link maximums
//*********************************************************

#include "ArcSnapshot.hpp"

//---------------------------------------------------------
//	Maximum_Check
//---------------------------------------------------------

void ArcSnapshot::Maximum_Check (void)
{
	Cell_Data *cell_ptr;

	for (cell_ptr = (Cell_Data *) cell_data.First (); cell_ptr; cell_ptr = (Cell_Data *) cell_data.Next ()) {
		if (cell_ptr->total == 0) continue;
		if (cell_ptr->max_total < cell_ptr->total) {
			cell_ptr->max_total = cell_ptr->total;
			memcpy (cell_ptr->maximum, cell_ptr->data, cell_ptr->lanes * cell_ptr->cells * sizeof (int));
		}
		cell_ptr->total = 0;
		memset (cell_ptr->data, '\0', cell_ptr->lanes * cell_ptr->cells * sizeof (int));
	}
}
