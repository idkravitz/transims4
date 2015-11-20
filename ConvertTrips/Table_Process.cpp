//*********************************************************
//	Table_Process.cpp - Trip Table Processing
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Table_Processing
//---------------------------------------------------------

void ConvertTrips::Table_Processing (File_Group *group)
{
	int num_share, tod, t, t1, t2, num_t, trp, o, d, period, current, first_t, last_t;
	int total, stat, errors, org, des, trips, num, num_shares, duration, even_bucket, even;
	bool share_flag, factor_flag, period_flag, scale_flag, return_flag;
	double trip, factor, added, deleted, bucket;

	Diurnal_Data *diurnal_ptr;
	Matrix_File *file;
	Factor_Data *factor_ptr;

	static char *error_msg = "%d Trip%sbetween TAZs %d and %d could not be allocated";

	//---- read the trip table ----

	total = errors = 0;
	added = deleted = 0.0;
	file = group->Trip_File ();
	return_flag = (group->Duration () > 0);
	even_bucket = 1;
	bucket = 0.45;

	factor_flag = (group->Trip_Factor () != NULL);
	period_flag = (group->Factor_Periods () > 0);
	scale_flag = (group->Scaling_Factor () != 1.0);

	num_shares = group->Num_Shares ();
	share_flag = (num_shares > 0);
	if (!share_flag) num_share = 1;

	duration = group->Duration ();
	first_t = t1 = 1;
	last_t = t2 = num_t = diurnal_data.Num_Records ();
	period = 0;

	Show_Message (0, "\tReading %s -- Record", file->File_Type ());
	Set_Progress (500);

	while (file->Read ()) {
		Show_Progress ();

		org = file->Origin ();
		if (org == 0) continue;

		if (org < 1 || org > num_zone) {
			Warning ("Origin TAZ %d is Out of Range (1-%d)", org, num_zone);
			continue;
		}
		des = file->Destination ();

		if (des < 1 || des > num_zone) {
			Warning ("Destination TAZ %d is Out of Range (1-%d)", des, num_zone);
			continue;
		}

		//---- check for a time period ----

		if (file->Period_Flag () && period_flag) {
			period = file->Period ();

			if (period > 0) {
				first_t = last_t = 0;

				for (t=1; t <= num_t; t++) {
					diurnal_ptr = diurnal_data [t];
					tod = (diurnal_ptr->Start_Time () + diurnal_ptr->End_Time ()) / 2;

					if (group->Factor_Period (tod) == period) {
						if (first_t == 0) first_t = t;
						last_t = t;
					}
				}
				if (last_t == 0) {
					first_t = 1;
					last_t = num_t;
					period = 0;
				}
			} else {
				first_t = 1;
				last_t = num_t;
				period = 0;
			}
		}

		trips = file->Data ();

		if (trips < 0) {
			Warning ("Number of Trips is Out of Range (%d < 0)", trips);
			continue;
		}

		//---- apply the scaling factor ----

		if (scale_flag) {
			trip = trips * group->Scaling_Factor () + bucket;
			trips = (int) trip;
			if (trips < 0) trips = 0;
			bucket = trip - trips;
		}
		if (trips == 0) continue;
		total += trips;

		//---- apply the selection script ----

		if (share_flag) {
			num = group->Execute ();

			if (num < 1 || num > num_shares) {
				Error ("Diurnal Selection Value %d is Out of Range (1..%d)", num, num_shares);
			}
		} else {
			num = num_share;
		}

		//---- get the travel time ----

		if (skim_flag) {
			skim_ptr = ttime_skim.Get (org, des);
		}

		//---- apply adjustment factors ----

		if (factor_flag) {
			o = (equiv_flag) ? zone_equiv.Zone_Group (org) : org;
			d = (equiv_flag) ? zone_equiv.Zone_Group (des) : des;

			if (period_flag) {
				period = -1;
				t1 = t2 = 1;
				trip = 0.0;

				for (t=first_t; t <= last_t; t++) {
					diurnal_ptr = diurnal_data [t];
					tod = (diurnal_ptr->Start_Time () + diurnal_ptr->End_Time ()) / 2;

					current = group->Factor_Period (tod);

					if (current != period) {
						if (period >= 0) {
							factor_ptr = factor_data.Get (o, d, period);

							if (factor_ptr == NULL) {
								factor_ptr = &default_factor;
							}
							factor = trip * factor_ptr->Factor ();
							if (factor > trip) {
								added += factor - trip;
							} else {
								deleted += trip - factor;
							}
							trp = factor_ptr->Bucket_Factor (trip);

							if (trp > 0 && return_flag) {
								even = (((trp + even_bucket) / 2) * 2);
								even_bucket += trp - even;
								trp = even;
							}
							if (trp > 0) {
								if ((stat = Set_Trips (group, org, des, trp, num, t1, t2, duration))) {
									errors += stat;
									Print (1, error_msg, stat, ((stat > 1) ? "s " : " "), org, des);
								}
							}
						}
						period = current;
						t1 = t;
						trip = 0.0;
					}
					trip += trips * diurnal_ptr->Share (num);
					t2 = t;
				}

			} else {
				if (period == 0) period = 1;
				t1 = first_t;
				t2 = last_t;
				trip = trips;
			}
			factor_ptr = factor_data.Get (o, d, period);

			if (factor_ptr == NULL) {
				factor_ptr = &default_factor;
			}
			factor = trip * factor_ptr->Factor ();
			if (factor > trip) {
				added += factor - trip;
			} else {
				deleted += trip - factor;
			}
			trp = factor_ptr->Bucket_Factor (trip);
		} else {
			t1 = first_t;
			t2 = last_t;
			trp = trips;
		}
		if (trp > 0 && return_flag) {
			even = (((trp + even_bucket) / 2) * 2);
			even_bucket += trp - even;
			trp = even;
		}

		//---- process the trips ----

		if (trp > 0) {
			if ((stat = Set_Trips (group, org, des, trp, num, t1, t2, duration))) {
				errors += stat;
				Print (1, error_msg, stat, ((stat > 1) ? "s " : " "), org, des);
			}
		}
	}
	End_Progress ();

	file->Close ();

	Print (1, "%s has %d Records and %d Trips", file->File_Type (), Progress_Count (), total);

	tot_trips += total;

	if (errors > 0) {
		Warning ("A Total of %d Trip%scould not be allocated", errors, ((errors > 1) ? "s " : " "));

		tot_errors += errors;
	}
	if (factor_flag) {
		Print (1, "Trip Adjustments: %.0lf trips added, %.0lf trips deleted", added, deleted);
		tot_add += added;
		tot_del += deleted;
	}
}
