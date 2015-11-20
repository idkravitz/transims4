//*********************************************************
//	Read_Problem.cpp - reads the problem file
//*********************************************************

#include "ArcProblem.hpp"

#include "Vehicle_Shape.hpp"
#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Read_Problem
//---------------------------------------------------------

void ArcProblem::Read_Problem (void)
{
	int time, nsaved, period, last_period, center, link, dir, lane;
	char buffer [STRING_BUFFER];
	double offset, side, veh_width, veh_length;

	XYZ_Point pt1, pt2;
	Link_Data *link_ptr = NULL;
	Dir_Data *dir_ptr;
	Location_Data *location_ptr = NULL;

	Show_Message ("Reading %s -- Record", problem_file.File_Type ());
	Set_Progress (10000);
	Print (1);

	//---- read each problem record ----

	last_period = -1;
	nsaved = link = dir = 0;
	if (shape_flag) {
		veh_length = 6.5;
	} else {
		veh_length = 1.5;
	}
	veh_width = lane_width * 0.80;

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

		if (!od_flag && link > 0) {

			link_ptr = link_data.Get (link);
			if (link_ptr == NULL) continue;

			offset = problem_file.Offset ();

		} else {

			location_ptr = location_data.Get (problem_file.Origin ());
			if (location_ptr == NULL) continue;

			if (od_flag) {
				link = 0;
			} else {
				link_ptr = link_data.Get (location_ptr->Link ());
				if (link_ptr == NULL) continue;

				dir = location_ptr->Dir ();
				offset = UnRound (location_ptr->Offset ());
			}
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
			} else {
				if (!In_Polygon (UnRound (location_ptr->X ()), UnRound (location_ptr->Y ()), &select_subarea.points)) continue;
			}
		}

		//---- get the time increment ----

		period = times.Period (time);

		if (period != last_period) {
			if (last_period >= 0) {
				arcview_problem.Close ();
			}
			last_period = period;

			//---- construct the shapefile name ----

			if (times.Period_Flag ()) {
				str_fmt (buffer, sizeof (buffer), "%s_%s.shp", shapename, times.Range_Label (period));
			} else {
				str_fmt (buffer, sizeof (buffer), "%s.shp", shapename);
			}
			if (!arcview_problem.Open (buffer)) {
				File_Error ("Opening ArcView Problem File", arcview_problem.Shape_Filename ());
			}
			arcview_problem.Write_Header ();
		}

		//---- save the record ----

		arcview_problem.Copy_Fields (&problem_file);

		//---- draw the problem ----

		if (od_flag) {
			arcview_problem.points.Reset ();

			pt1.x = UnRound (location_ptr->X ());
			pt1.y = UnRound (location_ptr->Y ());
			pt1.z = 0.0;

			arcview_problem.points.Add (&pt1);

			location_ptr = location_data.Get (problem_file.Destination ());
			if (location_ptr == NULL) continue;

			pt1.x = UnRound (location_ptr->X ());
			pt1.y = UnRound (location_ptr->Y ());

			arcview_problem.points.Add (&pt1);
			
		} else {

			offset -= veh_length;
			
			if (dir == 1) {
				offset = UnRound (link_ptr->Length ()) - offset;
			}
			if (center_flag) {
				if (link_ptr->BA_Dir () == 0) {
					dir_ptr = dir_data [link_ptr->AB_Dir ()];
					center = dir_ptr->Left () + dir_ptr->Thru () + dir_ptr->Right () + 1;
				} else if (link_ptr->AB_Dir () == 0) {
					dir_ptr = dir_data [link_ptr->BA_Dir ()];
					center = dir_ptr->Left () + dir_ptr->Thru () + dir_ptr->Right () + 1;
				} else {
					center = 1;
				}
			} else {
				center = 1;
			}
			if (link > 0) {
				lane = problem_file.Lane ();
			} else {
				lane = center + 1;
			}
			side = (2 * lane - center) * lane_width / 2.0;

			if (shape_flag) {
				Link_Shape (link_ptr, dir, &points, offset, veh_length, side);

				pt1 = *(points [points.Num_Points ()]);
				pt2 = *(points [1]);

				Vehicle_Shape (pt1, pt2, veh_width, &arcview_problem.points);
			} else {
				Link_Shape (link_ptr, dir, &points, offset, 0.0, side);

				arcview_problem.points.Set (1, points [1]);
			}
		}

		if (!arcview_problem.Write_Record ()) {
			Error ("Writing ArcView Problem File");
		}
		nsaved++;
	}
	End_Progress ();

	problem_file.Close ();

	if (last_period >= 0) {
		arcview_problem.Close ();
	}

	Print (2, "Number of Problem Records Read = %d", Progress_Count ());
	Print (1, "Number of Problem Records Saved = %d", nsaved);
}

