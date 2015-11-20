//*********************************************************
//	Transit_Leg.cpp - process a transit leg
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	Transit_Leg
//---------------------------------------------------------

bool SubareaPlans::Transit_Leg (void)
{
	int i, n1, n2, link, time;
	int location;
	int stop1, stop2, route, line1, line2, run, stops;
	bool flag, boarding_flag;

	Boundary *boundary_ptr;
	Line_Data *line_ptr, *new_line_ptr;
	Stop_Data *stop_ptr;
	Plan_Data *plan_ptr;

	plan_ptr = input_plans.Plan ();

	route = input_plans.Route ();
	new_line_ptr = new_line_data.Get (route);
	if (new_line_ptr == NULL) return (false);

	line_ptr = line_data.Get (route);

	//---- find the stop index ----

	stop1 = input_plans.Start_ID ();
	stop2 = input_plans.End_ID ();
	boarding_flag = (stop1 == boarding_stop);
	flag = false;

	for (i=1; i <= line_ptr->Stops (); i++) {
		if (flag) {
			if (line_ptr->Stop (i) == stop2) {
				stop2 = i;
				break;
			}

			//---- check for loop routes ----

			if (line_ptr->Stop (i) == input_plans.Start_ID ()) {
				stop1 = i;
			}
		} else {
			if (line_ptr->Stop (i) == stop1) {
				stop1 = i;
				flag = true;
			}
		}
	}
	if (i > line_ptr->Stops ()) {
		Warning ("Traveler %d Route %d from Stop %d to %d was Not Found", 
			current_traveler, line_ptr->Route (), input_plans.Start_ID (), input_plans.End_ID ());
		return (false);
	}

	//---- check if the leg is in the subarea ----

	n1 = line_ptr->New_Stop (stop1);
	n2 = line_ptr->New_Stop (stop2);

	if (n1 == 0 && n2 == 0) return (false);

	line1 = line_ptr->New_Line (stop1);
	line2 = line_ptr->New_Line (stop2);

	if (n1 < 0 && n2 < 0 && line1 == line2) return (false);

	//---- check for a boarding stop when exiting the subarea ----

	if (n1 > 0 && n2 < 0 && !boarding_flag) return (false);

	//---- find the run number ----

	time = Round (input_plans.Time ());

	for (run=1; run <= line_ptr->Runs (); run++) {
		if (time <= line_ptr->Schedule (run, stop1)) break;
	}
	if (run > line_ptr->Runs ()) {
		Warning ("No Runs Start after %d at Stop %d on Route %d for Traveler %d", 
			Resolve (time), line_ptr->Stop (stop1), route, current_traveler);
		return (false);
	}

	//---- copy the plan ----

	if (!subarea_plans.Plan (plan_ptr)) {
		Error ("Coping Plan Record");
	}

	subarea_plans.Trip (current_trip);
	subarea_plans.Leg (current_leg);

	if (line1 != route) {
		new_line_ptr = new_line_data.Get (line1);
		subarea_plans.Route (new_line_ptr->Route ());
	}

	//---- process the boarding stop ----

	if (n1 <= 0) {
		if (n1 < 0) {
			new_line_ptr = new_line_data.Get (line2);
		}

		//---- get the first stop ----

		stop1 = new_line_ptr->Stop (1);
		time = Resolve (new_line_ptr->Schedule (run, 1));

		stop_ptr = stop_data.Get (stop1);
		link = stop_ptr->Link ();

		boundary_ptr = (Boundary *) boundary.Get (&link);
		if (boundary_ptr == NULL) goto boundary_error;

		//---- start the trip at the boundary ----

		location = boundary_ptr->in_location;

		subarea_plans.Leg (current_leg);
		subarea_plans.Time (time-1);
		subarea_plans.Start_ID (location);
		subarea_plans.Start_Type (LOCATION_ID);
		subarea_plans.Driver_Flag (0);
		subarea_plans.Mode (Db_Code::WALK_MODE);
		subarea_plans.End_ID (stop1);
		subarea_plans.End_Type (STOP_ID);
		subarea_plans.Stop_Time (time);
		subarea_plans.Duration (1);
		subarea_plans.GCF (0);
		subarea_plans.Tokens (0);

		Save_Plans ();
		current_leg++;

		subarea_plans.Plan (plan_ptr);
			
		subarea_plans.Route (new_line_ptr->Route ());
		subarea_plans.Trip (current_trip);
		subarea_plans.Leg (current_leg);
		subarea_plans.Time (time);
		subarea_plans.Start_ID (stop1);
	}

	//---- process a multi-line trip ----

	if (n1 >= 0 && line1 != line2) {

		//---- get the last stop ----

		stops = new_line_ptr->Stops ();

		stop1 = new_line_ptr->Stop (stops);
		time = Resolve (new_line_ptr->Schedule (run, stops));

		subarea_plans.End_ID (stop1);
		subarea_plans.Stop_Time (time);

		Save_Plans ();
		current_leg++;

		//---- write the exit leg ----

		subarea_plans.Plan (plan_ptr);

		stop_ptr = stop_data.Get (stop1);
		link = stop_ptr->Link ();

		boundary_ptr = (Boundary *) boundary.Get (&link);
		if (boundary_ptr == NULL) goto boundary_error;

		location = boundary_ptr->out_location;
	
		subarea_plans.Trip (current_trip);
		subarea_plans.Leg (current_leg);
		subarea_plans.Time (time);
		subarea_plans.Start_ID (stop1);
		subarea_plans.Start_Type (STOP_ID);
		subarea_plans.Driver_Flag (0);
		subarea_plans.Mode (Db_Code::WALK_MODE);
		subarea_plans.End_ID (location);
		subarea_plans.End_Type (LOCATION_ID);
		subarea_plans.Stop_Time (time+1);
		subarea_plans.Duration (1);
		subarea_plans.GCF (0);
		subarea_plans.Tokens (0);

		Save_Plans ();
		current_leg++;
		last_location = location;
		last_time = time + 1;

		//---- get the new line ----

		new_line_ptr = new_line_data.Get (line2);

		stop1 = new_line_ptr->Stop (1);
		time = Resolve (new_line_ptr->Schedule (run, 1));

		stop_ptr = stop_data.Get (stop1);
		link = stop_ptr->Link ();

		boundary_ptr = (Boundary *) boundary.Get (&link);
		if (boundary_ptr == NULL) goto boundary_error;	

		location = boundary_ptr->in_location;

		//---- add a magic move ----

		if (!subarea_plans.Plan (plan_ptr)) {
			Error ("Coping Plan Record");
		}
		subarea_plans.Leg (current_leg);
		subarea_plans.Time (last_time);
		subarea_plans.Start_ID (last_location);
		subarea_plans.Start_Type (LOCATION_ID);
		subarea_plans.Driver_Flag (0);
		subarea_plans.Mode (Db_Code::MAGIC_MODE);
		subarea_plans.End_ID (location);
		subarea_plans.End_Type (LOCATION_ID);
		if (last_time > time - 2) {
			subarea_plans.Stop_Time (last_time+1);
		} else {
			subarea_plans.Stop_Time (time - 1);
		}
		subarea_plans.Duration (subarea_plans.Stop_Time () - last_time);
		subarea_plans.GCF (0);
		subarea_plans.Tokens (0);

		Save_Plans ();
		current_leg++;
		last_time = subarea_plans.Stop_Time ();
		last_location = location;

		//---- start the next transit leg ----

		subarea_plans.Trip (current_trip);
		subarea_plans.Leg (current_leg);
		subarea_plans.Time (last_time);
		subarea_plans.Start_ID (last_location);
		subarea_plans.Start_Type (LOCATION_ID);
		subarea_plans.Driver_Flag (0);
		subarea_plans.Mode (Db_Code::WALK_MODE);
		subarea_plans.End_ID (stop1);
		subarea_plans.End_Type (STOP_ID);
		subarea_plans.Stop_Time (time);
		subarea_plans.Duration (1);
		subarea_plans.GCF (0);
		subarea_plans.Tokens (0);

		Save_Plans ();
		current_leg++;

		subarea_plans.Plan (plan_ptr);
			
		subarea_plans.Route (new_line_ptr->Route ());
		subarea_plans.Trip (current_trip);
		subarea_plans.Leg (current_leg);
		subarea_plans.Time (time);
		subarea_plans.Start_ID (stop1);
	}

	//---- process the alighting stop ----

	if (n2 < 0) {

		//---- get the last stop ----

		stops = new_line_ptr->Stops ();

		stop2 = new_line_ptr->Stop (stops);
		time = Resolve (new_line_ptr->Schedule (run, stops));

		subarea_plans.End_ID (stop2);
		subarea_plans.Stop_Time (time);

		Save_Plans ();
		current_leg++;

		//---- write the exit leg ----

		stop_ptr = stop_data.Get (stop2);
		link = stop_ptr->Link ();

		boundary_ptr = (Boundary *) boundary.Get (&link);
		if (boundary_ptr == NULL) goto boundary_error;

		location = boundary_ptr->out_location;

		subarea_plans.Leg (current_leg);
		subarea_plans.Time (time);
		subarea_plans.Start_ID (stop2);
		subarea_plans.Start_Type (STOP_ID);
		subarea_plans.Driver_Flag (0);
		subarea_plans.Mode (Db_Code::WALK_MODE);
		subarea_plans.End_ID (location);
		subarea_plans.End_Type (LOCATION_ID);
		subarea_plans.Stop_Time (time+1);
		subarea_plans.Duration (1);
		subarea_plans.GCF (0);
		subarea_plans.Tokens (0);
	}

	//---- adjust the duration and impedance values ----

	time = subarea_plans.Duration ();
	subarea_plans.Duration (subarea_plans.Stop_Time () - subarea_plans.Time ());
	if (subarea_plans.Duration () != time && subarea_plans.Duration () > 0) {
		subarea_plans.GCF (subarea_plans.GCF () * time / subarea_plans.Duration ());
	}

	//---- write the plan records ----

	if (Save_Plans ()) {
		if (n1 > 0) {
			if (n2 > 0) {
				leg_in_in++;
			} else {
				leg_in_out++;
			}
		} else {
			if (n2 > 0) {
				leg_out_in++;
			} else {
				leg_out_out++;
			}
		}
	}
	current_leg++;

	return (n2 > 0);

boundary_error:
	Error ("Boundary Link %d was Not Found", link);
	return (false);
}
