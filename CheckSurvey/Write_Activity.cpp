//*********************************************************
//	Write_Activity.cpp - write the person activity
//*********************************************************

#include "CheckSurvey.hpp"

//---------------------------------------------------------
//	Write_Activity
//---------------------------------------------------------

void CheckSurvey::Write_Activity (int weight)
{
	char time [24];
	int i, j, previous_time, hhold, person, duration, start, end, time1, time2, tim, nact, last_loc;
	bool flag;

	Activity_Data *act_ptr, *tmp_ptr;

	Activity_File *act_file = (Activity_File *) Demand_Db_Base (NEW_ACTIVITY);

	//---- write the new survey activity ----

	if (new_act_flag) {

		previous_time = hhold = person = last_loc = 0;
		nact = hhold_act_data.Num_Records ();

		for (i=1; i <= nact; i++) {
			act_ptr = hhold_act_data [i];

			if (hhold != act_ptr->Household () || person != act_ptr->Person ()) {
				if (hhold == act_ptr->Household () && person > act_ptr->Person ()) {
					new_act_file.Notes ("Record Order");
				}
				hhold = act_ptr->Household ();
				person = act_ptr->Person ();
				previous_time = last_loc = 0;
			}
			new_act_file.Household (act_ptr->Household ());
			new_act_file.Person (act_ptr->Person ());
			new_act_file.Activity (act_ptr->Activity ());
			new_act_file.Purpose (act_ptr->Purpose ());
			new_act_file.Priority (act_ptr->Priority ());
			new_act_file.Notes ("");

			start = act_ptr->Start_Time ();

			if (previous_time > 0 && start < previous_time) {
				new_act_file.Notes ("Start Time");
				start = previous_time + 1;
			} else if (act_ptr->Mode () == ALL_WALK) {
				if (start - previous_time > max_walk) {
					new_act_file.Notes ("Walk Time");
					start = previous_time + max_walk;
				}
			} else if (start - previous_time > max_trip) {
				new_act_file.Notes ("Trip Time");
				start = previous_time + max_trip;
			}
			activity_time.Format_Step (start, time);
			new_act_file.Start_Min (time);
			new_act_file.Start_Max (time);

			end = act_ptr->End_Time ();
			if (end <= start) end = start + 1;

			//---- estimate the start time ----

			if (previous_time == 0 && i < nact) {
				tmp_ptr = hhold_act_data [i+1];

				if (tmp_ptr->Start_Time () == end) {
					flag = false;
					time1 = time2 = 0;
					previous_time = end;

					for (j=i+1; j <= nact; j++) {
						tmp_ptr = hhold_act_data [j];
						tim = tmp_ptr->Start_Time () - previous_time;

						if (tmp_ptr->Mode () == ALL_WALK) {
							tim /= 10;
						}
						if (flag) {
							time2 += tim;
						} else {
							time1 += tim;
						}
						if (tmp_ptr->Priority () == PRIMARY_ANCHOR) {
							if (flag) {
								time2 = 0;
							}
							flag = true;
						}
						if (tmp_ptr->Purpose () == 0 || tmp_ptr->Location () == 1) break;
						previous_time = tmp_ptr->End_Time ();
					}
					if (time2 > time1) {
						end -= time2 - time1;
						if (end < 0) end = 0;
					}
					previous_time = 0;
				}
			}
			activity_time.Format_Step (end, time);
			previous_time = end;

			new_act_file.End_Min (time);
			new_act_file.End_Max (time);

			duration = end - start;
			act_ptr->Duration (duration);

			activity_time.Format_Duration (duration, time);

			new_act_file.Time_Min (time);
			new_act_file.Time_Max (time);

			new_act_file.Mode (act_ptr->Mode ());
			new_act_file.Passengers (act_ptr->Passengers ());
			new_act_file.Constraint (act_ptr->Constraint ());

			//---- check the location ----

			if (act_ptr->Location () > 0) {
				if (last_loc == act_ptr->Location ()) {
					new_act_file.Location (act_ptr->Location () + 100);
					new_act_file.Notes ("New Location");
				} else {
					new_act_file.Location (act_ptr->Location ());
				}
			} else {
				new_act_file.Location (-act_ptr->Location ());
				new_act_file.Notes ("Location");
			}
			last_loc = new_act_file.Location ();

			if (act_ptr->Vehicle () == 0) {
				new_act_file.Mode (act_ptr->Mode ());
				new_act_file.Vehicle (0);
			} else {
				new_act_file.Mode (DRIVE_ALONE);

				if (act_ptr->Vehicle () < 0) {
					if (act_ptr->Vehicle () == -1) {
						new_act_file.Notes ("No Vehicle");
						new_act_file.Vehicle (1);
					} else {
						if (act_ptr->Vehicle () == -2) {
							new_act_file.Notes ("No Driver");
						} else if (act_ptr->Vehicle () == -3) {
							new_act_file.Notes ("Shared Location");
							new_act_file.Location (act_ptr->Problem ());
						} else {
							if (act_ptr->Vehicle () == -4) {
								new_act_file.Notes ("Origin Time");
							} else {
								new_act_file.Notes ("Origin Location");
							}
						}
						act_ptr->Problem (0);
						new_act_file.Vehicle (2);
					}
				} else {
					if (act_ptr->Mode () == CARPOOL2) {
						new_act_file.Vehicle (2);
					} else {
						new_act_file.Vehicle (1);
					}
				}
			}
			if (!new_act_file.Write ()) {
				Error ("Writing Activity File");
			}
			num_new++;
		}
	}

	//---- retrieve each activity ----

	previous_time = hhold = person = last_loc = 0;
	flag = false;

	for (act_ptr = hhold_act_data.First (); act_ptr; act_ptr = hhold_act_data.Next ()) {

		if (hhold != act_ptr->Household () || person != act_ptr->Person ()) {
			if (hhold == act_ptr->Household () && person > act_ptr->Person ()) {
				Activity_Problem (SORT_PROBLEM, act_ptr, weight);
				flag = true;
				continue;
			}
			hhold = act_ptr->Household ();
			person = act_ptr->Person ();
			previous_time = last_loc = 0;
		}
		act_file->Household (act_ptr->Household ());
		act_file->Person (act_ptr->Person ());
		act_file->Activity (act_ptr->Activity ());
		act_file->Purpose (act_ptr->Purpose ());
		act_file->Priority (act_ptr->Priority ());

		activity_time.Format_Step (act_ptr->Start_Time (), time);
		if (previous_time > 0 && act_ptr->Start_Time () < previous_time) {
			Activity_Problem (TIME_PROBLEM, act_ptr, weight);
			flag = true;
			continue;
		} else if (act_ptr->Mode () == ALL_WALK) {
			if (act_ptr->Start_Time () - previous_time > max_walk) {
				Activity_Problem (WALK_PROBLEM, act_ptr, weight);
				flag = true;
			}
		} else if (act_ptr->Start_Time () - previous_time > max_trip) {
			Activity_Problem (TIME_PROBLEM, act_ptr, weight);
			flag = true;
		}
		act_file->Start_Min (time);
		act_file->Start_Max (time);

		activity_time.Format_Step (act_ptr->End_Time (), time);
		previous_time = act_ptr->End_Time ();

		act_file->End_Min (time);
		act_file->End_Max (time);

		duration = act_ptr->End_Time () - act_ptr->Start_Time ();
		act_ptr->Duration (duration);

		activity_time.Format_Duration (duration, time);

		act_file->Time_Min (time);
		act_file->Time_Max (time);

		act_file->Mode (act_ptr->Mode ());
		act_file->Passengers (act_ptr->Passengers ());
		act_file->Constraint (act_ptr->Constraint ());

		if (act_ptr->Vehicle () < 0) {
			if (act_ptr->Vehicle () == -1) {
				Activity_Problem (ACCESS_PROBLEM, act_ptr, weight);
				flag = true;
				continue;
			} else {
				if (act_ptr->Vehicle () == -2) {
					Activity_Problem (PASSENGER_PROBLEM, act_ptr, weight);
				} else if (act_ptr->Vehicle () == -4) {
					Activity_Problem (TIME_PROBLEM, act_ptr, weight);
				} else {
					Activity_Problem (LOCATION_PROBLEM, act_ptr, weight);
				}
				flag = true;
				act_file->Vehicle (0);
				act_file->Mode (MAGIC_MOVE);
			}
		} else {
			act_file->Vehicle (act_ptr->Vehicle ());
		}

		if (act_ptr->Location () > 0) {
			if (last_loc == act_ptr->Location ()) {
				Activity_Problem (LOCATION_PROBLEM, act_ptr, weight);
				flag = true;
				continue;
			}
			act_file->Location (act_ptr->Location ());
			last_loc = act_ptr->Location ();
		} else {
			Activity_Problem (LOCATION_PROBLEM, act_ptr, weight);
			flag = true;
			continue;
		}
		if (!act_file->Write ()) {
			Error ("Writing Activity File");
		}
		num_activity++;
	}
	if (flag) {
		nproblem++;
	}
}
