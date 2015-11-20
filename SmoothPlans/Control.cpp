//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "SmoothPlans.hpp"

#include "Utility.hpp"
#include "Scanf.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void SmoothPlans::Program_Control (void)
{
	int i, j;
	double dvalue, factor;
	char *str_ptr;
	
	output_flag = (Get_Control_String (NEW_PLAN_FILE) != NULL);

	//---- open network files ----

	Demand_Service::Program_Control ();

	//---- open the plan file ----
	
	if (Get_Control_String (NODE_LIST_PATHS) != NULL) {
		type_flag = Get_Control_Flag (NODE_LIST_PATHS);
	}
	str_ptr = Get_Control_String (PLAN_FILE);

	if (str_ptr == NULL) goto control_error;

	str_ptr = Project_Filename (str_ptr, Extension ());

	Print_Filename (2, plan_file.File_Type (), str_ptr);

	plan_file.Filename (str_ptr);

	str_ptr = Get_Control_String (PLAN_FORMAT);

	if (str_ptr != NULL) {
		plan_file.File_Format (str_ptr);
		Print (1, "%s Format = %s", plan_file.File_Type (), str_ptr);
	}
	plan_file.Node_Based_Flag (type_flag);

	if (!plan_file.Open (0)) {
		File_Error ("Opening Plan File", plan_file.Filename ());
	}
	type_flag = plan_file.Node_Based_Flag ();

	//---- node list paths ----

	if (type_flag) {
		Print (1, "Plan File contains Node List Paths");
	} else {
		Print (1, "Plan File contains Link List Paths");
	}

	//---- link list ----

	str_ptr = Get_Control_String (LINK_LIST);

	if (str_ptr != NULL) {
		Print (1);
		list_file.File_Type ("Link List");

		list_file.Open (Project_Filename (str_ptr));
		link_flag = true;

		if (!Network_File_Flag (LINK) || !Network_File_Flag (NODE) || !Network_File_Flag (PARKING)) {
			Error ("Network Link, Node, and Parking Files are Required");
		}
	} else {
		Print (2, "Plans will be Smoothed based on Start Time");

		Network_File_False (LINK);
		Network_File_False (NODE);
		Network_File_False (PARKING);
		Network_File_False (LANE_CONNECTIVITY);
		Demand_File_False (LINK_DELAY);
	}

	//---- random number seed ----

	str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

	if (str_ptr != NULL) {
		random.Seed (atoi (str_ptr));
	}
	Print (2, "Random Number Seed = %d", random.Seed ());

	//---- get the time of day format ----
	
	str_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

	if (str_ptr == NULL) {
		str_ptr = "SECONDS";
	}
	if (!time_step.Format (str_ptr)) {
		Error ("Time of Day Format %s was Unrecognized", str_ptr);
	}
	Print (2, "Time of Day Format = %s", str_ptr);

	//---- read the time increment ----

	str_ptr = Get_Control_String (TIME_INCREMENT);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &increment);

		if (increment < 1 || increment > 60) {
			Error ("Time Increment %d is Out of Range (1..60)", increment);
		}
	}
	Print (1, "Time Increment = %d minute%s", increment, ((increment > 1) ? "s" : " "));

	increment *= 60;

	nperiod = (MIDNIGHT + increment - 1) / increment;

	period_index.Num_Records (nperiod);

	//---- read the max time shift ----

	str_ptr = Get_Control_String (MAXIMUM_TIME_SHIFT);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &max_change);

		if (max_change < 5 || max_change > 120) {
			Error ("Maximum Time Shift %d is Out of Range (5..120)", max_change);
		}
	}
	Print (1, "Maximum Time Shift = %d minutes", max_change);

	max_change = max_change * 60 / increment;
	if (max_change < 1) {
		Error ("Minimum Time Shift and Time Increment are Incompatible");
	}

	//---- read the number smooth records ----

	str_ptr = Get_Control_String (SMOOTH_GROUP_SIZE);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &naverage);

		if (naverage < 3 || naverage > 9) {
			Error ("Smooth Group Size %d is Out of Range (1..9)", naverage);
		}
		if ((naverage % 2) != 1) {
			Error ("Smooth Group Size %d must be an Odd Number", naverage);
		}
	}
	Print (1, "Smooth Group Size = %d", naverage);

	dvalue = 100.0 - 50.0 / naverage;

	//---- read the percent distributed forward ----

	str_ptr = Get_Control_String (PERCENT_MOVED_FORWARD);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &forward);
	
		if (forward < 0.0 || forward > dvalue) {
			Error ("Forward Percentage %.1lf is Out of Range (0..%.0lf)", forward, dvalue);
		}
	}
	Print (1, "Percent Distributed Forward = %.1lf percent", forward);

	//---- read the percent distributed backwarde ----
	
	str_ptr = Get_Control_String (PERCENT_MOVED_BACKWARD);

	if (str_ptr != NULL) {
		Get_Double (str_ptr, &backward);

		if (backward < 0.0 || backward > dvalue) {
			Error ("Backward Percentage %.1lf is Out of Range (0..%.0lf)", backward, dvalue);
		}
	}
	Print (1, "Percent Distributed Backward = %.1lf percent", backward);

	factor = forward + backward;
	dvalue = 100.0 - 100.0 / naverage;

	if (factor < 5.0 || factor > dvalue) {
		Error ("Combined Distribution Percentage %.1lf is Out of Range (5..%.0lf)", factor, dvalue);
	}	

	//---- number of iterations ----

	str_ptr = Get_Control_String (NUMBER_OF_ITERATIONS);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &niter);

		if (niter < 1 || niter > 100) {
			Error ("Number of Iterations %d is Out of Range (1-100)", niter);
		}
	}
	Print (1, "Number of Iterations = %d", niter);

	//---- create the distribution ----

	distribution = new double [naverage];

	factor = 0.0;
	naverage = naverage / 2;

	for (i=1; i <= naverage; i++) {
		factor += i;
	}
	factor = 1.0 / factor;
	forward /= 100.0;
	backward /= 100.0;

	distribution [naverage] = 1.0 - forward - backward;

	forward *= factor;
	backward *= factor;

	for (i=1, j=naverage; j > 0; j--, i++) {
		distribution [naverage - i] = backward * j;
		distribution [naverage + i] = forward * j;
	}
	factor = 0.0;

	for (i=0; i <= naverage * 2; i++) {
		factor += distribution [i];
		distribution [i] = factor;
	}

	//---- new plan file ----

	if (output_flag) {
		str_ptr = Get_Control_String (NEW_PLAN_FILE);

		new_plan_file.File_Type ("New Plan File");

		str_ptr = Project_Filename (str_ptr, Extension ());

		Print_Filename (2, new_plan_file.File_Type (), str_ptr);

		new_plan_file.Filename (str_ptr);
		new_plan_file.File_Access (CREATE);

		str_ptr = Get_Control_String (NEW_PLAN_FORMAT);

		if (str_ptr != NULL) {
			new_plan_file.File_Format (str_ptr);
			Print (1, "%s Format = %s", new_plan_file.File_Type (), str_ptr);
		}
		new_plan_file.Node_Based_Flag (type_flag);

		if (!new_plan_file.Open (0)) {
			File_Error ("Creating New Plan File", new_plan_file.Filename ());
		}
	}

	if (link_flag) {

		//---- link summary ----

		str_ptr = Get_Control_String (NEW_LINK_SUMMARY_FILE);

		if (str_ptr != NULL) {
			Print (1);
			summary_file.File_Type ("New Link Summary File");

			summary_file.Create (Project_Filename (str_ptr));
			summary_flag = true;
		}

		//---- check for a link delay file ----

		if (Demand_File_Flag (LINK_DELAY)) {
			delay_flag = true;

			Delay_File *file = (Delay_File *) Demand_Db_File (LINK_DELAY);

			if (file->Dbase_Format () != VERSION3) {
				if (!Network_File_Flag (LANE_CONNECTIVITY)) {
					Warning ("Lane Connectivity is needed for Turn Delay processing");
					Show_Message (1);
				} else {
					turn_flag = true;
				}
			} else {
				Network_File_False (LANE_CONNECTIVITY);
			}
		} else {
			Network_File_False (LANE_CONNECTIVITY);
		}
	}

	//---- time change report ----

	change_flag = Report_Flag (TIME_CHANGE);
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
