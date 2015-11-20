//*********************************************************
//	Survey_Activity.cpp - convert the survey activity
//*********************************************************

#include "NewFormat.hpp"

#include "Activity_File.hpp"
#include "Data_Array.hpp"

//---------------------------------------------------------
//	Survey_Activity
//---------------------------------------------------------

void NewFormat::Survey_Activity (void)
{
	int nact, nhhold, nperson, ntour, ntrip, hhold, person, lvalue, num, duration;
	int last_location, x_field, y_field, code_field, home_id, last_id, loc_id, min_loc, max_loc;
	int start, end, t1, t2, num_zero, num_x_fix, num_y_fix;
	double factor, min_x, min_y, max_x, max_y;
	char time_buffer [24];
	bool home_flag, zero_flag, skip_hhold;

	Activity_File *activity_file;

	XY_Point point, *ptr, home, last, anchor;

	Data_Array stops (sizeof (point), 20);

	activity_file = (Activity_File *) Demand_Db_Base (NEW_ACTIVITY);

	x_field = activity_file->Field_Number ("X_COORD");
	y_field = activity_file->Field_Number ("Y_COORD");
	code_field = activity_file->Field_Number ("CODE");

	//---- get the coordinate and location ranges ----

	if (loc_flag) {	
		Location_Data *loc_ptr;
		Link_Data *link_ptr;

		max_x = max_y = -100000000.0;
		min_x = min_y = 100000000.0;
		max_loc = 0;
		min_loc = MAX_INTEGER;

		for (loc_ptr = location_data.First (); loc_ptr; loc_ptr = location_data.Next ()) {
			if (loc_ptr->X () < min_x) min_x = loc_ptr->X ();
			if (loc_ptr->X () > max_x) max_x = loc_ptr->X ();
			if (loc_ptr->Y () < min_y) min_y = loc_ptr->Y ();
			if (loc_ptr->Y () > max_y) max_y = loc_ptr->Y ();

			if (loc_ptr->Zone () > 0) {
				link_ptr = link_data.Get (loc_ptr->Link ());

				if (link_ptr != NULL) {
					if (link_ptr->Type () != EXTERNAL) {
						if (loc_ptr->Location () < min_loc) min_loc = loc_ptr->Location ();
						if (loc_ptr->Location () > max_loc) max_loc = loc_ptr->Location ();
					}
				}
			}
		}
		min_x = UnRound ((int) min_x);
		min_y = UnRound ((int) min_y);
		max_x = UnRound ((int) max_x);
		max_y = UnRound ((int) max_y);
		if (max_loc == 0) {
			min_loc = 0;
			max_loc = MAX_INTEGER;
		}
		Print (2, "X Coordinate Range = %.2lf to %.2lf", min_x, max_x);
		Print (1, "Y Coordinate Range = %.2lf to %.2lf", min_y, max_y);
		Print (1, "Internal Activity Location Range = %d to %d", min_loc, max_loc);
	} else {
		min_x = min_y = -100000000.0;
		max_x = max_y = 100000000.0;

	}

	//---- read each record ----

	nact = nhhold = nperson = ntour = ntrip = num_zero = num_x_fix = num_y_fix = 0;
	hhold = person = num = home_id = last_id = start = end = t1 = t2 = 0;
	last_location = 1;
	home_flag = true;
	zero_flag = skip_hhold = false;
	anchor.x = 0;
	anchor.y = 0;
	factor = 1.0;

	Show_Message ("Reading %s -- Record", survey_file.File_Type ());
	Set_Progress ();

	while (survey_file.Read ()) {
		Show_Progress ();

		//---- household ----

		survey_file.Get_Field (1, &lvalue);

		//---- interpolate zero points ----

		if (zero_flag && !skip_hhold) {
			if (lvalue == hhold && (anchor.x != 0 || anchor.y != 0)) {

				survey_file.Get_Field (12, &(point.x));
				survey_file.Get_Field (13, &(point.y));

				if (point.x != 9999 && point.y != 9999) {
					if (!projection.Convert (&(point.x), &(point.y))) {
						Error ("Converting Coordinate %lf, %lf", point.x, point.y);
					}

					//---- travel time ----

					survey_file.Get_Field (10, &start);
					if (start < 0) start = 0;

					start *= 60;	//---- minutes to seconds ----
					t2 = end - start;

					t2 = t1 + t2;
					if (t2 > 0) {
						factor = (double) t1 / t2;
					} else {
						factor = 0.5;
					}

					//---- interpolate point ----

					point.x = anchor.x + factor * (point.x - anchor.x);
					point.y = anchor.y + factor * (point.y - anchor.y);

					for (ptr = (XY_Point *) stops.First (); ptr; ptr = (XY_Point *) stops.Next ()) {
						if (ptr->x == 0 && ptr->y == 0) continue;
						if (ptr->x == point.x && ptr->y == point.y) break;
					}
					if (ptr == NULL) {
						if (!stops.Add (&point)) {
							Error ("Adding Location to the Point Array");
						}
					}
					last_location = stops.Record_Index () + 1;
					last = point;
					if (loc_flag) {
						last_id = Closest_Location (&point);

						activity_file->Location (last_id);
						activity_file->Put_Field (code_field, last_location);
					} else {
						activity_file->Location (last_location);
					}

					//---- add coordinate ----

					activity_file->Put_Field (x_field, point.x);
					activity_file->Put_Field (y_field, point.y);
					zero_flag = false;
				}
			}
			if (!activity_file->Write ()) {
				Error ("Writing Activity Record");
			}
			if (zero_flag) num_zero++;
		}

		//---- initialize a new hhold ----

		if (lvalue != hhold) {
			nhhold++;
			hhold = lvalue;
			person = 0;
			num = 1;
			home_flag = true;
			stops.Reset ();

			survey_file.Get_Field (12, &(home.x));
			survey_file.Get_Field (13, &(home.y));

			if (home.x == 9999 && home.y == 9999) {
				home.x = 0;
				home.y = 0;
				home_id = 0;
			} else {
				if (!projection.Convert (&(home.x), &(home.y))) {
					Error ("Converting Coordinate %lf, %lf", home.x, home.y);
				}
				if (loc_flag) {
					home_id = Closest_Location (&home);
				}
			}
			last = home;
			last_id = home_id;
			anchor.x = 0;
			anchor.y = 0;

			skip_hhold = (home.x < min_x || home.x > max_x || 
				home.y < min_y || home.y > max_y || 
				home_id < min_loc || home_id > max_loc);
		}
		if (skip_hhold) {
			continue;
		}
		activity_file->Household (lvalue);

		//---- person ----

		survey_file.Get_Field (2, &lvalue);

		if (lvalue != person) {
			nperson++;
			person = lvalue;
			num = 1;
			home_flag = true;
			last = home;
			last_id = home_id;
		}
		activity_file->Person (lvalue);

		//---- activity ----

		activity_file->Activity (num);
		num++;

		//---- purpose ----

		survey_file.Get_Field (4, &lvalue);

		if (lvalue == 0) {
			if (!home_flag) ntour++;
			home_flag = true;
		} else {
			home_flag = false;
			ntrip++;
		}
		activity_file->Purpose (lvalue);

		//---- at home ----

		survey_file.Get_Field (5, &lvalue);

		if (lvalue != 1) {

			//---- were you there ----

			survey_file.Get_Field (6, &lvalue);

			if (lvalue != 1) {

				//---- check previous locations ----

				survey_file.Get_Field (12, &(point.x));
				survey_file.Get_Field (13, &(point.y));

				if (point.x == 9999 && point.y == 9999) {
					point.x = 0;
					point.y = 0;
					ptr = NULL;
				} else {
					if (!projection.Convert (&(point.x), &(point.y))) {
						Error ("Converting Coordinate %lf, %lf", point.x, point.y);
					}
					if (point.x < min_x || point.x > max_x) {
						if (anchor.x > 0) {
							point.x = (anchor.x * 2 + home.x) / 3.0;
						} else {
							point.x = home.x + ((num % 2) ? 1500.0 : -1500.0);
						}
						num_x_fix++;
					}
					if (point.y < min_y || point.y > max_y) {
						if (anchor.y > 0) {
							point.y = (anchor.y * 2 + home.y) / 3.0;
						} else {
							point.y = home.y + ((num % 3) ? 1500.0 : -1500.0);
						}
						num_y_fix++;
					}
					for (ptr = (XY_Point *) stops.First (); ptr; ptr = (XY_Point *) stops.Next ()) {
						if (ptr->x == 0 && ptr->y == 0) continue;
						if (ptr->x == point.x && ptr->y == point.y) break;
					}
				}
				if (ptr == NULL) {
					if (!stops.Add (&point)) {
						Error ("Adding Location to the Point Array");
					}
				}
				last_location = stops.Record_Index () + 1;
				last = point;
				if (loc_flag) {
					last_id = Closest_Location (&point);
				}
			}
			lvalue = last_location;
			point = last;
			loc_id = last_id;
		} else {
			point = last = home;
			loc_id = last_id = home_id;
		}
		zero_flag = (point.x == 0 && point.y == 0);
		if (!zero_flag) anchor = point;

		//---- save the location ----

		if (loc_flag) {
			activity_file->Location (loc_id);
			activity_file->Put_Field (code_field, lvalue);
		} else {
			activity_file->Location (lvalue);
		}

		//---- add coordinate ----

		activity_file->Put_Field (x_field, point.x);
		activity_file->Put_Field (y_field, point.y);

		//---- mode ----

		survey_file.Get_Field (7, &lvalue);

		activity_file->Mode (lvalue);

		//---- driver ----

		survey_file.Get_Field (8, &lvalue);

		activity_file->Vehicle (lvalue);

		//---- passengers ----

		survey_file.Get_Field (9, &lvalue);

		if (lvalue > 0) lvalue--;

		activity_file->Passengers (lvalue);

		//---- start time ----

		survey_file.Get_Field (10, &start);
		if (start < 0) start = 0;

		start *= 60;	//---- minutes to seconds ----

		activity_time.Format_Step (start, time_buffer);

		activity_file->Start_Min (time_buffer);
		activity_file->Start_Max (time_buffer);

		duration = start;
		t1 = start - end;

		//---- end time ----

		survey_file.Get_Field (11, &end);
		if (end < 0) end = 0;

		end *= 60;	//---- minutes to seconds ----

		if (end <= duration) {
			end = duration + 1;
		}
		activity_time.Format_Step (end, time_buffer);

		activity_file->End_Min (time_buffer);
		activity_file->End_Max (time_buffer);

		//---- duration ----

		duration = end - duration;

		activity_time.Format_Duration (duration, time_buffer);

		activity_file->Time_Min (time_buffer);
		activity_file->Time_Max (time_buffer);

		//---- save the activity record ----

		if (!zero_flag) {
			if (!activity_file->Write ()) {
				Error ("Writing Activity Record");
			}
		}
		nact++;
	}
	End_Progress ();

	if (zero_flag && !skip_hhold) {
		if (!activity_file->Write ()) {
			Error ("Writing Activity Record");
		}
		num_zero++;
	}
	survey_file.Close ();

	Print (2, "Number of %s Records = %d", survey_file.File_Type (), Progress_Count ());
	Print (1, "Number of Zero Locations = %d", num_zero);
	Print (1, "Number of X Coordinate Corrections = %d", num_x_fix);
	Print (1, "Number of Y Coordinate Corrections = %d", num_y_fix);

	activity_file->Close ();

	Print (1);
	Write (1, "Number of %s Records = %d", activity_file->File_Type (), nact);
	Write (1, "\tNumber of Households = %d", nhhold);
	Write (1, "\tNumber of Persons = %d", nperson);
	Write (1, "\tNumber of Tours = %d", ntour);
	Write (1, "\tNumber of Trips = %d", ntrip);
	Show_Message (1);
}
