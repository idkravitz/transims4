//*********************************************************
//	Read_Problem.cpp - Read the Problem File
//*********************************************************

#include "ProblemSum.hpp"

#include "Utility.hpp"
#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Read_Problem
//---------------------------------------------------------

void ProblemSum::Read_Problem (int nfile)
{
	int time, link, dir, type;
	bool density_flag;

	Link_Data *link_ptr = NULL;
	TTime_Data *ttime_ptr;
	Location_Data *location_ptr = NULL;

	if (problem_file.Extend ()) {
		Show_Message ("Reading Problem File %s -- Record", problem_file.Extension ());
	} else {
		Show_Message ("Reading Problem File -- Record");
	}
	Set_Progress (10000);

	//---- initialize the database ----

	if (nfile == 0) {
		if (!problem_db.Max_Records (problem_file.Estimate_Records ())) {
			Error ("Insufficient Memory for Problem Database");
		}
	}
	link = dir = 0;
	density_flag = Report_Flag (DENSITY_REPORT);

	//---- read each problem record ----

	while (problem_file.Read ()) {
		Show_Progress ();
		
		//---- get the link ----

		if (problem_file.Link_Field () > 0) {
			link = problem_file.Link ();
			dir = problem_file.Dir ();

			if (link < 0) {
				link = -link;
				dir = 1;
			}
		}

		//---- get the problem time ----

		if (problem_file.Time_Field () > 0) {
			time = times.Step (problem_file.Time ());
		} else {
			time = times.Step (problem_file.Start ());
		}

		//---- check the time period criteria ----

		if (time_flag) {
			if (!times.In_Range (time)) continue;
		}

		//---- check the link selection criteria ----

		if (link_flag) {
			if (!links.In_Range (link)) continue;
		}

		//---- check the problem type criteria ----

		if (type_flag) {
			if (!types.In_Range (problem_file.Problem ())) continue;
		}

		//---- get the link data ----
			
		if (link > 0 && net_flag) {
			link_ptr = link_data.Get (link);
			if (link_ptr == NULL) continue;

		} else if (location_flag) {
			location_ptr = location_data.Get (problem_file.Origin ());
			if (location_ptr == NULL) continue;

			link_ptr = link_data.Get (location_ptr->Link ());
			if (link_ptr == NULL) continue;

			dir = location_ptr->Dir ();
		}

		//---- check the subarea ----

		if (subarea_flag) {
			if (link > 0) {
				Node_Data *node_ptr = node_data.Get (link_ptr->Anode ());

				if (!In_Polygon (UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), &select_subarea.points)) {
					node_ptr = node_data.Get (link_ptr->Bnode ());

					if (!In_Polygon (UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), &select_subarea.points)) {
						continue;
					}
				}
			} else if (location_ptr != NULL) {
				if (!In_Polygon (UnRound (location_ptr->X ()), UnRound (location_ptr->Y ()), &select_subarea.points)) continue;
			} else {
				continue;
			}
		}

		//---- save the problem data ----

		problem_db.Copy_Fields (&problem_file);

		if (!problem_db.Add_Record ()) {
			Error ("Writing Problem Database");
		}
		nproblem++;

		//---- sum the problem types ----

		type = problem_file.Problem ();

		if (type > 0 && type < MAX_PROBLEM) {
			if (num_problems [type] == 0) num_types++;
			num_problems [type]++;
			num_problems [0]++;
		}

		//---- store link problems -----

		if (density_flag && link_ptr != NULL) {
			time = times.In_Increment (time);

			if (dir == 0) {
				dir = link_ptr->AB_Dir ();
			} else {
				dir = link_ptr->BA_Dir ();
			}
			if (dir == 0) continue;

			ttime_ptr = ttime_data [dir];
			if (ttime_ptr == NULL) continue;

			//---- store the data ----

			if (ttime_ptr->Periods () == 0) {
				if (!ttime_ptr->Periods (ttime_data.Periods ())) {
					Error ("Insufficient Memory for Time Period Data");
				}
			}
			ttime_ptr->Add_Count (time, 1);
		}
	}
	End_Progress ();

	problem_file.Close ();
}
