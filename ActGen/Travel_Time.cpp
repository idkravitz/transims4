//*********************************************************
//	Travel_Time.cpp - estimate travel time
//*********************************************************

#include "ActGen.hpp"

#include <math.h>

//---------------------------------------------------------
//	Travel_Time
//---------------------------------------------------------

void ActGen::Travel_Time (void)
{
	int i, j, nact, home_x, home_y, loc_x, loc_y, person, time, ttim, mode, dx, dy, distance, purpose;
	int tour_distance, tour_time, tour_purp, home_loc, loc, home_zone, zone0, zone, skim, field;
	int zone_dist, x1, y1, x2, y2, zone_dx, zone_dy, home_zx, home_zy, home_dx, home_dy;
	int purp_code, prev_purp, stops;
	double tim;
	bool home_flag;

	Location_Data *location_ptr;
	Activity_Data *act_ptr, *loc_ptr;
	Skim_Group *skim_ptr;
	Db_Base *skim_rec;
	Zone_List *zone_ptr;

	nact = hhold_act_data.Num_Records ();

	//---- calculate travel times ----

	person = loc_x = loc_y = home_x = home_y = home_loc = loc = home_zone = zone = 0;
	tour_purp = tour_distance = tour_time = prev_purp = stops = time = 0;
	home_zx = home_zy = x2 = y2 = home_dx = home_dy = zone_dx = zone_dy = 0;
	home_flag = false;

	for (act_ptr = hhold_act_data.First (); act_ptr; act_ptr = hhold_act_data.Next ()) {
		if (dump_flag && !home_flag) {
			Print (2, "Calculate Travel Times for Household %d", act_ptr->Household ());
		}

		//---- get the current location ----

		dx = loc_x;
		dy = loc_y;
		zone0 = zone;

		x1 = x2;
		y1 = y2;

		purpose = act_ptr->Purpose ();

		if (purpose == 0) {
			if (!home_flag) {
				location_ptr = location_data [act_ptr->Location ()];
				if (location_ptr == NULL) {
					Error ("Activity Location %d was Not Found", act_ptr->Location ());
				}
				home_x = location_ptr->X ();
				home_y = location_ptr->Y ();
				home_loc = location_ptr->Location ();
				home_zone = location_ptr->Zone ();
				home_flag = true;

				if (zone_flag) {
					zone_ptr = zone_data.Get (home_zone);

					if (zone_ptr != NULL) {
						home_zx = zone_ptr->X ();
						home_zy = zone_ptr->Y ();

						home_dx = zone_ptr->Max_X ();
						home_dy = zone_ptr->Max_Y ();
						home_dx = (home_dx - zone_ptr->Min_X () + 1) / 2;
						home_dy = (home_dy - zone_ptr->Min_Y () + 1) / 2;
					} else {
						home_zx = home_zy = home_dx = home_dy = 0;
					}
				}
			}

			//---- sum the tour length data ----

			if (tour_flag && tour_purp > 0) {
				if (!tour_length.Add_Trip (tour_purp, tour_distance, tour_time)) {
					Error ("Adding Tour Length Data");
				}
			}
			if (tour_purpose_flag && tour_purp > 0) {
				purp_code = (tour_purp << 16) + stops;

				if (!tour_purpose.Add_Trip (purp_code, tour_distance, tour_time)) {
					Error ("Adding Tour Purpose Data");
				}
			}
			loc_x = home_x;
			loc_y = home_y;
			loc = home_loc;
			zone = home_zone;
			x2 = home_zx;
			y2 = home_zy;
			zone_dx = home_dx;
			zone_dy = home_dy;
			tour_distance = tour_time = tour_purp = stops = 0;
		} else {
			location_ptr = location_data [act_ptr->Location ()];
			if (location_ptr == NULL) {
				Activity_Problem (LOCATION_PROBLEM, act_ptr);
				continue;
			}
			loc_x = location_ptr->X ();
			loc_y = location_ptr->Y ();
			loc = location_ptr->Location ();
			zone = location_ptr->Zone ();
			stops++;

			if (zone_flag) {
				zone_ptr = zone_data.Get (zone);

				if (zone_ptr != NULL) {
					x2 = zone_ptr->X ();
					y2 = zone_ptr->Y ();

					zone_dx = zone_ptr->Max_X ();
					zone_dy = zone_ptr->Max_Y ();
					zone_dx = (zone_dx - zone_ptr->Min_X () + 1) / 2;
					zone_dy = (zone_dy - zone_ptr->Min_Y () + 1) / 2;
				} else {
					x2 = y2 = 0;
				}
			}
		}

		//---- check for a new person or a passenger ----

		if (person != act_ptr->Person () || act_ptr->Mode () >= CARPOOL2) {
			person = act_ptr->Person ();
			act_ptr->Duration (0);
			time = 0;
			prev_purp = purpose;
			continue;
		}

		//---- get the distance ----

		dx -= loc_x;
		dy -= loc_y;

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

		//---- apply the travel time model ----

		mode = act_ptr->Mode ();
		if (mode < ALL_WALK || mode >= MAX_MODE) mode = DRIVE_ALONE;
			
		ttim = distance * Resolution () / avg_speed [mode];

		if (time_field [mode] > 0) {
			field = time_field [mode];
			skim = (field >> 16);
			field -= (skim << 16);

			skim_ptr = skim_group [skim];

			if (skim_ptr != NULL) {
				skim_ptr->Get_Record_A (zone0, zone, time);
				skim_rec = skim_ptr->Skim_A ();

				if (skim_rec != NULL) {
					skim_rec->Get_Field (field, &tim);

					if (zone_flag) {
						if ((x1 != 0 || y1 != 0) && (x2 != 0 || y2 != 0)) {
							if (zone0 == zone) {
								x1 = zone_dx;
								y1 = zone_dy;
							} else {
								x1 -= x2;
								y1 -= y2;
							}
							if (distance_method == STRAIGHT) {
								zone_dist = Resolve ((int) (sqrt ((double) x1 * x1 + (double) y1 * y1) + 0.5));
							} else if (distance_method == RIGHT_ANGLE) {
								zone_dist = Resolve (abs (x1) + abs (y1));
							} else {
								zone_dist = (int) (sqrt ((double) x1 * x1 + (double) y1 * y1) + 0.5);
								x1 = abs (x1) + abs (y1);

								if (distance_method == SIMPLE_AVERAGE) {
									zone_dist = Resolve ((zone_dist + x1) / 2);
								} else {
									zone_dist = Resolve ((zone_dist + x1) / 2);
								}
							}
							if (zone_dist > 0) {
								tim *= (double) distance / zone_dist;
							}
						}
					}
					ttim = (int) (tim + 0.5);
				}
			}
		}
		time = act_ptr->Start_Time () - time;

		if (ttim >= time) {
			act_ptr->Duration (ttim);
		} else {
			act_ptr->Duration (ttim + add_time [mode]);

			if (act_ptr->Duration () > time) {
				act_ptr->Duration (time);
			}
		}
		time = act_ptr->End_Time ();

		if (dump_flag) {
			Print (1, "Activity=%d, Location=%d, Mode=%d, Distance=%d, Time=%d", act_ptr->Activity (), 
				loc, mode, distance, act_ptr->Duration ());
		}

		//---- sum the trip length data ----

		if (length_flag && purpose > 0) {
			if (!trip_length.Add_Trip (purpose, distance, ttim)) {
				Error ("Adding Trip Length Data");
			}
		}
		if (tour_flag || tour_purpose_flag) {
			if (act_ptr->Priority () == PRIMARY_ANCHOR) {
				tour_purp = purpose;
			}
			tour_distance += distance;
			tour_time += ttim;
		}
		if (time_length_flag) {
			trip_time_length.Add_Trip (mode, prev_purp, purpose, ttim);
		}
		if (distance_length_flag) {
			trip_distance_length.Add_Trip (mode, prev_purp, purpose, distance);
		}
		if (trip_purpose_flag) {
			purp_code = (prev_purp << 16) + purpose;

			if (!trip_purpose.Add_Trip (purp_code, distance, ttim)) {
				Error ("Adding Trip Purpose Data");
			}
		}
		if (mode_flag) {
			if (!mode_length.Add_Trip (mode, distance, ttim)) {
				Error ("Adding Mode Length Data");
			}
		}
		if (mode_purpose_flag) {
			purp_code = (mode << 16) + purpose;

			if (!mode_purpose.Add_Trip (purp_code, distance, ttim)) {
				Error ("Adding Mode Purpose Data");
			}
		}
		prev_purp = purpose;
	}

	//---- set passenger travel times ----

	for (i=1; i <= nact; i++) {
		act_ptr = hhold_act_data [i];

		if (act_ptr->Mode () >= CARPOOL2) {
			for (j=1; j <= nact; j++) {
				loc_ptr = hhold_act_data [j];

				if (loc_ptr->Mode () == DRIVE_ALONE &&
					loc_ptr->Vehicle () == act_ptr->Vehicle () &&
					loc_ptr->Start_Time () == act_ptr->Start_Time ()) {

					act_ptr->Duration (loc_ptr->Duration ());
					break;
				}
			}
		}
	}
}
