//*********************************************************
//	Read_Traveler.cpp - reads the traveler file
//*********************************************************

#include "ArcTraveler.hpp"

#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Read_Traveler
//---------------------------------------------------------

void ArcTraveler::Read_Traveler (void)
{
	int id_field, veh_field, start_field, end_field, dist_field, ttime_field, speed_field;
	int i, traveler, last_traveler, last_spd, last_time, time, nsaved, link, dir, lane, start_time;
	double distance, ttime, offset, side, speed;
	bool select_flag;

	Link_Data *link_ptr;
	Point_Data points;
	Traveler_Data *data_ptr;

	Show_Message ("Reading %s -- Record", traveler_file.File_Type ());
	Set_Progress (10000);
	
	select_flag = (travelers.Num_Ranges () > 0);

	id_field = arcview_traveler.Field_Number ("TRAVELER");
	veh_field = arcview_traveler.Field_Number ("VEHICLE");
	start_field = arcview_traveler.Field_Number ("STARTTIME");
	end_field = arcview_traveler.Field_Number ("ENDTIME");
	dist_field = arcview_traveler.Field_Number ("DISTANCE");
	ttime_field = arcview_traveler.Field_Number ("TTIME");
	speed_field = arcview_traveler.Field_Number ("SPEED");

	if (!traveler_data.Max_Records (MIN (traveler_file.Estimate_Records (), 100000))) goto mem_error;

	//---- read and sort the traveler file ----

	while (traveler_file.Read ()) {
		Show_Progress ();

		//---- check the traveler id ----

		traveler = traveler_file.Traveler ();

		if (select_flag && !travelers.In_Range (traveler)) continue;

		//---- check the time ----

		time = times.Step (traveler_file.Time ());

		if (time_flag && !times.In_Range (time)) continue;

		//---- check the location ----

		link = traveler_file.Link ();
		dir = traveler_file.Dir ();
		offset = traveler_file.Offset ();
		lane = traveler_file.Lane ();

		if (link == 0 || lane == 0) continue;

		if (subarea_flag) {
			link_ptr = link_data.Get (link);
			if (link_ptr == NULL) continue;

			if (dir == 0) {
				distance = offset;
			} else {
				distance = UnRound (link_ptr->Length ()) - offset;
			}
			Link_Shape (link_ptr, dir, &points, distance, 0.0, 0.0);

			if (!In_Polygon (points [1]->x, points [1]->y, &select_subarea.points)) continue;
		}

		//---- save the data record ----

		data_ptr = traveler_data.New_Record (true);
		if (data_ptr == NULL) goto mem_error;

		data_ptr->Traveler (traveler);
		data_ptr->Time (time);
		data_ptr->Vehicle (traveler_file.Vehicle ());
		data_ptr->Link (link);
		data_ptr->Dir (dir);
		data_ptr->Offset (Round (offset));
		data_ptr->Lane (lane);
		data_ptr->Distance (Round (traveler_file.Distance ()));
		data_ptr->Speed (Round (traveler_file.Speed ()));

		if (!traveler_data.Add ()) goto mem_error;
	}
	End_Progress ();

	Print (2, "Number of Traveler Records Read = %d", Progress_Count ());

	traveler_file.Close ();

	//---- output the selected travelers ----

	Show_Message ("Writing %s -- Record", arcview_traveler.File_Type ());
	Set_Progress (10000);

	last_traveler = last_time = start_time = time = nsaved = 0;
	last_spd = 1;
	distance = ttime = 0.0;

	for (data_ptr = traveler_data.First_Key (); ; data_ptr = traveler_data.Next_Key ()) {
		Show_Progress ();

		if (data_ptr != NULL) {
			traveler = data_ptr->Traveler ();
			time = data_ptr->Time ();
		}
		if (traveler != last_traveler || (last_spd > 0 && (time - last_time) > 1) || data_ptr == NULL) {
			if (last_traveler > 0 && last_time > start_time) {
				ttime = last_time - start_time;
				speed = distance / ttime;

				if (speed > 0.01) {
					arcview_traveler.Put_Field (end_field, times.Format_Time (last_time));
					arcview_traveler.Put_Field (dist_field, distance);
					arcview_traveler.Put_Field (ttime_field, ttime);
					arcview_traveler.Put_Field (speed_field, distance / ttime);

					if (!arcview_traveler.Write_Record ()) {
						Error ("Writing ArcView Traveler File");
					}
					nsaved++;
				}
			}
			if (data_ptr == NULL) break;

			last_traveler = traveler;
			distance = 0;

			arcview_traveler.Put_Field (id_field, traveler);

			last_time = start_time = data_ptr->Time ();
			last_spd = data_ptr->Speed ();

			arcview_traveler.Put_Field (start_field, times.Format_Time (start_time));

			arcview_traveler.points.Reset ();
		}
		if (data_ptr->Vehicle () > 0) {
			arcview_traveler.Put_Field (veh_field, data_ptr->Vehicle ());
		}
		link = data_ptr->Link ();
		dir = data_ptr->Dir ();
		lane = data_ptr->Lane ();
		offset = UnRound (data_ptr->Offset ());
		distance += UnRound (data_ptr->Distance ());
		last_time = time;
		last_spd = data_ptr->Speed ();

		link_ptr = link_data.Get (link);

		side = (2 * lane - 1) * lane_width / 2.0;

		if (dir == 1) offset = UnRound (link_ptr->Length ()) - offset;

		Link_Shape (link_ptr, dir, &points, offset, 0.0, side);

		for (i=1; i <= points.Num_Points (); i++) {
			if (!arcview_traveler.points.Add (points [i])) goto mem_error;
		}
	}
	End_Progress ();

	arcview_traveler.Close ();

	Print (1, "Number of Traveler Records Saved = %d", nsaved);
	return;

mem_error:
	Error ("Insufficient Memory for Traveler Data");
	return;
}

