//*********************************************************
//	Trip_Check.cpp - Check the Selection Criteria
//*********************************************************

#include "TripPrep.hpp"

#include "In_Polygon.hpp"

#include <math.h>

//---------------------------------------------------------
//	Trip_Check
//---------------------------------------------------------

bool TripPrep::Trip_Check (int hhold, int origin, int destination, int start, int end, int mode, int purpose, int vehicle)
{
	int org, des, period, start_period, end_period;

	Location_Data *loc_ptr;
	Factor_Data *factor_ptr;
	Vehicle_Data *veh_ptr;

	//---- household selection ----

	if (traveler_flag) {
		if (!traveler_range.In_Range (hhold)) return (false);
	}

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
		org = loc_ptr->Zone ();

		if (org_zone_flag) {
			if (!org_zone_range.In_Range (org)) return (false);
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
		des = loc_ptr->Zone ();

		if (des_zone_flag) {
			if (!des_zone_range.In_Range (des)) return (false);
		}

		//---- apply the probability factor ----

		if (factor_flag) {
			if (zone_equiv_flag) {
				origin = zone_equiv.Zone_Group (org);
				destination = zone_equiv.Zone_Group (des);
			} else {
				origin = org;
				destination = des;
			}
			factor_ptr = factor_data.Get (origin, destination, time_equiv.Period ((start + end + 1) / 2));

			if (factor_ptr != NULL) {
				if (!factor_ptr->Bucket_Factor (1.0)) return (false);
			}
		}
	}

	//---- apply selection percentage ----

	if (prob_flag) {
		if (random2.Probability () > probability) return (false);
	}

	//---- add vehicle to the list ----

	if (vehicle > 0) {

		//---- synthetic OD data ----

		if (synod_flag) {
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
		if (vehicle_list.Get_Index (vehicle) == 0) {
			if (!vehicle_list.Add (vehicle)) {
				Error ("Adding Vehicle %d to the List", vehicle);
			}
		}
	}
	return (true);
}
