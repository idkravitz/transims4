//*********************************************************
//	Write_Riders - draw the transit ridership polygons
//*********************************************************

#include "ArcPlan.hpp"

#include "Smooth_Shape.hpp"

//---------------------------------------------------------
//	Write_Riders
//---------------------------------------------------------

void ArcPlan::Write_Riders (void)
{
	int i, j, link, dir, link_field, dir_field, rider_field, seg_field, on_field, off_field;
	int stop_num [20], nstop, stop, max_load, riders, *load, seg, board, alight;
	double width, offset, end, len, length, stop_off [20];

	XYZ_Point point, *ptr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Offset_Data *stop_ptr;

	link_field = arcview_rider.Field_Number ("LINK");
	dir_field = arcview_rider.Field_Number ("DIR");
	seg_field = arcview_rider.Field_Number ("SEGMENT");
	on_field = arcview_rider.Field_Number ("BOARD");
	rider_field = arcview_rider.Field_Number ("RIDERS");
	off_field = arcview_rider.Field_Number ("ALIGHT");

	Show_Message ("Writing %s -- Record", arcview_rider.File_Type ());
	Set_Progress (10000);
	
	//---- process each link direction ----

	load = load_data;
	
	for (dir_ptr = dir_data.First (); dir_ptr; dir_ptr = dir_data.Next ()) {
		Show_Progress ();

		link = dir_ptr->Link ();
		dir = dir_ptr->Dir ();

		load++;

		if (dir_ptr->Stop_List () == 0) {
			if (*load == 0 || *load < min_load) continue;
			nstop = 0;
		} else {

			//---- gather the stop data ----

			for (i=0; i < 20; i++) {
				if (i == 0) {
					stop = dir_ptr->Stop_List ();
				} else {
					stop = stop_list [stop];
				}
				if (stop == 0) {
					nstop = i;
					break;
				}
				stop_num [i] = stop;
				//stop_load [i] = board_data [stop] - alight_data [stop];
				
				stop_ptr = stop_offset [stop];
				if (stop_ptr != NULL) {
					stop_off [i] = stop_ptr->Offset ();
				}
			}

			//---- sort the stop offsets ----

			for (i=0; i < nstop-1; i++) {
				for (j=i+1; j < nstop; j++) {
					if ((dir == 0 && stop_off [i] > stop_off [j]) ||
						(dir == 1 && stop_off [i] < stop_off [j])) {
						offset = stop_off [i];
						stop_off [i] = stop_off [j];
						stop_off [j] = offset;

						stop = stop_num [i];
						stop_num [i] = stop_num [j];
						stop_num [j] = stop;
					}
				}
			}

			//---- calculate the max load ----

			max_load = riders = *load;

			for (i=0; i < nstop; i++) {
				stop = stop_num [i];
				riders += board_data [stop] - alight_data [stop];
				if (riders > max_load) max_load = riders;
			}
			if (max_load == 0 || max_load < min_load) continue;
		}

		//---- initialize the record ----

		arcview_rider.Put_Field (link_field, link);
		arcview_rider.Put_Field (dir_field, dir);

		link_ptr = link_data.Get (link);
		length = UnRound (link_ptr->Length ());

		if (dir == 0) {
			offset = 0.0;
		} else {
			offset = length;
		}

		riders = *load;
		seg = 1;
		board = 0;

		//---- process each link segment ----

		for (i=0; i <= nstop; i++) {
	
			if (i == nstop) {
				if (dir == 0) {
					end = length;
				} else {
					end = 0.0;
				}
				alight = 0;
			} else {
				stop = stop_num [i];
				end = stop_off [i];
				alight = alight_data [stop];
			}

			//---- check the ridership ----

			if (riders != 0 && riders >= min_load) {

				//---- write the data fields ----

				arcview_rider.points.Reset ();

				arcview_rider.Put_Field (seg_field, seg);
				arcview_rider.Put_Field (on_field, board);
				arcview_rider.Put_Field (rider_field, riders);
				arcview_rider.Put_Field (off_field, alight);

				if (dir == 0) {
					len = end - offset;
				} else {
					len = offset - end;
				}
				if (len <= 0.0) len = 1.0;

				//---- get the line width ----

				if (load_flag) {
					width = riders / load_factor;

					if (width < min_rider) {
						width = min_rider;
					} else if (width > max_rider) {
						width = max_rider;
					}
					width = -width;

					//---- get the centerline points ----

					Link_Shape (link_ptr, dir, &points, offset, len, 0.0);

					ptr = points.First ();
					point = *ptr;

					for (; ptr; ptr = points.Next ()) {
						if (!arcview_rider.points.Add (ptr)) goto point_error;
					}

					//---- get the outside points of the band ----

					Link_Shape (link_ptr, 1 - dir, &points, end, len, width);

					if (max_angle > 0 && min_length > 0) {
						Smooth_Shape (&points, max_angle, min_length);
					}

					for (ptr = points.First (); ptr; ptr = points.Next ()) {
						if (!arcview_rider.points.Add (ptr)) goto point_error;
					}

					//---- close the polygon ----

					if (!arcview_rider.points.Add (&point)) goto point_error;

				} else {

					width = route_offset;

					Link_Shape (link_ptr, dir, &arcview_rider.points, offset, len, width);
				}

				//---- write the shape record ----

				if (!arcview_rider.Write_Record ()) {
					Error ("Writing ArcView Ridership File");
				}
				num_rider++;
			}

			//---- initialize the next segment ----

			if (i < nstop) {
				board = board_data [stop];
			}
			riders += board - alight;
			offset = end;
			seg++;
		}
	}
	End_Progress ();

	arcview_rider.Close ();
	return;

point_error:
	Error ("Insufficient Memory for %d Points", arcview_rider.points.Num_Points ());
}
