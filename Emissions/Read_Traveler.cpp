//*********************************************************
//	Read_Traveler.cpp - reads the traveler file
//*********************************************************

#include "Emissions.hpp"

#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Read_Traveler
//---------------------------------------------------------

void Emissions::Read_Traveler (void)
{
	int i, second, time, link, dir, link_dir, traveler, last_traveler, last_time, last_dir, last_spd;
	double distance, speed, offset;

	Link_Data *link_ptr;
	Point_Data points;
	Traveler_Data *data_ptr, *start_ptr, *end_ptr;
	Traveler_Array traveler_sort;
	Vehicle_Data *veh_ptr;

	Show_Message ("Reading %s -- Record", traveler_file.File_Type ());
	Set_Progress (10000);

	if (!traveler_sort.Max_Records (MIN (traveler_file.Estimate_Records (), 100000))) goto mem_error;
	if (!traveler_data.Max_Records (traveler_sort.Max_Records ())) goto mem_error;

	//---- read and sort the traveler file ----

	while (traveler_file.Read ()) {
		Show_Progress ();

		if (traveler_file.Lane () == 0) continue;

		//---- check the time ----

		time = time_periods.Step (traveler_file.Time ());
		if (!time_periods.Period (time)) continue;

		//---- check the location ----

		link = traveler_file.Link ();
		if (link == 0) continue;

		dir = traveler_file.Dir ();
		offset = traveler_file.Offset ();

		if (subarea_flag || link_eq_flag) {
			link_ptr = link_data.Get (link);
			if (link_ptr == NULL) continue;

			if (dir == 0) {
				distance = offset;
				link_dir = link_ptr->AB_Dir ();
			} else {
				distance = UnRound (link_ptr->Length ()) - offset;
				link_dir = link_ptr->BA_Dir ();
			}
			if (link_eq_flag && link_list.Get_Index (link_dir) == 0) continue;

			if (subarea_flag) {
				Link_Shape (link_ptr, dir, &points, distance, 0.0, 0.0);

				if (!In_Polygon (points [1]->x, points [1]->y, &select_subarea.points)) continue;
			}
		}

		//---- save the data record ----

		data_ptr = traveler_sort.New_Record (true);
		if (data_ptr == NULL) goto mem_error;

		data_ptr->Traveler (traveler_file.Traveler ());
		data_ptr->Time (time);
		data_ptr->Vehicle (traveler_file.Vehicle ());
		data_ptr->Link (link);
		data_ptr->Dir (dir);
		data_ptr->Offset (Round (offset));
		data_ptr->Lane (traveler_file.Lane ());
		data_ptr->Distance (Round (traveler_file.Distance ()));
		data_ptr->Speed (Round (traveler_file.Speed ()));

		if (!traveler_sort.Add ()) goto mem_error;

new_traveler_file.Traveler (data_ptr->Traveler ()-1);
new_traveler_file.Vehicle (data_ptr->Vehicle ());
new_traveler_file.Time (time_periods.Format_Time (data_ptr->Time ()));
new_traveler_file.Link (data_ptr->Link ());
new_traveler_file.Dir (data_ptr->Dir ());
new_traveler_file.Offset (UnRound (data_ptr->Offset ()));
new_traveler_file.Lane (data_ptr->Lane ());
new_traveler_file.Distance (UnRound (data_ptr->Distance ()));
new_traveler_file.Speed (UnRound (data_ptr->Speed ()));

if (!new_traveler_file.Write ()) {
	Error ("Writing %s", new_traveler_file.File_Type ());
}
	}
	End_Progress ();

	Print (2, "Number of Traveler Records Read = %d", Progress_Count ());
	Print (1, "Number of Traveler Records Saved = %d", traveler_sort.Num_Records ());

	traveler_file.Close ();

	//---- expand the traveler records ----

	Show_Message ("Expand the Traveler Data -- Record");
	Set_Progress (10000);

	last_traveler = last_dir = last_time = last_spd = 0;
	start_ptr = end_ptr = NULL;

	for (data_ptr = traveler_sort.First_Key (); data_ptr; data_ptr = traveler_sort.Next_Key ()) {
		Show_Progress ();

		link_ptr = link_data.Get (data_ptr->Link ());
		if (link_ptr == NULL) continue;

		if (data_ptr->Dir () == 0) {
			link_dir = link_ptr->AB_Dir ();
		} else {
			link_dir = link_ptr->BA_Dir ();
		}
		traveler = data_ptr->Traveler ();
		time = data_ptr->Time ();

		if (traveler != last_traveler || link_dir != last_dir) {
			last_traveler = traveler;
			last_dir = link_dir;
			last_time = time - 1;
			last_spd = data_ptr->Speed ();
		}
		if (last_spd == 0) {
			for (i=last_time+1; i < time; i++) {
				end_ptr = traveler_data.New_Record (true);
				if (end_ptr == NULL) goto mem_error;

				memcpy (end_ptr, data_ptr, sizeof (Traveler_Data));

				end_ptr->Time (i);
				end_ptr->Distance (0);
				end_ptr->Speed (0);

				if (!traveler_data.Add ()) goto mem_error;
			}
		}
		end_ptr = traveler_data.New_Record (true);
		if (end_ptr == NULL) goto mem_error;

		memcpy (end_ptr, data_ptr, sizeof (Traveler_Data));

		if (!traveler_data.Add ()) goto mem_error;

		last_time = time;
		last_spd = data_ptr->Speed ();
	}
	End_Progress ();

	Print (1, "Number of Expanded Traveler Records = %d", traveler_data.Num_Records ());

	traveler_sort.Clear ();

	//---- smooth the traveler speeds ----

	if (smooth_flag) {

		Show_Message ("Smooth the Traveler Speeds -- Record");
		Set_Progress (10000);

		last_traveler = last_dir = 0;
		start_ptr =  NULL;

		for (data_ptr = traveler_data.First_Key (); ; data_ptr = traveler_data.Next_Key ()) {

			if (data_ptr != NULL) {
				Show_Progress ();

				link_ptr = link_data.Get (data_ptr->Link ());
				if (link_ptr == NULL) continue;

				if (data_ptr->Dir () == 0) {
					link_dir = link_ptr->AB_Dir ();
				} else {
					link_dir = link_ptr->BA_Dir ();
				}
				traveler = data_ptr->Traveler ();
			}
			if (traveler != last_traveler || link_dir != last_dir || data_ptr == NULL) {
				if (last_dir > 0 && second > 0) {
					smooth_data.Smooth (second);

					//---- output the smoothed data ----

					end_ptr = data_ptr;
					second = 0;

					for (data_ptr = traveler_data.Get (start_ptr); data_ptr; data_ptr = traveler_data.Next_Key ()) {
						if (data_ptr == end_ptr) break;

						speed = smooth_data.Output (++second);

						data_ptr->Speed (Round (speed));
						data_ptr->Distance (Round (speed));
					}
				}
				if (data_ptr == NULL) break;

				last_traveler = traveler;
				last_dir = link_dir;
				second = 0;

				start_ptr = data_ptr;
			}
			smooth_data.Input (++second, UnRound (data_ptr->Speed ()));
		}
		End_Progress ();

		Print (1, "Number of Traveler Records Smoothed = %d", Progress_Count ());
	}

	//---- write the new traveler file ----

	if (new_traveler_flag) {
		Show_Message ("Writing %s -- Record", new_traveler_file.File_Type ());
		Set_Progress (10000);

		last_spd = 1;
		i = 0;

		for (data_ptr = traveler_data.First_Key (); data_ptr; data_ptr = traveler_data.Next_Key ()) {
			Show_Progress ();

			if (data_ptr->Speed () > 0 || last_spd != 0) {
				new_traveler_file.Traveler (data_ptr->Traveler ());
				new_traveler_file.Vehicle (data_ptr->Vehicle ());
				new_traveler_file.Time (time_periods.Format_Time (data_ptr->Time ()));
				new_traveler_file.Link (data_ptr->Link ());
				new_traveler_file.Dir (data_ptr->Dir ());
				new_traveler_file.Offset (UnRound (data_ptr->Offset ()));
				new_traveler_file.Lane (data_ptr->Lane ());
				new_traveler_file.Distance (UnRound (data_ptr->Distance ()));
				new_traveler_file.Speed (UnRound (data_ptr->Speed ()));

				if (!new_traveler_file.Write ()) {
					Error ("Writing %s", new_traveler_file.File_Type ());
				}
				i++;
			}
			last_spd = data_ptr->Speed ();
		}
		End_Progress ();

		new_traveler_file.Close ();

		Print (1, "Number of Traveler Records Written = %d", i);
	}

	//---- convert the vehicle types ----

	if (Demand_File_Flag (VEHICLE)) {
		Show_Message ("Assigning Vehicle Types -- Record");
		Set_Progress (10000);

		last_traveler = i = 0;

		for (data_ptr = traveler_data.First_Key (); data_ptr; data_ptr = traveler_data.Next_Key ()) {
			Show_Progress ();

			traveler = data_ptr->Vehicle ();
			if (traveler != last_traveler) {
				last_traveler = traveler;

				veh_ptr = vehicle_data.Get (traveler);
				if (veh_ptr != NULL) {

					//---- check the vehicle type ----

					i = veh_ptr->Type ();

					if (veh_map_flag) {
						i = vehicle_map.Map (i, veh_ptr->Sub_Type ());

						if (i == 0) {
							Warning ("Vehicle Type %d-%d is Not in the Type Map", veh_ptr->Type (), veh_ptr->Sub_Type ());
							i = 1;
						}
					}
					if (i > 40) i = 1;
				} else {
					i = 1;
				}
			}
			data_ptr->Vehicle (i);
		}
		End_Progress ();
	} else {
		for (data_ptr = traveler_data.First (); data_ptr; data_ptr = traveler_data.Next ()) {
			data_ptr->Vehicle (1);
		}
	}

	//---- apply the emissions rates ----

	if (rate_flag) {
		Traveler_Emissions ();
	}
	return;

mem_error:
	Error ("Insufficient Memory for Traveler Data");
	return;
}

