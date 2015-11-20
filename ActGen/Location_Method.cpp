//*********************************************************
//	Location_Method.cpp - select the best location
//*********************************************************

#include "ActGen.hpp"

#include <math.h>

//---------------------------------------------------------
//	Location_Method
//---------------------------------------------------------

int ActGen::Location_Method (Purpose_Group *group_ptr, int loc1, int start, int budget1, int loc2, int end, int budget2, int mode)
{
	int loc, loc1_x, loc1_y, loc2_x, loc2_y, loc_x, loc_y, location;
	int zone1, zone2, zone;
	int distance1, distance2, new_mode; 
	double sum_utility, prob, utility, time1, time2;
	bool time_flag;

	Skim_Group *skim_ptr = 0;
	Db_Base *skim_rec;
	Choice_Data *location_ptr;
	
	if (dump_flag) {
		Print (2, "Location Method Group %d, Start: %d at %d, End: %d at %d, Mode %d",
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
	tour_info.Put_Field (min_fac1_field, 1.0);
	tour_info.Put_Field (max_fac1_field, 1.0);
	tour_info.Put_Field (min_fac2_field, 1.0);
	tour_info.Put_Field (max_fac2_field, 1.0);

	//---- get the anchor locations ----

	location_ptr = location_data [loc1];

	if (location_ptr == NULL) {
		Error ("Activity Location %d was Not Found", loc1);
	}
	loc1_x = location_ptr->X ();
	loc1_y = location_ptr->Y ();
	zone1 = location_ptr->Zone ();

	if (loc2 != loc1) {
		location_ptr = location_data [loc2];

		if (location_ptr == NULL) {
			Error ("Activity Location %d was Not Found", loc2);
		}
		loc2_x = location_ptr->X ();
		loc2_y = location_ptr->Y ();
		zone2 = location_ptr->Zone ();
	} else {
		loc2_x = loc1_x;
		loc2_y = loc1_y;
		zone2 = zone1;
	}

	//---- process each activity location ----

	sum_utility = 0.0;

	for (location_ptr = location_data.First (); location_ptr; location_ptr = location_data.Next ()) {
		loc = location_data.Record_Index ();

		if (!location_db.Read_Record (loc)) {
			Error ("Reading Activity Location Database");
		}
		location_ptr->Utility (0.0);
		zone = location_ptr->Zone ();
		location = location_ptr->Location ();

		//---- check for location attraction weights ----

		if (group_ptr->Location_Field () > 0) {
			location_db.Get_Field (group_ptr->Location_Field (), &utility);
			if (utility == 0.0) {
				if (dump_flag) {
					//Print (1, "Location %d Attraction Weight is Zero", location);
				}
				continue;
			}
			utility *= group_ptr->Location_Factor ();
			if (utility <= 0.0) {
				if (dump_flag) {
					Print (1, "Location %d Factored Attraction Weight %G is Out of Range", location, utility);
				}
				continue;
			}
		} else {
			utility = 0.0;
		}

		//---- calculate the distance from location 1 to the location ----

		if (time_flag || loc == loc1) {
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
			if (time_flag || loc == loc2) {
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

		//---- apply the model ----

		if (group_ptr->Model ()) {
			tour_info.Put_Field (mode_field, mode);
			tour_info.Put_Field (distance1_field, distance1);
			tour_info.Put_Field (distance2_field, distance2);
			tour_info.Put_Field (utility_field, utility);

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

			//---- execute the model ----

			if (!group_ptr->Execute ()) {
				if (dump_flag) {
					Print (1, "Script Returned Zero for Location %d", location);
				}
				continue;
			}

			tour_info.Get_Field (utility_field, &utility);

			if (utility <= 0.0) {
				if (dump_flag) {
					Print (1, "Location %d Utility %G is Out of Range", location, utility);
				}
				continue;
			} else if (dump_flag) {
				Print (1, "Location %d Utility = %G", location, utility);
			}

			tour_info.Get_Field (mode_field, &new_mode);
			if (new_mode < ALL_WALK || new_mode >= MAX_MODE) new_mode = DRIVE_ALONE;

		} else {

			//---- apply the default model ----

			if (group_ptr->Location_Field () == 0) {
				utility = 1.0 * group_ptr->Location_Factor ();
				if (utility <= 0.0) continue;
			}

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
					Print (1, "SkimA from %d to %d at %d = %lf", zone1, zone, start, time1);
				}

				if (!skim_ptr->Get_Record_B (zone, zone2, end)) {
					if (dump_flag) {
						Print (1, "  SkimB from %d to %d at %d is undefined", zone, zone2, end);
					}
					continue;
				}

				skim_rec = skim_ptr->Skim_B ();
				if (skim_rec == NULL) continue;

				skim_rec->Get_Field (group_ptr->Time_Field (), &time2);

				if (dump_flag) {
					Print (1, "  SkimB from %d to %d at %d = %lf", zone, zone2, end, time2);
				}
				utility *= Exp (group_ptr->Time_Factor (mode) * (time1 + time2));
			} else {
				utility *= Exp (group_ptr->Distance_Factor (mode) * (distance1 + distance2));
			}
			if (utility <= 0.0) {
				if (dump_flag) {
					Print (1, "Location %d Utility %G is Out of Range", location, utility);
				}
				continue;
			} else if (dump_flag) {
				Print (1, "Location %d Utility = %G", location, utility);
			}
			new_mode = mode;
		}
		sum_utility += utility;

		location_ptr->Mode (new_mode);
		location_ptr->Utility (utility);
	}

	if (sum_utility == 0.0) {
		if (dump_flag) {
			Print (1, "The Location Utility Sum is Zero");
		}
		return (0);
	}

	//---- select the activity location ----

	prob = random.Probability () * sum_utility;

	sum_utility = 0.0;

	for (location_ptr = location_data.First (); location_ptr; location_ptr = location_data.Next ()) {
		sum_utility += location_ptr->Utility ();

		if (sum_utility >= prob) break;
	}
	if (location_ptr == NULL) return (0);

	return (location_data.Record_Index ());
}
