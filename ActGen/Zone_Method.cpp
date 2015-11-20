//*********************************************************
//	Zone_Method.cpp - select the best location within a zone
//*********************************************************

#include "ActGen.hpp"

#include <math.h>

//---------------------------------------------------------
//	Zone_Method
//---------------------------------------------------------

int ActGen::Zone_Method (Purpose_Group *group_ptr, int loc1, int start, int budget1, int loc2, int end, int budget2, int mode)
{
	int loc, loc1_x, loc1_y, loc2_x, loc2_y, loc_x, loc_y, zone1, zone2, zone, distance1, distance2, new_mode; 
	double sum_utility, prob, utility, time1, time2, min_fac1, max_fac1, min_fac2, max_fac2, distance;
	double dx, dy, x1, y1, x2, y2;
	bool time_flag;

	Skim_Group *skim_ptr = 0;
	Db_Base *skim_rec;
	Choice_Data *location_ptr;
	Zone_List *zone_ptr;

	loc = zone = new_mode = 0;

	if (dump_flag) {
		Print (2, "Zone Method Group %d, Start: %d at %d, End: %d at %d, Mode %d",
			group_ptr->Group (), loc1, start, loc2, end, mode);
		Print (1, "Travel Time Budget1 = %d, Budget2 = %d", budget1, budget2);
	}

	//---- initialize the model data -----

	if (group_ptr->Model ()) {
		time_flag = false;
	} else {
		time_flag = (group_ptr->Time_Field () > 0);

		if (time_flag) {
			skim_ptr = skim_group [group_ptr->Time_Skim ()];
		}
	}
	budget1 = Round (budget1 + MIN_TTIME) * RANGE_SPEED;
	if (budget1 < 0) budget1 = MAX_INTEGER;

	budget2 = Round (budget2 + MIN_TTIME) * RANGE_SPEED;
	if (budget2 < 0) budget2 = MAX_INTEGER;

	//---- get the anchor locations ----

	location_ptr = location_data [loc1];

	if (location_ptr == NULL) {
		Error ("Activity Location %d was Not Found", loc1);
	}
	loc1_x = location_ptr->X ();
	loc1_y = location_ptr->Y ();
	zone1 = location_ptr->Zone ();

	zone_ptr = zone_data.Get (zone1);
	if (zone_ptr != NULL) {
		x1 = zone_ptr->X ();
		y1 = zone_ptr->Y ();
	} else {
		x1 = y1 = 0.0;
	}
	if (loc2 != loc1) {
		location_ptr = location_data [loc2];

		if (location_ptr == NULL) {
			Error ("Activity Location %d was Not Found", loc2);
		}
		loc2_x = location_ptr->X ();
		loc2_y = location_ptr->Y ();
		zone2 = location_ptr->Zone ();

		zone_ptr = zone_data.Get (zone2);
		if (zone_ptr != NULL) {
			x2 = zone_ptr->X ();
			y2 = zone_ptr->Y ();
		} else {
			x2 = y2 = 0.0;
		}
	} else {
		loc2_x = loc1_x;
		loc2_y = loc1_y;
		zone2 = zone1;
		x2 = x1;
		y2 = y1;
	}

	//---- process each zone ----

	sum_utility = 0.0;

	for (zone_ptr = zone_data.First (); zone_ptr; zone_ptr = zone_data.Next ()) {

		loc = zone_ptr->Location ();
		if (loc == 0) continue;

		location_ptr = location_data [loc];
		if (location_ptr == NULL) goto pointer_error;

		location_ptr->Utility (0.0);

		//---- check time budgets ----

		distance1 = abs (zone_ptr->X () - loc1_x) + abs (zone_ptr->Y () - loc1_y);
		if (distance1 > budget1) continue;

		distance2 = abs (zone_ptr->X () - loc2_x) + abs (zone_ptr->Y () - loc2_y);
		if (distance2 > budget2) continue;

		//---- prepare the zone data ----

		if (!zone_db.Read_Record (zone_data.Record_Index ())) {
			Error ("Reading Zone Database");
		}
		zone = location_ptr->Zone ();

		//---- check for zone attraction weights ----

		if (group_ptr->Zone_Field () > 0) {
			zone_db.Get_Field (group_ptr->Zone_Field (), &utility);
			if (utility == 0.0) {
				if (dump_flag) {
					//Print (1, "Zone %d Attraction Weight is Zero", zone);
				}
				continue;
			}
			utility *= group_ptr->Zone_Factor () * group_ptr->Balance_Factor (zone);
			if (utility <= 0.0) {
				if (dump_flag) {
					Print (1, "Zone %d Factored Attraction Weight %G is Out of Range", zone, utility);
				}
				continue;
			}
		} else {
			utility = 0.0;
		}

		//---- calculate the distance from location 1 to the zone ----

		if (time_flag) {
			distance1 = 0;
			min_fac1 = max_fac1 = 0.0;
		} else {

			//---- calculate the skim adjustment factors ----

			dx = (double) zone_ptr->X () - loc1_x;
			dy = (double) zone_ptr->Y () - loc1_y;
			min_fac1 = max_fac1 = dx * dx + dy * dy;
			if (zone == zone1) min_fac1 = 0.0;

			dx = (double) zone_ptr->Min_X () - loc1_x;
			dy = (double) zone_ptr->Min_Y () - loc1_y;
			distance = dx * dx + dy * dy;
			if (distance < min_fac1) min_fac1 = distance;
			if (distance > max_fac1) max_fac1 = distance;

			dx = (double) zone_ptr->Min_X () - loc1_x;
			dy = (double) zone_ptr->Max_Y () - loc1_y;
			distance = dx * dx + dy * dy;
			if (distance < min_fac1) min_fac1 = distance;
			if (distance > max_fac1) max_fac1 = distance;

			dx = (double) zone_ptr->Max_X () - loc1_x;
			dy = (double) zone_ptr->Min_Y () - loc1_y;
			distance = dx * dx + dy * dy;
			if (distance < min_fac1) min_fac1 = distance;
			if (distance > max_fac1) max_fac1 = distance;

			dx = (double) zone_ptr->Max_X () - loc1_x;
			dy = (double) zone_ptr->Max_Y () - loc1_y;
			distance = dx * dx + dy * dy;
			if (distance < min_fac1) min_fac1 = distance;
			if (distance > max_fac1) max_fac1 = distance;

			dx = zone_ptr->X () - x1;
			dy = zone_ptr->Y () - y1;
			distance = dx * dx + dy * dy;

			if (distance > 0.0) {
				distance = sqrt (distance);
				min_fac1 = sqrt (min_fac1) / distance;
				max_fac1 = sqrt (max_fac1) / distance;
			} else {
				min_fac1 = 0.0;
				max_fac1 = 1.0;
			}

			//---- distance to the centroid ----

			loc_x = abs (zone_ptr->X () - loc1_x);
			loc_y = abs (zone_ptr->Y () - loc1_y);

			if (distance_method == STRAIGHT) {
				distance = sqrt ((double) loc_x * loc_x + (double) loc_y * loc_y);
			} else if (distance_method == RIGHT_ANGLE) {
				distance = (double) loc_x + loc_y;
			} else {
				distance = (sqrt ((double) loc_x * loc_x + (double) loc_y * loc_y) + loc_x + loc_y) / 2.0;
			}
			distance1 = (int) (distance / Resolution () + 0.5);
		}

		//---- calculate the distance from the zone to location 2 ----

		if (loc1 != loc2) {
			if (time_flag) {
				distance2 = 0;
				min_fac2 = max_fac2 = 0.0;
			} else {

				//---- calculate the skim adjustment factors ----

				dx = (double) zone_ptr->X () - loc2_x;
				dy = (double) zone_ptr->Y () - loc2_y;
				min_fac2 = max_fac2 = dx * dx + dy * dy;
				if (zone == zone2) min_fac2 = 0.0;

				dx = (double) zone_ptr->Min_X () - loc2_x;
				dy = (double) zone_ptr->Min_Y () - loc2_y;
				distance = dx * dx + dy * dy;
				if (distance < min_fac2) min_fac2 = distance;
				if (distance > max_fac2) max_fac2 = distance;

				dx = (double) zone_ptr->Min_X () - loc2_x;
				dy = (double) zone_ptr->Max_Y () - loc2_y;
				distance = dx * dx + dy * dy;
				if (distance < min_fac2) min_fac2 = distance;
				if (distance > max_fac2) max_fac2 = distance;

				dx = (double) zone_ptr->Max_X () - loc2_x;
				dy = (double) zone_ptr->Min_Y () - loc2_y;
				distance = dx * dx + dy * dy;
				if (distance < min_fac2) min_fac2 = distance;
				if (distance > max_fac2) max_fac2 = distance;

				dx = (double) zone_ptr->Max_X () - loc2_x;
				dy = (double) zone_ptr->Max_Y () - loc2_y;
				distance = dx * dx + dy * dy;
				if (distance < min_fac2) min_fac2 = distance;
				if (distance > max_fac2) max_fac2 = distance;

				dx = zone_ptr->X () - x2;
				dy = zone_ptr->Y () - y2;
				distance = dx * dx + dy * dy;

				if (distance > 0.0) {
					distance = sqrt (distance);
					min_fac2 = sqrt (min_fac2) / distance;
					max_fac2 = sqrt (max_fac2) / distance;
				} else {
					min_fac2 = 0.0;
					max_fac2 = 1.0;
				}

				//---- distance to the centroid ----

				loc_x = abs (zone_ptr->X () - loc2_x);
				loc_y = abs (zone_ptr->Y () - loc2_y);

				if (distance_method == STRAIGHT) {
					distance = sqrt ((double) loc_x * loc_x + (double) loc_y * loc_y);
				} else if (distance_method == RIGHT_ANGLE) {
					distance = (double) loc_x + loc_y;
				} else {
					distance = (sqrt ((double) loc_x * loc_x + (double) loc_y * loc_y) + loc_x + loc_y) / 2.0;
				}
				distance2 = (int) (distance / Resolution () + 0.5);
			}
		} else {
			distance2 = distance1;
			min_fac2 = min_fac1;
			max_fac2 = max_fac1;
		}

		//---- apply the model ----

		if (group_ptr->Model ()) {
			tour_info.Put_Field (mode_field, mode);
			tour_info.Put_Field (distance1_field, distance1);
			tour_info.Put_Field (distance2_field, distance2);
			tour_info.Put_Field (utility_field, utility);
			tour_info.Put_Field (min_fac1_field, min_fac1);
			tour_info.Put_Field (max_fac1_field, max_fac1);
			tour_info.Put_Field (min_fac2_field, min_fac2);
			tour_info.Put_Field (max_fac2_field, max_fac2);

			if (dump_flag) {
				Print (1, "Min_Fac1 = %.3lf, Max_Fac1 = %.3lf, Min_Fac2 = %.3lf, Max_Fac2 = %.3lf", 
					min_fac1, max_fac1, min_fac2, max_fac2);
			}

			//---- retrieve the skim records ----

			for (skim_ptr = skim_group.First (); skim_ptr; skim_ptr = skim_group.Next ()) {
				skim_ptr->Get_Record_A (zone1, zone, start);
				skim_ptr->Get_Record_B (zone, zone2, end);

				if (dump_flag) {
					skim_rec = skim_ptr->Skim_A ();
					if (skim_rec != NULL) {
						skim_rec->Get_Field (skim_ptr->Dump_Field (), &time1);
						Print (1, "SkimA%d from %d to %d at %d = %.1lf", skim_ptr->Group (), zone1, zone, start, time1);
					} else {
						Print (1, "SkimA%d from %d to %d at %d is undefined", skim_ptr->Group (), zone1, zone, start);
					}
					skim_rec = skim_ptr->Skim_B ();
					if (skim_rec != NULL) {
						skim_rec->Get_Field (skim_ptr->Dump_Field (), &time2);
						Print (0, "  SkimB%d from %d to %d at %d = %.1lf", skim_ptr->Group (), zone, zone2, end, time2);
					} else {
						Print (0, "  SkimB%d from %d to %d at %d is undefined", skim_ptr->Group (), zone, zone2, end);
					}
				}
			}

			//---- execute the script ----

			if (!group_ptr->Execute ()) {
				if (dump_flag) {
					Print (1, "Script Returned Zero for Zone %d", zone);
				}
				continue;
			}

			tour_info.Get_Field (utility_field, &utility);

			if (utility <= 0.0) {
				if (dump_flag) {
					Print (1, "Zone %d Utility %G is Out of Range", zone, utility);
				}
				continue;
			} else if (dump_flag) {
				Print (1, "Zone %d Utility = %G", zone, utility);
			}
			tour_info.Get_Field (mode_field, &new_mode);
			if (new_mode < ALL_WALK || new_mode >= MAX_MODE) new_mode = DRIVE_ALONE;

		} else {

			//---- get the travel time ----

			if (time_flag) {
				if (!skim_ptr->Get_Record_A (zone1, zone, start)) {
					if (dump_flag) {
						Print (1, "SkimA from %d to %d at %d is undefined", zone1, zone, start);
					}
					continue;
				}

				skim_rec = skim_ptr->Skim_A ();
				if (skim_rec == NULL) continue;

				skim_rec->Get_Field (group_ptr->Time_Field (), &time1);

				if (dump_flag) {
					Print (1, "SkimA from %d to %d at %d = %.1lf", zone1, zone, start, time1);
				}

				if (!skim_ptr->Get_Record_B (zone, zone2, end)) {
					if (dump_flag) {
						Print (0, "  SkimB from %d to %d at %d is undefined", zone, zone2, end);
					}
					continue;
				}

				skim_rec = skim_ptr->Skim_B ();
				if (skim_rec == NULL) continue;

				skim_rec->Get_Field (group_ptr->Time_Field (), &time2);

				if (dump_flag) {
					Print (0, "  SkimB from %d to %d at %d = %.1lf", zone, zone2, end, time2);
				}
				utility *= Exp (group_ptr->Time_Factor (mode) * (time1 + time2));
			} else {
				utility *= Exp (group_ptr->Distance_Factor (mode) * (distance1 + distance2));
			}
			if (utility <= 0.0) {
				if (dump_flag) {
					Print (1, "Zone %d Utility %G is Out of Range", zone, utility);
				}
				continue;
			} else if (dump_flag) {
				Print (1, "Zone %d Utility = %G", zone, utility);
			}
			new_mode = mode;
		}
		sum_utility += utility;

		location_ptr->Mode (new_mode);
		location_ptr->Utility (utility);
	}

	if (sum_utility == 0.0) {
		if (dump_flag) {
			Print (1, "The Zone Utility Sum is Zero");
		}
		return (0);
	}

	//---- select the activity zone ----

	prob = random.Probability () * sum_utility;
	sum_utility = 0.0;

	for (zone_ptr = zone_data.First (); zone_ptr; zone_ptr = zone_data.Next ()) {

		loc = zone_ptr->Location ();
		if (loc == 0) continue;

		location_ptr = location_data [loc];
		if (location_ptr == NULL) goto pointer_error;

		sum_utility += location_ptr->Utility ();
		new_mode = location_ptr->Mode ();

		if (sum_utility >= prob) break;
	}

	if (zone_ptr == NULL) return (0);

	//---- calculate the utility of each location within the zone ----

	sum_utility = 0.0;

	for (; loc > 0; loc = location_ptr->List ()) {

		location_ptr = location_data [loc];
		if (location_ptr == NULL) goto pointer_error;

		if (group_ptr->Location_Field () > 0) {

			if (!location_db.Read_Record (loc)) {
				Error ("Reading Activity Location Database");
			}
			location_db.Get_Field (group_ptr->Location_Field (), &utility);

			if (utility <= 0.0) {
				if (dump_flag) {
					Print (1, "Location %d Distribution Weight %G is Out of Range", 
						location_ptr->Location (), utility);
				}
				location_ptr->Utility (0.0);
				continue;
			}
		} else {
			utility = 1.0;
		}

		//---- calculate the distance from location 1 to the location ----

		if (loc == loc1) {
			distance1 = 0;
		} else {
			loc_x = location_ptr->X () - loc1_x;
			loc_y = location_ptr->Y () - loc1_y;

			if (distance_method == STRAIGHT) {
				distance1 = Resolve ((int) (sqrt ((double) loc_x * loc_x + (double) loc_y * loc_y) + 0.5));
			} else if (distance_method == RIGHT_ANGLE) {
				distance1 = Resolve (abs (loc_x) + abs (loc_y));
			} else {
				distance1 = (int) (sqrt ((double) loc_x * loc_x + (double) loc_y * loc_y) + 0.5);
				loc_x = abs (loc_x) + abs (loc_y);

				if (distance_method == SIMPLE_AVERAGE) {
					distance1 = Resolve ((distance1 + loc_x) / 2);
				} else {
					distance1 = Resolve ((distance1 + loc_x) / 2);
				}
			}
		}

		//---- calculate the distance from the location to location 2 ----

		if (loc1 != loc2) {
			if (loc == loc2) {
				distance2 = 0;
			} else {
				loc_x = location_ptr->X () - loc2_x;
				loc_y = location_ptr->Y () - loc2_y;

				if (distance_method == STRAIGHT) {
					distance2 = Resolve ((int) (sqrt ((double) loc_x * loc_x + (double) loc_y * loc_y) + 0.5));
				} else if (distance_method == RIGHT_ANGLE) {
					distance2 = Resolve (abs (loc_x) + abs (loc_y));
				} else {
					distance2 = (int) (sqrt ((double) loc_x * loc_x + (double) loc_y * loc_y) + 0.5);
					loc_x = abs (loc_x) + abs (loc_y);

					if (distance_method == SIMPLE_AVERAGE) {
						distance2 = Resolve ((distance2 + loc_x) / 2);
					} else {
						distance2 = Resolve ((distance2 + loc_x) / 2);
					}
				}
			}
		} else {
			distance2 = distance1;
		}
		if (new_mode == ALL_WALK) {
			if (distance1 > max_walk) {
				utility = 0;
			} else {
				utility *= (max_walk - distance1 + 1);
			}
		} else {
			utility *= (distance1 + distance2 + 1);
		}
		sum_utility += utility;

		location_ptr->Utility (utility);
		location_ptr->Mode (new_mode);
	}
	if (sum_utility == 0.0) {
		if (dump_flag) {
			Print (1, "Sum of the Location Utilities for Zone %d are Zero", zone);
		}
		return (0);
	}

	//---- select the activity location ----

	prob = random.Probability () * sum_utility;

	sum_utility = 0.0;

	for (loc = zone_ptr->Location (); loc > 0; loc = location_ptr->List ()) {
		location_ptr = location_data [loc];
		if (location_ptr == NULL) goto pointer_error;

		sum_utility += location_ptr->Utility ();

		if (sum_utility >= prob) break;
	}
	return (loc);

pointer_error:
	Error ("Location Pointer is Null");
	return (0);
}
