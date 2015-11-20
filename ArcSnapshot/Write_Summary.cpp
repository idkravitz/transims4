//*********************************************************
//	Write_Summary.cpp - write link summary records
//*********************************************************

#include "ArcSnapshot.hpp"

//---------------------------------------------------------
//	Write_Summary
//---------------------------------------------------------

void ArcSnapshot::Write_Summary (int period)
{
	int i, index;

	Link_Data *link_ptr;
	Range_Data *range_ptr;

	range_ptr = time_period [period];

	summary_file.Put_Field (start_fld, time_period.Format_Step (range_ptr->Low ()));
	summary_file.Put_Field (end_fld, time_period.Format_Step (range_ptr->High () + 1));

	for (link_ptr = link_data.First_Key (); link_ptr; link_ptr = link_data.Next_Key ()) {

		index = link_data.Record_Index ();
		if (vehicles [0] [index] <= 0) continue;

		summary_file.Put_Field (link_fld, link_ptr->Link ());

		for (i=0; i < max_type; i++) {
			if (i > 0 && veh_type_flag && !veh_types.In_Range (i)) continue;

			summary_file.Put_Field (veh_fld [i], vehicles [i] [index]);
			summary_file.Put_Field (per_fld [i], persons [i] [index]);
		}
		if (!summary_file.Write ()) {
			Error ("Writing %s", summary_file.File_Type ());
		}
		num_sum++;
	}
}
