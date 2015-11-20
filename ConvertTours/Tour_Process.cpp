//*********************************************************
//	Tour_Process.cpp - process records in the tour file
//*********************************************************

#include "ConvertTours.hpp"

//---------------------------------------------------------
//	Tour_Processing
//---------------------------------------------------------

void ConvertTours::Tour_Processing (void)
{
	int hhold, person, tour, org, des, stop, mode, park, start, end, person_id, veh, ttim, skim, skim_in;
	int num_person, num_work, num_veh, last_person;
	int group_out, group_in, diurnal_out, diurnal_in, mode_out, mode_in, work_code, minutes, sort_key;
	int last_hhold, home, home_taz, trip, out_end, start_return;
	int vehicle [20], work [20], work_end [20], work_return [20], trip_end [20], work_veh [20];
	double dist1, dist2;
	char start_str [20], arrive_str [20];
	Tour_Data tour_rec, *tour_ptr;
	bool stat, work_flag;

	File_Group *group;
	Matrix_Data *skim_ptr;

	//---- read the tour file ----

	Show_Message ("Reading %s -- Record", tour_file.File_Type ());
	Set_Progress (100000);

	last_hhold = -1;
	home = home_taz = num_person = num_work = num_veh = 0;
	trip = 1;
	memset (vehicle, '\0', sizeof (vehicle));
	memset (work_veh, '\0', sizeof (work_veh));
	memset (work, '\0', sizeof (work));
	memset (work_end, '\0', sizeof (work_end));
	memset (work_return, '\0', sizeof (work_return));
	work_flag = false;

	//---- sort the tour records -----

	for (;;) {
		stat = tour_file.Read ();

		hhold = tour_file.Household ();

		if (!stat || hhold != last_hhold) {
			if (last_hhold >= 0) {
				if (last_hhold == 0) last_hhold = hhold_id++;

				tour_file_db.Rewind ();

				while (tour_file_db.Read_Record ()) {

					//---- process record ----

					tour_file_db.Get_Field (tour_file.Person_Field () + 1, &person);
					tour_file_db.Get_Field (tour_file.Tour_Field () + 1, &tour);

					//---- get the origin and destination activity locations -----

					tour_file_db.Get_Field (tour_file.Origin_Field () + 1, &org);
					tour_file_db.Get_Field (tour_file.Dest_Field () + 1, &des);
					tour_file_db.Get_Field (tour_file.Stop_Out_Field () + 1, &stop);

					if (home != 0) {
						org = home;
					}
					if (script_flag) {
						script_base.Put_Field (1, 1);	//---- group out ----
						script_base.Put_Field (2, 1);	//---- group in ----
						script_base.Put_Field (3, 0);	//---- diurnal out ----
						script_base.Put_Field (4, 0);	//---- diurnal in ----
						script_base.Put_Field (5, 0);	//---- mode out ----
						script_base.Put_Field (6, 0);	//---- mode in ----
						script_base.Put_Field (7, 0);	//---- work code ----

						if (!group_script.Execute ()) continue;

						script_base.Get_Field (1, &group_out);
						script_base.Get_Field (2, &group_in);
						script_base.Get_Field (3, &diurnal_out);
						script_base.Get_Field (4, &diurnal_in);
						script_base.Get_Field (5, &mode_out);
						script_base.Get_Field (6, &mode_in);
						script_base.Get_Field (7, &work_code);

						work_flag = false;
						if (work_code == 2) {
							if (work [person] != 0) {
								org = work [person];
							} else {
								work_flag = true;
								num_work++;
							}
						}
					} else {
						tour_file_db.Get_Field (tour_file.Group_Field () + 1, &group_out);
						group_in = group_out;
						diurnal_out = diurnal_in = mode_out = mode_in = work_code = 0;
						work_flag = false;
					}
					if (group_out == 0) group_out = 1;
					if (group_in == 0) group_in = 1;

					skim = skim_in = 0;

					if (skim_flag) {
						skim_ptr = ttime_skim.Get (org, des);

						if (skim_ptr != NULL) {
							skim = skim_ptr->Data ();
						}
						skim_ptr = ttime_skim.Get (des, org);

						if (skim_ptr != NULL) {
							skim_in = skim_ptr->Data ();
						}
					}
					group = file_group [group_out];
					if (group == NULL) {
						Warning ("Outbound Group %d was Not Found", group_out);
						continue;
					}
					if (!Locate_OD (group, org, des, stop, park, dist1, dist2)) {
						Warning ("Household %d Person %d Tour %d could not be Located", last_hhold, person, tour);
						tot_errors++;
						continue;
					}
					if (work_code < 2) {
						home = -org;

						if (work_code == 1) {
							work [person] = -des;
						}
					} else if (work_flag) {
						work [person] = -org;
					}

					//---- get the mode ----

					mode = mode_out;
					if (mode == 0) mode = group->Mode ();

					//---- estimate the travel time ----

					if (skim != 0) {
						ttim = skim + additional_time;
					} else {
						ttim = (int) (((dist1 + dist2) / group->Speed ()) + additional_time + 0.5);
					}
					if (stop > 0) ttim += group->Duration ();

					//--- get the trip start and end time ----

					tour_file_db.Get_Field (tour_file.Start_Field () + 1, &start);
					start *= 60;

					if (group->Method () == 0) {
						start -= 30;
						if (start < 0) start = 0;
						end = start + 60;
						if (end > 24*60) end = 24*60;
					} else if (group->Method () == 1) {
						end = start + 30;
						if (end > 24*60) end = 24*60;
						start = end - 60;
						if (start < 0) start = 0;
					} else {
						end = start + 60;
						if (end > 24*60) end = 24*60;
					}
					if (work_code == 2) {
						if (start < work_end [person] && work_end [person] != 0) {
							start = work_end [person];
							if (end < start + 10) {
								end = start + 10;
								if (end > 24*60) end = 24*60;
							}
						}
						if (work_return [person] != 0) {
							start_return = (work_end [person] + work_return [person]) / 2;

							if (end > start_return) {
								end = start_return;
								if (end < start + 10) {
									start = end - 10;
									if (start < work_end [person] && work_end [person] != 0) {
										start = work_end [person];
									}
								}
							}
						}
					} else {
						if (start < trip_end [person] && trip_end [person] != 0) {
							start = trip_end [person];
							if (end < start + 5) {
								end = start + 5;
								if (end > 24*60) end = 24*60;
							}
						}
					}
					if (!Set_Time (group, start, end, ttim, diurnal_out)) {
						Warning ("Household %d Person %d Tour %d could not be Scheduled", last_hhold, person, tour);
						tot_errors++;
						continue;
					}
					if (work_code == 2 && work_return [person] != 0) {
						start_return = work_return [person] * 60 - ttim;

						if (end > start_return) {
							Warning ("Household %d Person %d At-Work Tour %d could not be Scheduled", last_hhold, person, tour);
							tot_errors++;
							continue;
						}
						start_return /= 60;
					} else {
						start_return = 0;
					}

					//---- write new household records ----

					if (hhold_num.Get_Index (last_hhold) == 0) {
						hhold_num.Add (last_hhold);

						household_file->Household (last_hhold);
						household_file->Location (org);
						household_file->Persons (num_person);
						household_file->Workers (num_work);
						household_file->Vehicles (num_veh);

						if (!household_file->Write ()) {
							Error ("Writing Household File");
						}
						nhhold++;
					}

					//---- write new person records ---- 

					person_id = last_hhold * 20 + person;

					if (person_num.Get_Index (person_id) == 0) {
						person_num.Add (person_id);

						population_file->Household (last_hhold);
						population_file->Person (person);
						population_file->Age (25);
						population_file->Gender (1);
						population_file->Work (1);
						population_file->Drive (1);

						if (!population_file->Write ()) {
							Error ("Writing Population File");
						}
						nperson++;
					}

					//---- vehicle data ----

					if (mode == DRIVE_ALONE || mode == PNR_OUT || mode == PNR_IN || 
						mode == CARPOOL2 ||	mode == CARPOOL3 || mode == CARPOOL4 || 
						mode == KNR_OUT || mode == KNR_IN) {

						if (work_flag) {
							veh = work_veh [person];
						} else {
							veh = vehicle [person];
						}
						if (veh == 0) {
							veh = vehicle_id++;
							if (work_flag) {
								work_veh [person] = veh;
							} else {
								vehicle [person] = veh;
							}
							num_veh++;
							vehicle_file->Vehicle (veh);
							vehicle_file->Household (last_hhold);
							vehicle_file->Location (park);
							vehicle_file->Type (group->Type ());
							vehicle_file->Sub_Type (group->SubType ());

							if (!vehicle_file->Write ()) {
								Error ("Writing Vehicle File");
							}
						}
					} else {
						veh = 0;
					}

					//---- add the trip to the stop ----

					tour_rec.household = last_hhold;
					tour_rec.person = person;
					tour_rec.purpose = group->Purpose ();
					tour_rec.mode = mode;
					tour_rec.vehicle = veh;

					if (stop > 0) {
						if (skim == 0) skim = ttim;
						skim = ttim - group->Duration ();
						ttim = (int) (skim * dist1 / (dist1 + dist2) + 0.5);
						skim -= ttim;

						ttim += start;
						tour_rec.start = start;
						tour_rec.origin = org;
						tour_rec.arrive = ttim;
						tour_rec.destination = stop;
						tour_rec.constraint = 0;

						if (!tour_sort.Add (&tour_rec)) goto tour_error;

						start = ttim + group->Duration ();
					} else {
						stop = org;
					}
					out_end = end / 60 + 1;
					if (work_code > 0) {
						work_end [person] = out_end;
					}
					tour_rec.start = start;
					tour_rec.origin = stop;
					tour_rec.arrive = end;
					tour_rec.destination = des;
					tour_rec.constraint = group->Method ();

					if (!tour_sort.Add (&tour_rec)) goto tour_error;

					//---- process the return trip ----

					group = file_group [group_in];
					if (group == NULL) {
						Warning ("Inbound Group %d was Not Found", group_in);
						continue;
					}
					tour_file_db.Get_Field (tour_file.Stop_In_Field () + 1, &stop);

					des = -des;
					org = -org;

					if (!Locate_OD (group, des, org, stop, park, dist1, dist2)) {
						Warning ("Household %d Person %d Tour %d could not be Located", last_hhold, person, tour);
						tot_errors++;
						continue;
					}

					//---- get the mode ----

					mode = mode_in;
					if (mode == 0) mode = group->Mode ();

					tour_rec.mode = mode;

					//---- estimate the travel time ----

					if (skim_in != 0) {
						ttim = skim_in + additional_time;
					} else {
						ttim = (int) (((dist1 + dist2) / group->Speed ()) + additional_time + 0.5);
					}
					if (stop > 0) ttim += group->Duration ();
					minutes = (ttim + 50) / 60;

					//--- get the trip return time ----

					tour_file_db.Get_Field (tour_file.Return_Field () + 1, &end);
					end *= 60;

					if (group->Method () == 0) {
						start = end - 30;
						if (start < 0) start = 0;
						end = start + 60;
						if (end > 24*60) end = 24*60;
						if (start < out_end) start = out_end;
					} else if (group->Method () == 1) {
						end += 30;
						if (end > 24*60) end = 24*60;
						start = end - 60;
						if (start < 0) start = 0;
						if (start < out_end + minutes) start = out_end + minutes;
					} else {
						start = end;
						end = start + 60;
						if (end > 24*60) end = 24*60;
						if (start < out_end + minutes / 2) start = out_end + minutes / 2;
					}
					if (end < start + 10) {
						end = start + 10;
						if (end > 24*60) {
							end = 24*60;
							if (start > end) start = end - 5;
						}
					}
					if (work_code == 2) {
						if (start < work_end [person] && work_end [person] != 0) {
							start = work_end [person];
							if (end < start + 10) {
								end = start + 10;
								if (end > 24*60) end = 24*60;
							}
						}
						if (end > work_return [person] && work_return [person] != 0) {
							end = work_return [person];
							if (end < start + 10) {
								start = end - 10;
								if (start < work_end [person]) {
									start = work_end [person];
								}
							}
						}
					}

					if (!Set_Time (group, start, end, ttim, diurnal_in)) {
						Warning ("Household %d Person %d Tour %d could not be Scheduled", last_hhold, person, tour);
						tot_errors++;
						continue;
					}
					if (work_code == 1) {
						work_return [person] = (start / 60) - 1;
					} else if (work_code == 2) {
						work_end [person] = (end / 60) - 1;
					}

					//---- add the trip to the stop ----

					if (stop > 0) {
						if (skim_in == 0) skim_in = ttim;
						skim_in = ttim - group->Duration ();
						ttim = (int) (skim_in * dist1 / (dist1 + dist2) + 0.5);
						skim_in -= ttim;

						ttim += start;
						tour_rec.start = start;
						tour_rec.origin = des;
						tour_rec.arrive = ttim;
						tour_rec.destination = stop;
						tour_rec.constraint = 0;

						if (!tour_sort.Add (&tour_rec)) goto tour_error;

						start = ttim + group->Duration ();
					} else {
						stop = des;
					}
					tour_rec.start = start;
					tour_rec.origin = stop;
					tour_rec.arrive = end;
					tour_rec.destination = org;
					tour_rec.constraint = group->Method ();
					trip_end [person] = end / 60 + 1;

					if (!tour_sort.Add (&tour_rec)) goto tour_error;
				}

				//---- save the records ----

				if (tour_sort.Num_Records () > 0) {
					last_person = 0;

					for (tour_ptr = tour_sort.First_Key (); tour_ptr; tour_ptr = tour_sort.Next_Key ()) {
						trip_file->Household (tour_ptr->household);
						trip_file->Person (tour_ptr->person);
						if (tour_ptr->person != last_person) {
							last_person = tour_ptr->person;
							trip = 1;
						}
						trip_file->Trip (trip++);
						trip_file->Purpose (tour_ptr->purpose);
						trip_file->Mode (tour_ptr->mode);
						trip_file->Vehicle (tour_ptr->vehicle);

						if (trip_time.Format_Step (tour_ptr->start, start_str) == NULL) goto time_error;
						if (trip_time.Format_Step (tour_ptr->arrive, arrive_str) == NULL) goto time_error;

						trip_file->Start (start_str);
						trip_file->Origin (tour_ptr->origin);
						trip_file->Arrive (arrive_str);
						trip_file->Destination (tour_ptr->destination);
						trip_file->Constraint (tour_ptr->constraint);

						if (!trip_file->Write ()) goto trip_error;
						num_trip++;
					}
				}
			}
			last_hhold = hhold;
			trip = 1;
			home = home_taz = num_person = num_work = 0;
			work_flag = false;
			memset (vehicle, '\0', sizeof (vehicle));
			memset (work_veh, '\0', sizeof (work_veh));
			memset (work, '\0', sizeof (work));
			memset (work_end, '\0', sizeof (work_end));
			memset (work_return, '\0', sizeof (work_return));
			memset (trip_end, '\0', sizeof (trip_end));
			tour_sort.Clear ();
			tour_file_db.Clear ();
		}
		if (!stat) break;
		Show_Progress ();

		tot_trips++;

		//---- check the hhold list ----

		if (hhlist_flag) {
			if (hhold > max_hhlist) break;
			if (hhold_list.Get_Index (hhold) == 0) continue;
		}

		//---- save the tour record and sort index ----

		person = tour_file.Person ();
		if (person > num_person) num_person = person;

		start = tour_file.Start ();
		end = tour_file.Return ();
		org = tour_file.Origin ();

		if (home_taz == 0) {
			home_taz = org;
			work_code = 0;
		} else if (home_taz == org) {
			work_code = 0;
		} else {
			work_code = 1;
		}
		sort_key = (person << 11) + (start << 6) + (work_code << 5) + end;

		tour_file_db.Copy_Fields (&tour_file);

		tour_file_db.Put_Field (1, sort_key);

		if (!tour_file_db.Add_Record ()) {
			Error ("Writing Tour File Database");
		}
	}
	End_Progress ();
	return;

time_error:
	Error ("Time of Day Formating");

trip_error:
	Error ("Writing Trip File");

tour_error:
	Error ("Saving Tour Record");
}
