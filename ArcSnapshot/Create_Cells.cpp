//*********************************************************
//	Create_Cells.cpp - allocate cells for each link
//*********************************************************

#include "ArcSnapshot.hpp"

//---------------------------------------------------------
//	Create_Cells
//---------------------------------------------------------

void ArcSnapshot::Create_Cells (void)
{
	int i, dir, lanes, cells;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Cell_Data *cell_ptr;

	if (!cell_data.Num_Records (dir_data.Num_Records ())) goto mem_error;

	for (link_ptr = link_data.First (); link_ptr; link_ptr = link_data.Next ()) {
		if (link_ptr->Use () == 0) continue;

		cells = (int) (UnRound (link_ptr->Length ()) / cell_size + 0.5);

		for (i=0; i < 2; i++) {
			if (i == 0) {
				dir = link_ptr->AB_Dir ();
			} else {
				dir = link_ptr->BA_Dir ();
			}
			if (dir == 0) continue;

			dir_ptr = dir_data [dir];
			if (dir_ptr == NULL) continue;

			cell_ptr = (Cell_Data *) cell_data [dir];
			if (cell_ptr == NULL) continue;

			cell_ptr->lanes = lanes = dir_ptr->Thru () + dir_ptr->Right () + dir_ptr->Left ();
			cell_ptr->cells = cells;
			cell_ptr->total = 0;

			cell_ptr->data = new int [lanes * cells];
			if (cell_ptr->data == NULL) goto mem_error;

			memset (cell_ptr->data, '\0', lanes * cells * sizeof (int));	

			if (method == MAXIMUM) {
				cell_ptr->max_total = 0;
				cell_ptr->maximum = new int [lanes * cells];
				if (cell_ptr->maximum == NULL) goto mem_error;

				memset (cell_ptr->maximum, '\0', lanes * cells * sizeof (int));
			}
		}
	}
	return;

mem_error:
	Error ("Insufficient Memory for Cell Data");
}
