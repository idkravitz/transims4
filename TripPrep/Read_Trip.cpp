//*********************************************************
//	Read_Trip.cpp - Read the Trip File
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Read_Trip
//---------------------------------------------------------

bool TripPrep::Read_Trip (int file_num)
{
	int hhold, purpose, mode, start, end, origin, destination, vehicle, shift;
	int next_in, next_merge, replace_id, *parking, nparking;
	int last_hh, num_out, num_kept;
	bool read_merge, read_in, save_merge, keep, flag;

	Trip_File *trip_file, *new_file;
	Activity_File *activity_file;
	Vehicle_File *veh_file;
	Vehicle_Data *veh_ptr;
	Location_Data *loc_ptr;
	Access_Data *acc_ptr;

	trip_file = (Trip_File *) Demand_Db_Base (TRIP);
	activity_file = (Activity_File *) Demand_Db_Base (ACTIVITY);
	nparking = 0;
	parking = 0;

	if (file_num > 0) {
		if (!split_flag) {
			if (!trip_file->Open (file_num)) return (false);
		} else {
			trip_file->Rewind ();
		}
		if (hhlist_flag && hhlist_file.Extend ()) {
			if (!Household_List (file_num)) {
				if (split_flag) return (false);
				Error ("Opening %s", hhlist_file.File_Type ());
			}
		}
	}
	if (output_flag) {
		new_file = (Trip_File *) Demand_Db_Base (NEW_TRIP);

		if (file_num > 0) {
			if (new_file->Extend ()) {
				if (!new_file->Open (file_num)) {
					Error ("Creating %s", new_file->File_Type ());
				}
			}
			if ((move_org_flag || move_des_flag || prob_flag) && newhh_flag && newhh_file.Extend ()) {
				if (!newhh_file.Open (file_num)) {
					Error ("Opening %s", newhh_file.File_Type ());
				}
			}
			if (merge_trip_flag && merge_trip_file.Extend ()) {
				if (!merge_trip_file.Open (file_num)) {
					Error ("Opening %s", merge_trip_file.File_Type ());
				}
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
		}
		if (move_org_flag || create_flag) {

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

	if (merge_trip_flag) {
		if (trip_file->Extend ()) {
			Show_Message ("Merging %s %s -- Record", trip_file->File_Type (), trip_file->Extension ());
		} else {
			Show_Message ("Merging %s -- Record", trip_file->File_Type ());
		}
		if (merge_trip_file.Read ()) {
			next_merge = merge_trip_file.Household ();
		} else {
			next_merge = MAX_INTEGER;
		}
	} else {
		if (trip_file->Extend ()) {
			Show_Message ("Reading %s %s -- Record", trip_file->File_Type (), trip_file->Extension ());
		} else {
			Show_Message ("Reading %s -- Record", trip_file->File_Type ());
		}
		next_merge = MAX_INTEGER;
	}
	Set_Progress (10000);

	last_hh = num_kept = num_out = replace_id = 0;
	read_merge = read_in = save_merge = false;

	if (trip_file->Read ()) {
		next_in = trip_file->Household ();
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
			hhold = trip_file->Household ();

			if (hhlist_flag && hhold_list.Get_Index (hhold) == 0) {
				if (all_flag) {
					new_file->Copy_Fields (trip_file);
					flag = false;

					if (move_org_flag) {
						origin = trip_file->Origin ();
						int *ptr = (int *) move_org_map.Get (&origin);
						if (ptr != NULL) {
							new_file->Origin (ptr [1]);
							flag = true;

							int rec [2];
							rec [0] = trip_file->Vehicle ();
							rec [1] = parking [ptr [1]];
							if (!move_veh_map.Add (rec)) {
								Error ("Adding Parking Map");
							}
						}
					}
					if (move_des_flag) {
						destination = trip_file->Destination ();
						int *ptr = (int *) move_des_map.Get (&destination);
						if (ptr != NULL) {
							new_file->Destination (ptr [1]);
							flag = true;
						}
					}
					if (flag && newhh_flag) {
						fprintf (newhh_file.File (), "%d\n", hhold);
					}
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

				//---- check the time period criteria ----

				purpose = trip_file->Purpose ();
				mode = trip_file->Mode ();
				origin = trip_file->Origin ();
				destination = trip_file->Destination ();
				vehicle = trip_file->Vehicle ();

				start = time_periods.Step (trip_file->Start ());
				if (start < 0) {
					Error ("Converting Start Time %s for Household %d", trip_file->Start (), hhold);
				}
				end = time_periods.Step (trip_file->Arrive ());
				if (end < 0) {
					Error ("Converting Arrival Time %s for Household %d", trip_file->Arrive (), hhold);
				}

				//---- check the selection criteria ----

				keep = Trip_Check (hhold, origin, destination, start, end, mode, purpose, vehicle);

				if (output_flag) {
					new_file->Copy_Fields (trip_file);

					if (keep) {
						if (shift_flag) {
							shift = start + shift_size;
							if (shift < 0) {
								shift = 0;
							} else if (shift >= MIDNIGHT) {
								shift = MIDNIGHT - 1;
							}
							end += shift - start;
							start = shift;

							new_file->Start (time_periods.Format_Step (start));
							new_file->Arrive (time_periods.Format_Step (end));
						}
						if (script_flag) {
							if (activity_flag) {
								activity_file->Reset_Record ();
							}
							keep = (program.Execute () != 0);
						}
					}
					if (keep || all_flag) {
						if (move_org_flag || move_des_flag) {
							flag = false;
						} else {
							flag = keep;
						}
						if (keep) num_kept++;
						keep = true;

						if (move_org_flag) {
							int *ptr = (int *) move_org_map.Get (&origin);
							if (ptr != NULL) {
								new_file->Origin (ptr [1]);
								flag = true;

								int rec [2];
								rec [0] = trip_file->Vehicle ();
								rec [1] = parking [ptr [1]];
								if (!move_veh_map.Add (rec)) {
									Error ("Adding Parking Map");
								}
							}
						}
						if (move_des_flag) {
							int *ptr = (int *) move_des_map.Get (&destination);
							if (ptr != NULL) {
								new_file->Destination (ptr [1]);
								flag = true;
							}
						}
						if (create_flag) {
							new_file->Household (hhold_id);

							if (new_file->Vehicle () > 0) {
								new_file->Vehicle (veh_id);
								origin = new_file->Origin ();

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
							}
							if (newhh_flag) {
								fprintf (newhh_file.File (), "%d\t%d\n", hhold_id, hhold);
							}
							hhold_id++;
						} else if (flag && newhh_flag) {
							fprintf (newhh_file.File (), "%d\n", hhold);
						}
						if (!new_file->Write ()) {
							Error ("Writing %s", new_file->File_Type ());
						}
						num_out++;
					} else {
						keep = false;
					}
				}
			}
			if (next_merge == next_in) {
				if (keep) {
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
		
		//---- write the merge trip to the output file ----
		
		if (save_merge) {
			vehicle = merge_trip_file.Vehicle ();

			if (vehicle_list.Get_Index (vehicle) == 0) {
				if (!vehicle_list.Add (vehicle)) {
					Error ("Adding Vehicle %d to the List", vehicle);
				}
			}
			if (output_flag) {
				new_file->Copy_Fields (&merge_trip_file);

				if (!new_file->Write ()) {
					Error ("Writing %s", new_file->File_Type ());
				}
				num_out++;
			}
		}

		//---- get the next merge trip ----

		if (read_merge) {
			if (merge_trip_file.Read ()) {
				next_merge = merge_trip_file.Household ();
			} else {
				next_merge = MAX_INTEGER;
			}
		}
        			
		//---- get the next input trip ----

		if (read_in) {
			if (trip_file->Read ()) {
				next_in = trip_file->Household ();
			} else {
				next_in = MAX_INTEGER;
			}
		}
	}
	End_Progress ();

	if (file_num == 0 || !split_flag) {
		total_in += Progress_Count ();
	}
	if (trip_file->Extend ()) {
		Print (2, "Number of Trip Records Read from %s = %d", trip_file->Extension (), Progress_Count ());
	} else {
		Print (2, "Number of Trip Records Read = %d", Progress_Count ());
	}
	total_used += num_kept;

	if (output_flag) {
		total_out += num_out;
		if (new_file->Extend ()) {
			Print (1, "Number of Trip Records Written to %s = %d", new_file->Extension (), num_out);
		} else {
			Print (1, "Number of Trip Records Written = %d", num_out);
		}
	}
	if (prob_flag) {
		Print (1, "Number of Trip Records Selected = %d", num_kept);
		if (num_out > 0) Print (0, " (%.1lf%%)", 100.0 * num_kept / num_out);
	}
	if (nparking > 0 && parking) {
		delete [] parking;
	}
	return (true);
}
