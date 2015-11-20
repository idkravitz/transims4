//*********************************************************
//	Read_Problem.cpp - Read the Problem File
//*********************************************************

#include "ProblemSelect.hpp"

#include "Utility.hpp"
#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Read_Problem
//---------------------------------------------------------

void ProblemSelect::Read_Problem (void)
{
	int hhold, time, link;

	if (problem_file.Extend ()) {
		Show_Message ("Reading Problem File %s -- Record", problem_file.Extension ());
	} else {
		Show_Message ("Reading Problem File -- Record");
	}
	Set_Progress (10000);

	hhold_list.Reset ();
	if (!hh_flag) {
		hh_list.Reset ();
	}

	//---- read each problem record ----

	while (problem_file.Read ()) {
		Show_Progress ();
		nproblem++;

		//---- check for a new household ID ----

		hhold = problem_file.Household ();

		if (hhold_list.Get_Index (hhold) > 0) continue;

		if (!hh_flag) {
			if (!hhold_list.Add (hhold)) goto add_error;
		}

		//---- check the link selection criteria ----

		link = problem_file.Link ();
		if (link < 0) link = -link;

		if (link_flag) {
			if (!links.In_Range (link)) continue;
		}

		//---- check the time period criteria ----

		if (time_flag) {
			if (problem_file.Time_Field () > 0) {
				time = times.Step (problem_file.Time ());
			} else {
				time = times.Step (problem_file.Start ());
			}
			if (!times.In_Range (time)) continue;
		}

		//---- check the problem type criteria ----

		if (type_flag) {
			if (!types.In_Range (problem_file.Problem ())) continue;
		}

		//---- check the subarea ----

		if (subarea_flag) {
			if (link > 0) {
				Link_Data *link_ptr = link_data.Get (link);
				if (link_ptr == NULL) continue;

				Node_Data *node_ptr = node_data.Get (link_ptr->Anode ());

				if (!In_Polygon (UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), &select_subarea.points)) {
					node_ptr = node_data.Get (link_ptr->Bnode ());

					if (!In_Polygon (UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()), &select_subarea.points)) {
						continue;
					}
				}
			} else {
				Location_Data *location_ptr = location_data.Get (problem_file.Origin ());
				if (location_ptr == NULL) continue;

				if (!In_Polygon (UnRound (location_ptr->X ()), UnRound (location_ptr->Y ()), &select_subarea.points)) continue;
			}
		}

		//---- save the selected household ----

		if (hh_flag) {
			if (!hhold_list.Add (hhold)) goto add_error;
		} else {
			if (!hh_list.Add (hhold)) goto add_error;
		}
		num_select++;
	}
	End_Progress ();

	problem_file.Close ();
	return;

add_error:
	Error ("Adding Household to the List");
}
