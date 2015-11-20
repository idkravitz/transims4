//*********************************************************
//	Write_Skim.cpp - Write Skim file
//*********************************************************

#include "PlanSum.hpp"

#include <math.h>

//---------------------------------------------------------
//	Write_Skim
//---------------------------------------------------------

void PlanSum::Write_Skim (void)
{
	bool version3_flag, transit_flag, drive_flag, other_flag;

	int org, des, period, count, round, mode, last_org, last_period, min_count;
	double time, variance, min_time, min_walk, min_wait, min_transit, min_drive, min_other, min_cost;
	double min_length;

	Skim_Data *skim_ptr;

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
		transit_flag = drive_flag = other_flag = false;
	} else {
		transit_flag = (skim_mode [TRANSIT] || skim_mode [RAIL_TRANSIT] ||
						skim_mode [PNR_OUT] || skim_mode [PNR_IN]);

		drive_flag = (skim_mode [DRIVE_ALONE] || skim_mode [PNR_OUT] || 
					skim_mode [PNR_IN] || skim_mode [CARPOOL2] || 
					skim_mode [CARPOOL3] || skim_mode [CARPOOL4]);

		other_flag = (skim_mode [BICYCLE] || skim_mode [MAGIC_MOVE] || 
					skim_mode [SCHOOL_BUS]);
	}

	//---- write the records ----

	Show_Message ("Writing %s -- Record", skim_file.File_Type ());
	Set_Progress (10000);
	
	last_org = last_period = min_count = 0;
	min_time = min_walk = min_wait = min_transit = min_drive = min_other = min_cost = 0.0;

	for (skim_ptr = skim_data.First_Key (); skim_ptr; skim_ptr = skim_data.Next_Key ()) {
		Show_Progress ();

		count = skim_ptr->Count ();

		if (count == 0) continue;

		skim_data.Key (skim_ptr->ID (), &org, &des, &period);

		if (neighbor_flag) {
			if (last_period != period || last_org != org) {
				if (last_org > 0 && min_count > 0) {
					skim_file.Origin (last_org);
					skim_file.Destination (last_org);
					skim_file.Period (last_period);
					skim_file.Count (min_count);

					if (version3_flag) {
						skim_file.Time (min_time);

						time = min_walk + min_wait + min_transit;
						variance = min_drive + min_other;

						variance = fabs (time - variance);

						skim_file.Variance (variance);

					} else {
						skim_file.Walk ((int) (min_walk + 0.5));
					
						if (transit_flag) {
							skim_file.Wait ((int) (min_wait + 0.5));
							skim_file.Transit ((int) (min_transit + 0.5));
						} 
						if (drive_flag) {
							skim_file.Drive ((int) (min_drive + 0.5));
						}
						if (other_flag) {
							skim_file.Other ((int) (min_other + 0.5));
						}
						skim_file.Length ((int) (min_length + 0.5));

						if (transit_flag || drive_flag) {
							skim_file.Cost ((int) (min_cost + 0.5));
						}
					}
					if (!skim_file.Write ()) goto error_msg;
				}
				last_period = period;
				last_org = org;
				min_count = 0;
				min_time = min_walk = min_wait = min_transit = min_drive = min_other = min_cost = 0.0;
				min_length = 0.0;
			}
			if (org == des) continue;

			time = skim_ptr->Walk () + skim_ptr->Wait () + skim_ptr->Transit () + skim_ptr->Drive () + skim_ptr->Other ();

			time = neighbor_factor * time / count;
			if (time < min_time || min_time == 0.0) {
				min_count = count;
				min_time = time;
				min_walk = neighbor_factor * skim_ptr->Walk () / count;
				min_wait = neighbor_factor * skim_ptr->Wait () / count;
				min_transit = neighbor_factor * skim_ptr->Transit () / count;
				min_drive = neighbor_factor * skim_ptr->Drive () / count;
				min_other = neighbor_factor * skim_ptr->Other () / count;
				min_cost = neighbor_factor * skim_ptr->Cost () / count;
				min_length = neighbor_factor * skim_ptr->Length () / count;
			}
		}
		round = count / 2;

		skim_file.Origin (org);
		skim_file.Destination (des);
		skim_file.Period (period);
		skim_file.Count (count);

		if (version3_flag) {
			time = skim_ptr->Walk () + skim_ptr->Wait () + skim_ptr->Transit () + skim_ptr->Drive () + skim_ptr->Other ();

			skim_file.Time (time / count);

			time = skim_ptr->Walk () + skim_ptr->Wait () + skim_ptr->Transit ();
			variance = skim_ptr->Drive () + skim_ptr->Other ();

			variance = fabs (time - variance) / count;

			skim_file.Variance (variance);

		} else {
			skim_file.Walk ((skim_ptr->Walk () + round) / count);
		
			if (transit_flag) {
				skim_file.Wait ((skim_ptr->Wait () + round) / count);
				skim_file.Transit ((skim_ptr->Transit () + round) / count);
			} 
			if (drive_flag) {
				skim_file.Drive ((skim_ptr->Drive () + round) / count);
			}
			if (other_flag) {
				skim_file.Other ((skim_ptr->Other () + round) / count);
			}
			skim_file.Length ((skim_ptr->Length () + round) / count);

			if (transit_flag || drive_flag) {
				skim_file.Cost ((skim_ptr->Cost () + round) / count);
			}
		}
		if (!skim_file.Write ()) goto error_msg;
	}

	//---- write the last intrazonal ----

	if (last_org > 0 && min_count > 0) {
		skim_file.Origin (last_org);
		skim_file.Destination (last_org);
		skim_file.Period (last_period);
		skim_file.Count (min_count);

		if (version3_flag) {
			skim_file.Time (min_time);

			time = min_walk + min_wait + min_transit;
			variance = min_drive + min_other;

			variance = fabs (time - variance);

			skim_file.Variance (variance);

		} else {
			skim_file.Walk ((int) (min_walk + 0.5));
		
			if (transit_flag) {
				skim_file.Wait ((int) (min_wait + 0.5));
				skim_file.Transit ((int) (min_transit + 0.5));
			} 
			if (drive_flag) {
				skim_file.Drive ((int) (min_drive + 0.5));
			}
			if (other_flag) {
				skim_file.Other ((int) (min_other + 0.5));
			}
			skim_file.Length ((int) (min_length + 0.5));

			if (transit_flag || drive_flag) {
				skim_file.Cost ((int) (min_cost + 0.5));
			}
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
