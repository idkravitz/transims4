//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "AdjustPlans.hpp"

#include "Delay_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void AdjustPlans::Program_Control (void)
{
	char *str_ptr, *format_ptr;
	
	str_ptr = Get_Control_String (NEW_PLAN_FILE);

	if (str_ptr == NULL) {
		output_flag = false;
		Demand_Option_False (VEHICLE);
		Demand_Option_False (NEW_VEHICLE);
	} else {
		output_flag = true;
		Demand_File_True (VEHICLE);
		Demand_File_True (NEW_VEHICLE);
	}

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

	//---- traffic count file ----

	str_ptr = Get_Control_String (TRAFFIC_COUNT_FILE);

	if (str_ptr == NULL) goto control_error;
	Print (1);

	format_ptr = Get_Control_String (TRAFFIC_COUNT_FORMAT);

	if (format_ptr != NULL) {
		count_file.Dbase_Format (format_ptr);
	}
	count_file.Open (Project_Filename (str_ptr));

	//---- open the link equivalence ----

	str_ptr = Get_Control_String (LINK_EQUIVALENCE_FILE);

	if (str_ptr != NULL) {
		Print (1);
		link_equiv.Open (Project_Filename (str_ptr));
		equiv_flag = true;
	}

	//---- count scaling factor ----

	str_ptr = Get_Control_String (COUNT_SCALING_FACTOR);

	if (str_ptr != NULL) {
		factor = atof (str_ptr);

		if (factor < 0.1 || factor > 2.0) {
			Error ("Count Scaling Factor %.2lf is Out of Range (0.1..2.0)", factor);
		}
		Print (2, "Count Scaling Factor = %.2lf", factor);
	}

	//---- random number seed ----

	str_ptr = Get_Control_String (RANDOM_NUMBER_SEED);

	if (str_ptr != NULL) {
		random.Seed (atoi (str_ptr));
	}
	Print (2, "Random Number Seed = %d", random.Seed ());

	//---- maximum percent error ----

	str_ptr = Get_Control_String (MAXIMUM_PERCENT_ERROR);

	if (str_ptr != NULL) {
		max_error = atof (str_ptr);

		if (max_error < 0.01 || max_error > 100.0) {
			Error ("Maximum Percent Error %.2lf is Out of Range (0.01..100.0)", max_error);
		}
	}
	Print (2, "Maximum Percent Error = %.2lf%%", max_error);

	//---- maximum iterations ----

	str_ptr = Get_Control_String (MAXIMUM_ITERATIONS);

	if (str_ptr != NULL) {
		max_iter = atoi (str_ptr);

		if (max_iter < 1 || max_iter > 1000) {
			Error ("Maximum Iterations %d is Out of Range (1..1000)", max_iter);
		}
	}
	Print (1, "Maximum Iterations = %d", max_iter);

	//---- maximum replication ----

	str_ptr = Get_Control_String (MAXIMUM_REPLICATION);

	if (str_ptr != NULL) {
		max_copy = atoi (str_ptr);

		if (max_copy < 1 || max_copy > 100) {
			Error ("Maximum Replication %d is Out of Range (1..100)", max_copy);
		}
	}
	Print (1, "Maximum Replication = %d", max_copy);

	//---- deletion flag ----

	str_ptr = Get_Control_String (PROHIBIT_DELETIONS);

	if (str_ptr != NULL) {
		delete_flag = Get_Control_Flag (PROHIBIT_DELETIONS);

		if (delete_flag) {
			Print (1, "Deletions are Prohibited");
		} else {
			Print (1, "Deletions are Permitted");
		}
	}

	//---- new plan file ----

	if (output_flag) {
		str_ptr = Get_Control_String (NEW_PLAN_FILE);

		new_plan_file.File_Type ("New Plan File");

		str_ptr = Project_Filename (str_ptr, Extension ());

		Print_Filename (2, new_plan_file.File_Type (), str_ptr);

		new_plan_file.Filename (str_ptr);
		new_plan_file.File_Access (CREATE);

		if (plan_file.Extend () > 0 && new_plan_file.Extend () == 0) {
			new_plan_file.Extend (plan_file.Extend ());
		}
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

	//---- get the household list ----

	str_ptr = Get_Control_String (NEW_HOUSEHOLD_LIST);

	if (str_ptr != NULL) {
		str_ptr = Project_Filename (str_ptr, Extension ());

		hhold_file.File_Type ("New Household List");
		hhold_file.File_Access (Db_Code::CREATE);
		hhold_file.Filename (str_ptr);

		if (plan_file.Extend () > 0 && hhold_file.Extend () == 0) {
			hhold_file.Extend (plan_file.Extend ());
		}
		Print_Filename (2, hhold_file.File_Type (), str_ptr);

		if (!hhold_file.Open (0)) {
			File_Error ("Opening New Household List", hhold_file.Filename ());
		}
		hhold_flag = true;
	}

	//---- convergence file ----

	str_ptr = Get_Control_String (CONVERGENCE_FILE);

	if (str_ptr != NULL) {
		converge_file.File_Type ("Convergence File");
		Print (1);

		format_ptr = Get_Control_String (CONVERGENCE_FORMAT);

		if (format_ptr != NULL) {
			converge_file.Dbase_Format (format_ptr);
		}
		converge_file.Create (Project_Filename (str_ptr));
		converge_flag = true;

		if (equiv_flag) {
			converge_file.Add_Field ("GROUP", INTEGER, 4);
		} else {
			converge_file.Add_Field ("LINK", INTEGER, 10);
			converge_file.Add_Field ("DIR", INTEGER, 2);
			converge_file.Add_Field ("ANODE", INTEGER, 10);
			converge_file.Add_Field ("BNODE", INTEGER, 10);
		}
		converge_file.Add_Field ("START", STRING, 20);
		converge_file.Add_Field ("END", STRING, 20);
		converge_file.Add_Field ("COUNT", INTEGER, 10);
		converge_file.Add_Field ("VOLUME", INTEGER, 10);
		converge_file.Add_Field ("DIFFERENCE", INTEGER, 10);

		converge_file.Write_Header ();
	}

	//---- write the report names ----

	List_Reports ();

	//---- process support data ----

	if (equiv_flag) {
		link_equiv.Read (Report_Flag (LINK_EQUIV));
	}

	//---- check for a link delay file ----

	if (Demand_File_Flag (LINK_DELAY)) {
		delay_flag = true;

		Delay_File *file = (Delay_File *) Demand_Db_File (LINK_DELAY);

		if (file->Dbase_Format () != VERSION3) {
			if (!Network_File_Flag (LANE_CONNECTIVITY)) {
				Write (1, "Warning: Lane Connectivity is needed for Turn Delay processing");
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
	return;

	//---- error message ----

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
} 
