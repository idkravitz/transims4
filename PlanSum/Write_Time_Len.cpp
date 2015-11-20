//*********************************************************
//	Write_Time_Len.cpp - Write Time Length file
//*********************************************************

#include "PlanSum.hpp"

#include <math.h>

//---------------------------------------------------------
//	Write_Time_Length
//---------------------------------------------------------

void PlanSum::Write_Time_Length (void)
{
	bool version3_flag;

	int org, des, period, count, mode, last_org, last_period, min_count;
	double min_time, min_length, time;

	Time_Len_Skim_Data *tlen_ptr;

	version3_flag = (skim_file.Dbase_Format () == VERSION3);

	if (version3_flag) {
		for (mode=1; mode < MAX_MODE; mode++) {
			if (skim_mode [mode] == true) break;
		}
		if (mode < MAX_MODE) {
			skim_file.Mode (mode);
		} else {
			skim_file.Mode (0);
		}
	}

	//---- write the records ----

	Show_Message ("Writing %s -- Record", skim_file.File_Type ());
	Set_Progress (10000);

	last_org = last_period = min_count = 0;
	min_time = min_length = 0.0;

	for (tlen_ptr = time_len_skim_data.First_Key (); tlen_ptr; tlen_ptr = time_len_skim_data.Next_Key ()) {
		Show_Progress ();

		count = tlen_ptr->count;

		if (count == 0) continue;

		time_len_skim_data.Key (tlen_ptr->id, &org, &des, &period);

		if (neighbor_flag) {
			if (last_period != period || last_org != org) {
				if (last_org > 0 && min_count > 0) {
					skim_file.Origin (last_org);
					skim_file.Destination (last_org);
					skim_file.Period (last_period);

					if (version3_flag) {
						skim_file.Count (min_count);
						skim_file.Time (min_time);
					} else {
						skim_file.Total_Time ((int) (min_time + 0.5));
						skim_file.Length ((int) (min_length + 0.5));
					}
					if (!skim_file.Write ()) goto error_msg;
				}
				last_period = period;
				last_org = org;
				min_count = 0;
				min_time = 0.0;
				min_length = 0.0;
			}
			if (org == des) continue;

			time = neighbor_factor * tlen_ptr->time / count;
			if (time < min_time || min_time == 0.0) {
				min_count = count;
				min_time = time;
				min_length = neighbor_factor * tlen_ptr->length / count;
			}
		}
		skim_file.Origin (org);
		skim_file.Destination (des);
		skim_file.Period (period);

		if (version3_flag) {
			skim_file.Count (count);
			skim_file.Time ((double) tlen_ptr->time / count);
		} else {
			skim_file.Total_Time ((tlen_ptr->time + (count / 2)) / count);
			skim_file.Length ((tlen_ptr->length + (count / 2)) / count);
		}
		if (!skim_file.Write ()) goto error_msg;
	}

	//---- write the last intrazonal ----

	if (last_org > 0 && min_count > 0) {
		skim_file.Origin (last_org);
		skim_file.Destination (last_org);
		skim_file.Period (last_period);

		if (version3_flag) {
			skim_file.Count (min_count);
			skim_file.Time (min_time);
		} else {
			skim_file.Total_Time ((int) (min_time + 0.5));
			skim_file.Length ((int) (min_length + 0.5));
		}
		if (!skim_file.Write ()) goto error_msg;
	}
	End_Progress ();

	skim_file.Close ();

	Print (2, "Number of %s Records = %d", skim_file.File_Type (), Progress_Count ());
	return;

error_msg:
	Error ("Writing %s", skim_file.File_Type ());
} 
