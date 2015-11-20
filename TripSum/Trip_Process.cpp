//*********************************************************
//	Trip_Process.cpp - Process the Trip Data
//*********************************************************

#include "TripSum.hpp"

#include "In_Polygon.hpp"

#include <math.h>

//---------------------------------------------------------
//	Trip_Process
//---------------------------------------------------------

bool TripSum::Trip_Process (int origin, int destination, int start, int end, int mode, int purpose, int prev_purp, int vehicle, int count)
{
	int org, des, period, start_period, end_period, dir, dx, dy, distance, ttim, purp_code;

	TTime_Data *ttime_ptr;
	Matrix_Data trip_rec, *trip_ptr;
	Location_Data *loc_ptr;
	Link_Data *link_ptr;
	Factor_Data *factor_ptr;
	Point_Time_Data *end_ptr;
	Vehicle_Data *veh_ptr;

	//---- mode selection -----

	if (mode < 0 || mode >= MAX_MODE) {
		Warning ("Mode %d is Out of Range (0..%d)", mode, MAX_MODE-1);
		return (false);
	}
	if (!trip_mode [mode]) return (false);

	//---- purpose selection ----

	if (purpose_flag) {
		if (!purpose_range.In_Range (purpose)) return (false);
	}

	//---- get the mid trip time ----

	start_period = time_periods.In_Index (start);
	end_period = time_periods.In_Index (end);

	period = time_periods.In_Index ((start + end + 1) / 2);

	if (period == 0) {
		period = start_period;

		if (period == 0) {
			period = end_period;
		}
	}
	if (period == 0) return (false);

	//---- set the origin ----

	if (location_flag && origin >= 0 && destination >= 0) {
		loc_ptr = location_data.Get (origin);

		if (loc_ptr == NULL) {
			if (origin != 0) {
				Warning ("Location %d was Not Found in the Location File", origin);
			}
			return (false);
		}
		if (select_org_flag) {
			if (!In_Polygon (UnRound (loc_ptr->X ()), UnRound (loc_ptr->Y ()), &select_origin.points)) return (false);
		}
		dx = loc_ptr->X ();
		dy = loc_ptr->Y ();
		org = loc_ptr->Zone ();

		if (org_zone_flag) {
			if (!org_zone_range.In_Range (org)) return (false);
		}

		//---- link trip ends ----

		if (link_flag) {
			link_ptr = link_data.Get (loc_ptr->Link ());

			if (loc_ptr->Dir ()) {
				dir = link_ptr->BA_Dir ();
			} else {
				dir = link_ptr->AB_Dir ();
			}

			if (dir > 0) {
				ttime_ptr = ttime_data [dir];

				if (ttime_ptr->Periods () == 0) {
					ttime_ptr->Periods (num_inc);
				}
				ttime_ptr->Add_Volume (period, 1);
			}
		}

		//---- location trip ends ----

		if (loc_end_flag && start_period) {
			end_ptr = loc_end_data.Get (origin);

			if (end_ptr == NULL) {
				end_ptr = loc_end_data.New_Record (true);
				if (end_ptr == NULL) goto loc_end_error;

				end_ptr->ID (origin);
				if (!loc_end_data.Add ()) goto loc_end_error;
			}
			if (end_ptr->Periods () == 0) {
				if (!end_ptr->Periods (loc_end_data.Periods ())) goto loc_end_error;
			}
			end_ptr->Add_Out (start_period, 1);
		}

		//---- zone trip ends ----

		if (zone_end_flag && org && start_period) {
			end_ptr = zone_end_data.Get (org);

			if (end_ptr == NULL) {
				end_ptr = zone_end_data.New_Record (true);
				if (end_ptr == NULL) goto zone_end_error;

				end_ptr->ID (org);
				if (!zone_end_data.Add ()) goto zone_end_error;
			}
			if (end_ptr->Periods () == 0) {
				if (!end_ptr->Periods (zone_end_data.Periods ())) goto zone_end_error;
			}
			end_ptr->Add_Out (start_period, 1);
		}

		//---- set the destination ----

		loc_ptr = location_data.Get (destination);

		if (loc_ptr == NULL) {
			if (destination != 0) {
				Warning ("Location %d was Not Found in the Location File", destination);
			}
			return (false);
		}
		if (select_des_flag) {
			if (!In_Polygon (UnRound (loc_ptr->X ()), UnRound (loc_ptr->Y ()), &select_destination.points)) return (false);
		}
		dx -= loc_ptr->X ();
		dy -= loc_ptr->Y ();
		des = loc_ptr->Zone ();

		if (des_zone_flag) {
			if (!des_zone_range.In_Range (des)) return (false);
		}

		//---- link trip ends ----

		if (link_flag) {
			link_ptr = link_data.Get (loc_ptr->Link ());

			if (loc_ptr->Dir ()) {
				dir = link_ptr->BA_Dir ();
			} else {
				dir = link_ptr->AB_Dir ();
			}

			if (dir > 0) {
				ttime_ptr = ttime_data [dir];

				if (ttime_ptr->Periods () == 0) {
					ttime_ptr->Periods (num_inc);
				}
				ttime_ptr->Add_Volume (period, count);
			}
		}

		//---- location trip ends ----

		if (loc_end_flag && end_period) {
			end_ptr = loc_end_data.Get (destination);

			if (end_ptr == NULL) {
				end_ptr = loc_end_data.New_Record (true);
				if (end_ptr == NULL) goto loc_end_error;

				end_ptr->ID (destination);
				if (!loc_end_data.Add ()) goto loc_end_error;
			}
			if (end_ptr->Periods () == 0) {
				if (!end_ptr->Periods (loc_end_data.Periods ())) goto loc_end_error;
			}
			end_ptr->Add_In (end_period, 1);
		}

		//---- zone trip ends ----

		if (zone_end_flag && des && end_period) {
			end_ptr = zone_end_data.Get (des);

			if (end_ptr == NULL) {
				end_ptr = zone_end_data.New_Record (true);
				if (end_ptr == NULL) goto zone_end_error;

				end_ptr->ID (des);
				if (!zone_end_data.Add ()) goto zone_end_error;
			}
			if (end_ptr->Periods () == 0) {
				if (!end_ptr->Periods (zone_end_data.Periods ())) goto zone_end_error;
			}
			end_ptr->Add_In (end_period, 1);
		}

		//---- apply the probability factor ----

		if (factor_flag) {
			factor_ptr = factor_data.Get (org, des, time_equiv.Period ((start + end + 1) / 2));

			if (factor_ptr != NULL) {
				if (!factor_ptr->Bucket_Factor (1.0)) return (false);
			}
		}

		//---- calculate the distance ----

		if (distance_method == STRAIGHT) {
			distance = Resolve ((int) (sqrt ((double) dx * dx + (double) dy * dy) + 0.5));
		} else if (distance_method == RIGHT_ANGLE) {
			distance = Resolve (abs (dx) + abs (dy));
		} else {
			distance = (int) (sqrt ((double) dx * dx + (double) dy * dy) + 0.5);
			dx = abs (dx) + abs (dy);

			if (distance_method == SIMPLE_AVERAGE) {
				distance = Resolve ((distance + dx) / 2);
			} else {
				distance = Resolve ((distance + dx) / 2);
			}
		}

		//---- synthetic OD data ----

		if (synod_flag && vehicle > 0) {
			veh_ptr = vehicle_data.Get (vehicle);

			//---- process trucks ----

			if (veh_ptr->Type () == 2) {
				org_target [org] -= 1;
				des_target [des] -= 1;
				if (org_target [org] < 0) org_target [org] = 0.0;
				if (des_target [des] < 0) des_target [des] = 0.0;
			} else {
				if (hhold_range.In_Range (veh_ptr->Household ())) {
					veh_count [org] [des] += 1;
					next_veh [vehicle_data.Record_Index ()] = first_veh [org] [des];
					first_veh [org] [des] = vehicle_data.Record_Index ();
				}
			}
		}
	} else {
		distance = 0;
		org = des = 0;
	}
	ttim = end - start;

	//---- sum the trip length data ----

	if (length_flag && purpose > 0) {
		if (!trip_length.Add_Trip (purpose, distance, ttim, 0, count)) {
			Error ("Adding Trip Length Data");
		}
	}
	if (time_length_flag) {
		trip_time_length.Add_Trip (mode, prev_purp, purpose, ttim, count);
	}
	if (distance_length_flag) {
		trip_distance_length.Add_Trip (mode, prev_purp, purpose, distance, count);
	}
	if (trip_purpose_flag) {
		purp_code = (prev_purp << 16) + purpose;

		if (!trip_purpose.Add_Trip (purp_code, distance, ttim, 0, count)) {
			Error ("Adding Trip Purpose Data");
		}
	}
	if (mode_flag) {
		if (!mode_length.Add_Trip (mode, distance, ttim, 0, count)) {
			Error ("Adding Mode Length Data");
		}
	}
	if (mode_purpose_flag) {
		purp_code = (mode << 16) + purpose;

		if (!mode_purpose.Add_Trip (purp_code, distance, ttim, 0, count)) {
			Error ("Adding Mode Purpose Data");
		}
	}
	prev_purp = purpose;

	//---- trip table data -----

	if (table_flag && org > 0 && des > 0) {

		if (zone_flag) {
			org = zone_equiv.Zone_Group (org);
			des = zone_equiv.Zone_Group (des);
		}
		if (org > 0 && des > 0) {
			trip_rec.ID (trip_table.Key (org, des, period));
			trip_rec.Data (count);

			if (trip_rec.ID () == 0) {
				Error ("Trip Table Index %d-%d-%d is Out of Range", org, des, period);
			}
			trip_ptr = trip_table.Get (&trip_rec);

			if (trip_ptr == NULL) {
				if (!trip_table.Add (&trip_rec)) {
					Error ("Adding Trip Table Record");
				}
			} else {
				trip_ptr->Add_Data (count);
			}
		}
	}
	if (vehicle > 0) {
		if (vehicle_list.Get_Index (vehicle) == 0) {
			if (!vehicle_list.Add (vehicle)) {
				Error ("Adding Vehicle %d to the List", vehicle);
			}
		}
	}

	//---- increment the trip time counters ----

	if (time_flag) {
		period = time_periods.In_Index ((start + end + 1) / 2);

		if (period > 0) {
			(*(mid_time.Record (period))) += count;
		}

		//---- find the start time increment ----

		period = time_periods.In_Index (start);

		if (period > 0) {
			(*(start_time.Record (period))) += count;
		}

		//---- find the end time increment ----

		period = time_periods.In_Index (end);

		if (period > 0) {
			(*(end_time.Record (period))) += count;
		}
	}
	return (true);

loc_end_error:
	Error ("Adding Location Trip End Data");
	return (false);

zone_end_error:
	Error ("Adding Zone Trip End Data");
	return (false);
}
