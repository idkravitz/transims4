//*********************************************************
//	Read_Problem.cpp - Read the Problem File
//*********************************************************

#include "ArcPlan.hpp"

#include "Vehicle_Shape.hpp"

//---------------------------------------------------------
//	Read_Problem
//---------------------------------------------------------

void ArcPlan::Read_Problem (void)
{
	int problem, traveler, hhold, person, link, dir;
	double offset;

	Link_Data *link_ptr;

	Show_Message ("Reading %s -- Record", problem_file.File_Type ());
	Set_Progress (10000);

	//---- read each problem record ----

	while (problem_file.Read ()) {
		Show_Progress ();

		//---- check the problem type ----

		problem = problem_file.Problem ();
		if (problem < 1 || problem >= max_problem || !problem_type [problem]) continue;

		//---- check the household list ----

		if (hhlist_flag) {
			hhold = problem_file.Household ();

			if (hhold_list.Get (&hhold) == NULL) continue;
		}

		//---- output the problem details ----

		link = problem_file.Link ();

		if (problem_out && link != 0) {
			arcview_problem.Copy_Fields (&problem_file);

			dir = problem_file.Dir ();

			if (problem_file.LinkDir_Type () == LINK_SIGN) {
				if (link < 0) {
					link = -link;
					dir = 1;
				}
			}
			link_ptr = link_data.Get (link);

			if (link_ptr == NULL) {
				Write (1, "Warning: Problem Link %d was Not Found", link);
				continue;
			}

			//---- get the link direction ----

			if (problem_file.LinkDir_Type () == LINK_NODE) {

				if (dir == link_ptr->Anode ()) {
					dir = 1;
				} else if (dir == link_ptr->Bnode ()) {
					dir = 0;
				} else {
					Write (1, "Warning: Problem Node %d not on Link %d", dir, link);
					continue;
				}
			}
			offset = problem_file.Offset ();

			if (dir == 1) {
				offset = UnRound (link_ptr->Length ()) - offset;
			}

			Link_Shape (link_ptr, dir, &points, offset, 0.0, 0.0);

			arcview_problem.points.Set (1, points [1]);

			//Link_Shape (link_ptr, dir, &points, offset, 6.5, 0.0);
			//
			//Vehicle_Shape (*(points [1]), *(points [points.Num_Points ()]), 2.0, &arcview_problem.points);

			if (!arcview_problem.Write_Record ()) {
				Error ("Writing ArcView Problem File");
			}
			num_problem++;
		}

		hhold = problem_file.Household ();
		person = problem_file.Person ();

		traveler = hhold * Traveler_Scale () + person;

		if (problem_list.Get_Index (traveler) == 0) {
			problem_list.Add (traveler);
		}
	}
	End_Progress ();

	problem_file.Close ();
	arcview_problem.Close ();

	Print (2, "Number of Problem Records Read = %d", Progress_Count ());
	Print (1, "Number of Problem Travelers Saved = %d", problem_list.Num_Records ());
}
