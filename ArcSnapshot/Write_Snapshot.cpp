//*********************************************************
//	Write_Snapshot.cpp - write snapshot records
//*********************************************************

#include "ArcSnapshot.hpp"

//---------------------------------------------------------
//	Write_Snapshot
//---------------------------------------------------------

void ArcSnapshot::Write_Snapshot (int period)
{
	int cell, lane, occupancy;
	char buffer [STRING_BUFFER];

	Cell_Data *cell_ptr;
	Dir_Data *dir_ptr;
	Range_Data *range_ptr;

	if (snapshot_flag) {
		Show_Message ("Writing %s -- Record", arcview_snapshot.File_Type ());

		//---- open the output file ----

		str_fmt (buffer, sizeof (buffer), "%s_%s.shp", shapename, time_period.Range_Label (period, pad_flag));

		if (!arcview_snapshot.Open (buffer)) {
			File_Error ("Opening ArcView Snapshot File", arcview_snapshot.Shape_Filename ());
		}
		arcview_snapshot.Write_Header ();

	} else {
		Show_Message ("Writing %s -- Record", new_snapshot_file.File_Type ());
	}
	Set_Progress (10000);

	range_ptr = time_period [period];

	arcview_snapshot.Put_Field (start_field, time_period.Format_Step (range_ptr->Low ()));
	arcview_snapshot.Put_Field (end_field, time_period.Format_Step (range_ptr->High () + 1));

	for (cell_ptr = (Cell_Data *) cell_data.First (); cell_ptr; cell_ptr = (Cell_Data *) cell_data.Next ()) {
		if (method == MAXIMUM) {
			if (cell_ptr->max_total == 0) continue;
		} else {
			if (cell_ptr->total == 0) continue;
		}
		dir_ptr = dir_data [cell_data.Record_Index ()];
		if (dir_ptr == NULL) continue;

		arcview_snapshot.Put_Field (link_field, dir_ptr->Link ());
		arcview_snapshot.Put_Field (dir_field, dir_ptr->Dir ());

		for (lane=0; lane < cell_ptr->lanes; lane++) {
			for (cell=0; cell < cell_ptr->cells; cell++) {
				if (method == MAXIMUM) {
					occupancy = cell_ptr->maximum [lane * cell_ptr->cells + cell];
				} else {
					occupancy = cell_ptr->data [lane * cell_ptr->cells + cell];
				}
				if (occupancy > 0) {
					arcview_snapshot.Put_Field (lane_field, lane + 1);
					arcview_snapshot.Put_Field (offset_field, cell * cell_size + cell_size);
					arcview_snapshot.Put_Field (occ_field, occupancy);

					Show_Progress ();
					Draw_Vehicle ();
				}
			}
		}
		memset (cell_ptr->data, '\0', cell_ptr->lanes * cell_ptr->cells * sizeof (int));
		cell_ptr->total = 0;

		if (method == MAXIMUM) {
			memset (cell_ptr->maximum, '\0', cell_ptr->lanes * cell_ptr->cells * sizeof (int));
			cell_ptr->max_total = 0;
		}
	}
	End_Progress ();

	if (snapshot_flag) {
		arcview_snapshot.Close ();
	}
}
