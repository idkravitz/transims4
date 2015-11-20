//*********************************************************
//	Trip_Process.cpp - process a trip record
//*********************************************************

#include "ModeChoice.hpp"

#include <math.h>

//---------------------------------------------------------
//	Trip_Process
//---------------------------------------------------------

int ModeChoice::Trip_Process (Travel_Leg &leg)
{
	int i, duration, distance, ttime, xa, ya, xb, yb, tod;
	double dx, dy;

	Model_Group *model_ptr;
	Location_Data *loc_ptr;
	Zone_Data *zone_ptr;

	//---- get the model ----

	model_ptr = trip_group.Model (leg.purpose);

	if (model_ptr == NULL) return (0);

	//---- retrieve origin and destination activity location records ----

	loc_ptr = location_data.Get (leg.origin);
	if (loc_ptr == NULL) return (0);

	org_loc.Record (location_db [location_data.Record_Index ()]);

	xa = loc_ptr->X ();
	ya = loc_ptr->Y ();
	leg.org_zone = loc_ptr->Zone ();

	loc_ptr = location_data.Get (leg.destination);
	if (loc_ptr == NULL) return (0);

	des_loc.Record (location_db [location_data.Record_Index ()]);

	xb = loc_ptr->X ();
	yb = loc_ptr->Y ();
	leg.des_zone = loc_ptr->Zone ();

	//---- calculate distance ----

	dx = UnRound (xb - xa);
	dy = UnRound (yb - ya);

	if (distance_method == STRAIGHT) {
		distance = (int) (sqrt (dx * dx + dy * dy) + 0.5);
	} else if (distance_method == RIGHT_ANGLE) {
		distance = (int) (fabs (dx) + fabs (dy) + 0.5);
	} else {
		distance = (int) (((sqrt (dx * dx + dy * dy) + fabs (dx) + fabs (dy)) / 2.0) + 0.5);
	}

	//---- retrieve origin and destination zone records ----

	if (zone_flag) {
		zone_ptr = zone_data.Get (leg.org_zone);
		if (zone_ptr == NULL) {
			org_zone.Reset_Record ();
		} else {
			org_zone.Record (zone_db [zone_data.Record_Index ()]);
		}

		zone_ptr = zone_data.Get (leg.des_zone);
		if (zone_ptr == NULL) {
			des_zone.Reset_Record ();
		} else {
			des_zone.Record (zone_db [zone_data.Record_Index ()]);
		}
	}

	//---- retrieve skim records ----

	tod = (leg.start + leg.end) / 2;

	for (i=1; i <= skim_group.Num_Records (); i++) {
		skim_group [i]->Get_Record (leg.org_zone, leg.des_zone, tod);
	}

	//---- set trip attribute fields ----

	duration = ttime = 0;

	trip_info.Put_Field (trip_number, leg.trip);
	trip_info.Put_Field (trip_mode, leg.mode);
	trip_info.Put_Field (trip_purpose, leg.purpose);
	trip_info.Put_Field (trip_start, leg.start);
	trip_info.Put_Field (trip_end, leg.end);
	trip_info.Put_Field (trip_duration, duration);
	trip_info.Put_Field (trip_distance, distance);
	trip_info.Put_Field (trip_ttime, ttime);

	//---- apply the trip model by trip purpose ----

	return (model_ptr->Execute ());
}
