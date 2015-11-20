//*********************************************************
//	Read_Events.cpp - Process Event Records
//*********************************************************

#include "EventSum.hpp"

//---------------------------------------------------------
//	Read_Events
//---------------------------------------------------------

void EventSum::Read_Events (void)
{
	int hhold, person, trip, type, num_rec, period, schedule, actual, link, diff, leg, mode;
	bool add_flag;

	Event_Data *event_ptr, event_rec;
	Event_Time *time_ptr;
	Link_Event *link_ptr, link_rec;

	//----- initialize the time summary memory ----

	if (!compare_flag) {
		num_rec = event_file.Estimate_Records () / 2;
		if (!event_data.Max_Records (num_rec)) {
			Error ("Insufficient Memory for %d Event Records", num_rec);
		}
	}
	if (time_sum_flag) {
		if (!event_time.Num_Records (num_inc)) {
			Error ("Insufficient Memory for %d Event Times", num_inc);
		}
	}

	//---- read the event file ----

	Show_Message ("Reading %s -- Record", event_file.File_Type ());
	Set_Progress ();

	num_rec = 0;

	while (event_file.Read ()) {
		Show_Progress ();

		//---- check the event type ----

		type = event_file.Event ();
		if (type != START_EVENT && type != END_EVENT) continue;

		//---- check the household id ----

		hhold = event_file.Household ();
		if (hhold <= 0) continue;
		if (hhold > max_hhold) {
			Error ("Household %d is Out of Range (1..%d)", hhold, max_hhold);
		}
		if (range_flag) {
			if (!hhold_range.In_Range (hhold)) continue;
		}
		if (hhlist_flag) {
			if (hhlist_data.Get (hhold) == NULL) continue;
		}
		mode = event_file.Mode ();
		if (!plan_modes [mode]) continue;

		//---- get the event times ----

		schedule = time_periods.Step (event_file.Schedule ());
		actual = time_periods.Step (event_file.Actual ());
		link = abs (event_file.Link ());

		//---- add to link and period totals ----

		if (link_flag) {
			period = time_periods.In_Index (schedule);

			if (period > 0) {
				link_rec.Link (link);
				link_rec.Period (period);

				link_ptr = link_event.Get (&link_rec);

				if (link_ptr == NULL) {
					link_ptr = link_event.New_Record (true);
					if (link_ptr == NULL) goto link_error;

					link_ptr->Link (link);
					link_ptr->Period (period);

					if (!link_event.Add ()) goto link_error;
				}
				if (type == START_EVENT) {
					link_ptr->Add_Plan_Start ();
				} else {
					link_ptr->Add_Plan_End ();
				}
			}
		}

		//---- add to link and period totals ----

		if (link_flag) {
			period = time_periods.In_Index (actual);

			if (period > 0) {
				diff = actual - schedule;

				link_rec.Link (link);
				link_rec.Period (period);

				link_ptr = link_event.Get (&link_rec);

				if (link_ptr == NULL) {
					link_ptr = link_event.New_Record (true);
					if (link_ptr == NULL) goto mem_error;

					link_ptr->Link (link);
					link_ptr->Period (period);

					if (!link_event.Add ()) goto mem_error;
				}
				if (type == START_EVENT) {
					link_ptr->Add_Start_Diff (diff);
				} else {
					link_ptr->Add_End_Diff (diff);
				}
			}
		}

		//---- save the trip data ----

		person = event_file.Person ();
		trip = event_file.Trip ();
		leg = event_file.Leg ();

		add_flag = true;

		if (compare_flag || type == END_EVENT) {
			event_rec.Household (hhold);
			event_rec.Person (person);
			event_rec.Trip (trip);
			event_rec.Leg (leg);

			event_ptr = event_data.Get (&event_rec);

			if (event_ptr != NULL) {
				add_flag = false;
			} else {
				event_rec.Leg (0);
				event_ptr = event_data.Get (&event_rec);
				if (event_ptr == NULL) continue;

				add_flag = false;
			}
		}
		if (add_flag) {
			event_ptr = event_data.New_Record (true);
			if (event_ptr == NULL) goto mem_error;

			event_ptr->Household (hhold);
			event_ptr->Person (person);
			event_ptr->Trip (trip);
			event_ptr->Leg (leg);
			event_ptr->Mode (0);
			event_ptr->Purpose (0);
			event_ptr->Constraint (FIXED_TIME);

			if (!event_data.Add ()) goto mem_error;
		}

		//---- convert from plan mode to trip mode ----

		if (add_flag || !compare_flag) {
			switch (mode) {
				case AUTO_MODE:
					if (event_ptr->Mode () == NO_MODE || event_ptr->Mode () == ALL_WALK) {
						event_ptr->Mode (DRIVE_ALONE);
					} else if (event_ptr->Mode () == TRANSIT) {
						event_ptr->Mode (PNR_IN);
					}
					break;
				case TRANSIT_MODE:
					if (event_ptr->Mode () == NO_MODE || event_ptr->Mode () == ALL_WALK) {
						event_ptr->Mode (TRANSIT);
					} else if (event_ptr->Mode () == DRIVE_ALONE) {
						event_ptr->Mode (PNR_OUT);
					} else if (event_ptr->Mode () == CARPOOL2) {
						event_ptr->Mode (KNR_OUT);
					}
					break;
				case WALK_MODE:
					if (event_ptr->Mode () == NO_MODE) event_ptr->Mode (ALL_WALK);
					break;
				case BIKE_MODE:
					if (event_ptr->Mode () == NO_MODE || event_ptr->Mode () == ALL_WALK) {
						event_ptr->Mode (BICYCLE);
					}
					break;
				case OTHER_MODE:
				case DRIVER_MODE:
					if (event_ptr->Mode () == NO_MODE || event_ptr->Mode () == ALL_WALK) {
						event_ptr->Mode (SCHOOL_BUS);
					}
					break;
				case MAGIC_MODE:
					if (event_ptr->Mode () == NO_MODE || event_ptr->Mode () == ALL_WALK) {
						event_ptr->Mode (MAGIC_MOVE);
					}
					break;
				case CARPOOL_MODE:
					if (event_ptr->Mode () == NO_MODE || event_ptr->Mode () == ALL_WALK) {
						event_ptr->Mode (CARPOOL2);
					} else if (event_ptr->Mode () == TRANSIT) {
						event_ptr->Mode (KNR_IN);
					}
					break;
			}
		}

		//---- save the trip start time ----

		if (type == START_EVENT) {
			if (add_flag || (compare_flag && link == event_ptr->Start_Link ())) {
				if (!trip_file_flag && !activity_flag) {
					event_ptr->Trip_Start (schedule);
				}
				if (!time_in_flag) {
					event_ptr->Base_Start (schedule);
				}
				event_ptr->Started (actual);
				event_ptr->Start_Link (link);

			}
		} else {
			if (!add_flag || (compare_flag && link == event_ptr->End_Link ())) {

				//---- complete the trip ----

				if (!trip_file_flag && !activity_flag) {
					event_ptr->Trip_End (schedule);
				}
				if (!time_in_flag) {
					event_ptr->Base_End (schedule);
				}
				event_ptr->Ended (actual);
				event_ptr->End_Link (link);
			}

			//---- process complete trips ----

			if (event_ptr->Ended () > event_ptr->Started ()) {

				//---- add to the time summary ----

				if (time_sum_flag) {

					//---- process the start time ----

					schedule = event_ptr->Trip_Start ();
					actual = event_ptr->Started ();

					period = time_periods.In_Index (schedule);

					if (period > 0) {
						time_ptr = event_time [period];

						if (time_ptr != NULL) {
							time_ptr->Period (period);
							time_ptr->Add_Trip_Start ();
						}
					}
					period = time_periods.In_Index (actual);

					if (period > 0) {
						diff = actual - schedule;

						time_ptr = event_time [period];

						if (time_ptr != NULL) {
							time_ptr->Period (period);
							time_ptr->Add_Start_Diff (diff);

						}
					}

					//---- process the end time ----

					schedule = event_ptr->Trip_End ();
					actual = event_ptr->Ended ();

					period = time_periods.In_Index (schedule);

					if (period > 0) {
						time_ptr = event_time [period];

						if (time_ptr != NULL) {
							time_ptr->Period (period);
							time_ptr->Add_Trip_End ();
						}
					}
					period = time_periods.In_Index (actual);

					if (period > 0) {
						diff = actual - schedule;

						time_ptr = event_time [period];

						if (time_ptr != NULL) {
							time_ptr->Period (period);
							time_ptr->Add_End_Diff (diff);

						}
					}

					//---- travel time difference ----
					
					actual = event_ptr->Mid_Trip ();

					period = time_periods.In_Index (actual);

					if (period > 0) {
						time_ptr = event_time [period];

						if (time_ptr != NULL) {
							diff = event_ptr->Travel_Time () - event_ptr->Trip_Time ();

							time_ptr->Add_Travel_Time (event_ptr->Travel_Time (), diff);

							diff = abs (event_ptr->Trip_Start_Diff ()) + abs (event_ptr->Trip_End_Diff ());

							if (diff >= min_variance) {
								time_ptr->Add_Sum_Error (diff);
							}
						}
					}
				}

				//---- add to the time distribution -----

				if ((time_flag || period_flag) && 
					event_ptr->Base_Start () < event_ptr->Base_End () &&
					event_ptr->Started () < event_ptr->Ended ()) {

					time_diff.Add_Trip (event_ptr->Mid_Trip (), event_ptr->Travel_Time (), event_ptr->Base_Time ());
				}
			}
		}
		num_rec++;
	}
	End_Progress ();

	event_file.Close ();
	
	Print (2, "Number of %s Records = %d", event_file.File_Type (), Progress_Count ());
	Print (1, "Number of Event Records Processed = %d", num_rec);

	event_data.Max_Records (event_data.Num_Records ());

	return;

mem_error:
	Error ("Insufficient Memory for Event Data");

link_error:
	Error ("Insufficient Memory for Link Event Data");
}
