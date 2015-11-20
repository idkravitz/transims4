//*********************************************************
//	Problem_Link.cpp - write the problem link file
//*********************************************************

#include "ProblemSum.hpp"

//---------------------------------------------------------
//	Problem_Link
//---------------------------------------------------------

void ProblemSum::Problem_Link (void)
{
	int i, count, problem, nout;
	bool dir_flag;

	Link_Data *link_ptr;
	TTime_Data *ttime_ptr;
	Range_Data *range_ptr;

	dir_flag = (link_file.Field_Number ("NODE") == 0);

	if (type_flag) {
		range_ptr = types [1];
		problem = range_ptr->Low ();
	} else {
		problem = 0;
	}

	Show_Message ("Writing %s -- Record", link_file.File_Type ());
	Set_Progress (10000);
	nout = 0;

	//---- write the problem links ----

	for (ttime_ptr = ttime_data.First (); ttime_ptr; ttime_ptr = ttime_data.Next ()) {
		Show_Progress ();

		if (ttime_ptr->Periods () > 0) {
			link_file.Link (ttime_ptr->Link ());

			if (dir_flag) {
				link_file.Node (ttime_ptr->Dir ());
			} else {
				link_ptr = link_data.Get (ttime_ptr->Link ());

				if (ttime_ptr->Dir () == 1) {
					link_file.Node (link_ptr->Anode ());
				} else {
					link_file.Node (link_ptr->Bnode ());
				}
			}
			for (i=1; i <= ttime_ptr->Periods (); i++) {
				count = ttime_ptr->Count (i);
				if (count == 0) continue;

				range_ptr = times [i];
				
				link_file.Start_Time (times.Format_Step (range_ptr->Low ()));
				link_file.End_Time (times.Format_Step (range_ptr->High () + 1));

				link_file.Problem (problem);
				link_file.Count (count);

				if (!link_file.Write ()) {
					Error ("Writing %s", link_file.File_Type ());
				}
				nout++;
			}
		}
	}
	End_Progress ();

	link_file.Close ();

	Print (2, "Number of %s Records = %d", link_file.File_Type (), nout);
}
