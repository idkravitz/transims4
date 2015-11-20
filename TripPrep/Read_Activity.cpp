//*********************************************************
//	Read_Activity.cpp - Read the Activity File
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Read_Activity
//---------------------------------------------------------

bool TripPrep::Read_Activity (int file_num)
{
	int last_hhold, last_person, hhold, person, purpose, mode, start, end, origin, destination;
	int time1, time2, num_kept, num_out, count, vehicle, *parking, nparking, pass;
	int next_in, next_merge, replace_id, last_keep;
	bool flag, read_merge, read_in, save_merge, keep;

	Activity_File *activity_file, *new_file;
	Trip_File *trip_file, *in_file;
	Vehicle_File *veh_file;
	Vehicle_Data *veh_ptr;
	Location_Data *loc_ptr;
	Access_Data *acc_ptr;

	activity_file = (Activity_File *) Demand_Db_Base (ACTIVITY);
	in_file = (Trip_File *) Demand_Db_Base (TRIP);
	nparking = 0;
	parking = 0;

	if (file_num > 0) {
		if (!split_flag) {
			if (!activity_file->Open (file_num)) return (false);
		} else {
			activity_file->Rewind ();
		}
		if (hhlist_flag && hhlist_file.Extend ()) {
			if (!Household_List (file_num)) {
				if (split_flag) return (false);
				Error ("Opening %s", hhlist_file.File_Type ());
			}
		}
	}
	if (output_flag) {
		new_file = (Activity_File *) Demand_Db_Base (NEW_ACTIVITY);

		if (file_num > 0) {
			if (new_file->Extend ()) {
				if (!new_file->Open (file_num)) {
					Error ("Creating %s", new_file->File_Type ());
				}
			}
			if (prob_flag && newhh_flag && newhh_file.Extend ()) {
				if (!newhh_file.Open (file_num)) {
					Error ("Opening %s", newhh_file.File_Type ());
				}
			}
			if (merge_act_flag && merge_act_file.Extend ()) {
				if (!merge_act_file.Open (file_num)) {
					Error ("Opening %s", merge_act_file.File_Type ());
				}
			}
		}
	}
	if (create_flag || convert_flag) {
		trip_file = (Trip_File *) Demand_Db_Base (NEW_TRIP);

		if (file_num > 0 && trip_file->Extend ()) {
			if (!trip_file->Open (file_num)) {
				Error ("Creating %s", trip_file->File_Type ());
			}
		}
		if (create_flag) {
			veh_file = (Vehicle_File *) Demand_Db_Base (NEW_VEHICLE);

			if (file_num > 0) {
				if (newhh_flag && newhh_file.Extend ()) {
					if (!newhh_file.Open (file_num)) {
						Error ("Opening %s", newhh_file.File_Type ());
					}
					fprintf (newhh_file.File (), "HHOLD\tOLDHH\n");
				}
			} else if (newhh_flag) {
				fprintf (newhh_file.File (), "HHOLD\tOLDHH\n");
			}

			//---- build the parking map ----

			loc_ptr = location_data.Last ();
			nparking = loc_ptr->Location () + 1;

			parking = new int [nparking];
			memset (parking, '\0', nparking * sizeof (int));

			for (acc_ptr = access_data.First (); acc_ptr; acc_ptr = access_data.Next ()) {
				if (acc_ptr->From_Type () == LOCATION_ID && acc_ptr->To_Type () == PARKING_ID) {
					parking [acc_ptr->From_ID ()] = acc_ptr->To_ID ();
				}
			}
		}
	}

	//---- process each trip ----

	if (merge_act_flag) {
		if (activity_file->Extend ()) {
			Show_Message ("Merging %s %s -- Record", activity_file->File_Type (), activity_file->Extension ());
		} else {
			Show_Message ("Merging %s -- Record", activity_file->File_Type ());
		}
		if (merge_act_file.Read ()) {
			next_merge = merge_act_file.Household ();
		} else {
			next_merge = MAX_INTEGER;
		}
	} else {
		if (activity_file->Extend ()) {
			Show_Message ("Reading %s %s -- Record", activity_file->File_Type (), activity_file->Extension ());
		} else {
			Show_Message ("Reading %s -- Record", activity_file->File_Type ());
		}
		next_merge = MAX_INTEGER;
	}
	Set_Progress (10000);

	last_hhold = last_person = num_kept = num_out = replace_id = last_keep = 0;
	read_merge = read_in = save_merge = false;
	count = 1;
	flag = true;

	if (activity_file->Read ()) {
		next_in = activity_file->Household ();
	} else {
		next_in = MAX_INTEGER;
	}

	//---- process each record ----

	while (next_in != MAX_INTEGER || next_merge != MAX_INTEGER) {
		Show_Progress ();

		//---- set the processing flags ----

		if (next_merge < next_in) {
			if (next_merge == replace_id) {
				save_merge = false;
			} else {
				save_merge = true;
			}
			read_merge = true;
			read_in = false;
		} else {
			hhold = activity_file->Household ();

			if (hhlist_flag && hhold_list.Get_Index (hhold) == 0) {
				if (all_flag) {
					new_file->Copy_Fields (activity_file);
					flag = false;

					if (!new_file->Write ()) {
						Error ("Writing %s", new_file->File_Type ());
					}
					num_out++;
					keep = true;
					replace_id = next_in;
				} else {
					keep = false;
				}
			} else {
				person = activity_file->Person ();
				purpose = activity_file->Purpose ();
				mode = activity_file->Mode ();
				vehicle = activity_file->Vehicle ();
				destination = activity_file->Location ();

				time1 = time_periods.Step (activity_file->Start_Min ());
				time2 = time_periods.Step (activity_file->Start_Max ());

				if (time1 < 0 || time2 < 0) {
					Error ("Converting Start Time for Household %d", hhold);
				}
				end = (time1 + time2 + 1) / 2;

				if (hhold == last_hhold && person == last_person) {
					flag = Trip_Check (hhold, origin, destination, start, end, mode, purpose, vehicle);

					if (flag) {
						if (create_flag) {
							trip_file->Household (hhold_id);
							trip_file->Person (person);
							trip_file->Trip (activity_file->Activity ());
							trip_file->Purpose (purpose);

							pass = activity_file->Passengers ();

							if (mode == DRIVE_ALONE) {
								if (pass > 2) {
									mode = CARPOOL4;
								} else if (pass > 1) {
									mode = CARPOOL3;
								} else if (pass > 0) {
									mode = CARPOOL2;
								}
							} else if (mode == CARPOOL2 || mode == CARPOOL3 || mode == CARPOOL4) {
								mode = MAGIC_MOVE;
							}
							trip_file->Mode (mode);

							trip_file->Start (time_periods.Format_Step (start));
							trip_file->Origin (origin);
							trip_file->Arrive (time_periods.Format_Step (end));
							trip_file->Destination (destination);
							trip_file->Constraint (activity_file->Constraint ());

							if (vehicle > 0) {
								trip_file->Vehicle (veh_id);

								veh_ptr = vehicle_data.Get (vehicle);

								if (veh_ptr != NULL && origin < nparking) {
									veh_file->Vehicle (veh_id);
									veh_file->Household (hhold_id);
									veh_file->Location (parking [origin]);
									veh_file->Type (veh_ptr->Type ());
									veh_file->Sub_Type (veh_ptr->Sub_Type ());

									if (!veh_file->Write ()) {
										Error ("Writing %s", veh_file->File_Type ());
									}
								}
								veh_id++;
							} else {
								trip_file->Vehicle (vehicle);
							}
							if (!trip_file->Write ()) {
								Error ("Writing %s", trip_file->File_Type ());
							}
							if (newhh_flag) {
								fprintf (newhh_file.File (), "%d\t%d\n", hhold_id, hhold);
							}
							hhold_id++;
						} else if (convert_flag) {
							trip_file->Household (hhold);
							trip_file->Person (person);
							trip_file->Trip (activity_file->Activity ());
							trip_file->Purpose (purpose);

							pass = activity_file->Passengers ();

							if (mode == DRIVE_ALONE) {
								if (pass > 2) {
									mode = CARPOOL4;
								} else if (pass > 1) {
									mode = CARPOOL3;
								} else if (pass > 0) {
									mode = CARPOOL2;
								}
							} else if (mode == CARPOOL2 || mode == CARPOOL3 || mode == CARPOOL4) {
								mode = MAGIC_MOVE;
							}
							trip_file->Mode (mode);

							trip_file->Start (time_periods.Format_Step (start));
							trip_file->Origin (origin);
							trip_file->Arrive (time_periods.Format_Step (end));
							trip_file->Destination (destination);
							trip_file->Constraint (activity_file->Constraint ());
							trip_file->Vehicle (vehicle);

							if (!trip_file->Write ()) {
								Error ("Writing %s", trip_file->File_Type ());
							}
						}
					}
				} else {
					flag = Trip_Check (hhold, origin, destination, start, end, mode, purpose, vehicle);
				}
				origin = destination;

				time1 = time_periods.Step (activity_file->End_Min ());
				time2 = time_periods.Step (activity_file->End_Max ());

				if (time1 < 0 || time2 < 0) {
					Error ("Converting End Time for Household %d", hhold);
				}
				start = (time1 + time2 + 1) / 2;

				last_hhold = hhold;
				last_person = person;

				if ((flag || all_flag) && output_flag) {
					new_file->Copy_Fields (activity_file);

					if (script_flag) {
						if (trip_flag) {
							in_file->Reset_Record ();
						}
						keep = (program.Execute () != 0);
					} else {
						keep = flag;
					}
					if (keep || all_flag) {
						if (flag && keep) {
							num_kept++;
							if (newhh_flag && hhold != last_keep) {
								fprintf (newhh_file.File (), "%d\n", hhold);
								last_keep = hhold;
							}
						}
						if (!new_file->Write ()) {
							Error ("Writing %s", new_file->File_Type ());
						}
						num_out++;
					}
				}
			}
			if (next_merge == next_in) {
				if (flag) {
					replace_id = next_in;
					save_merge = false;
				} else {
					save_merge = true;
				}
				read_merge = read_in = true;
			} else {
				read_in = true;
				read_merge = save_merge = false;
			}
		}
		
		//---- write the merge activity to the output file ----
		
		if (save_merge) {
			vehicle = merge_act_file.Vehicle ();

			if (vehicle_list.Get_Index (vehicle) == 0) {
				if (!vehicle_list.Add (vehicle)) {
					Error ("Adding Vehicle %d to the List", vehicle);
				}
			}
			if (output_flag) {
				new_file->Copy_Fields (&merge_act_file);

				if (script_flag) {
					if (trip_flag) {
						in_file->Reset_Record ();
					}
					keep = (program.Execute () != 0);
				} else {
					keep = true;
				}
				if (keep) {
					if (!new_file->Write ()) {
						Error ("Writing %s", new_file->File_Type ());
					}
					num_out++;
				}
			}
		}

		//---- get the next merge activity ----

		if (read_merge) {
			if (merge_act_file.Read ()) {
				next_merge = merge_act_file.Household ();
			} else {
				next_merge = MAX_INTEGER;
			}
		}
        			
		//---- get the next input activity ----

		if (read_in) {
			if (activity_file->Read ()) {
				next_in = activity_file->Household ();
			} else {
				next_in = MAX_INTEGER;
			}
		}
	}
	End_Progress ();

	if (file_num == 0 || !split_flag) {
		total_in += Progress_Count ();
	}
	if (activity_file->Extend ()) {
		Print (2, "Number of Activity Records Read from %s = %d", activity_file->Extension (), Progress_Count ());
	} else {
		Print (2, "Number of Activity Records Read = %d", Progress_Count ());
	}
	if (output_flag) {
		total_out += num_out;
		if (new_file->Extend ()) {
			Print (1, "Number of Activity Records Written to %s = %d", new_file->Extension (), num_out);
		} else {
			Print (1, "Number of Activity Records Written = %d", num_out);
		}
	}
	total_used += num_kept;
	if (prob_flag) {
		Print (1, "Number of Activity Records Selected = %d", num_kept);
		if (num_out > 0) Print (0, " (%.1lf%%)", 100.0 * num_kept / num_out);
	}

	if (nparking > 0 && parking) {
		delete [] parking;
	}
	return (true);
}
