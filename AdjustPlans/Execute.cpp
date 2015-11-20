//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "AdjustPlans.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void AdjustPlans::Execute (void)
{
	int i, nfile, error, count;
	double percent;

	Count_Data *count_ptr;

	//---- read vehicle file later ----

	Demand_File_False (VEHICLE);

	//---- read the network ----

	Demand_Service::Execute ();

	//---- read the count file ----

	Read_Counts ();

	//---- read the plan file ----

	for (nfile=0; ; nfile++) {
		if (!plan_file.Open (nfile)) break;

		//---- process the plan files ----

		Read_Plans ();
	}
	if (nfile == 0) {
		File_Error ("No Plan Files were Found", plan_file.Filename ());
	}

	//---- write summary statistics ----

	Break_Check (8);

	Write (2, "Number of Plan Files = %d", nfile);
	Write (1, "Number of Input Plans = %d", plan_file.Num_Plans ());
	Write (1, "Number of Input Records = %d", plan_file.Num_Records ());
	Write (1, "Number of Input Travelers = %d", plan_file.Num_Travelers ());
	Write (1, "Number of Input Trips = %d", plan_file.Num_Trips ());

	count = travel_data.Num_Records ();

	if (count == 0) {
		Error ("No Plans are using the Count Links");
	} else {
		Print (2, "Number of Plans using Count Links = %d", count);
	}

	//---- iterate to minimize error ----

	for (i=1; i <= max_iter; i++) {

		//---- calculate the error ---

		error = count = 0;

		for (count_ptr = count_data.First (); count_ptr; count_ptr = count_data.Next ()) {
			error += abs (count_ptr->Difference ()); 
			count += count_ptr->Count ();
		}
		if (count == 0) break;

		percent = 100.0 * error / count;

		Write (2, "Iteration %d: Percent Error = %.2lf", i, percent);

		if (percent <= max_error) break;

		//---- make adjustments ----

		if (!Adjust_Weights ()) break;
	}

	//---- convergence file ----

	if (converge_flag) {
		Write_Results ();
	}

	//---- output the results ----

	if (output_flag) {
		max_household = max_traveler / Traveler_Scale ();

		//---- copy plan records ----

		Show_Message (1);

		for (nfile=0; ; nfile++) {
			if (!plan_file.Open (nfile)) break;

			if (!new_plan_file.Open (nfile)) {
				Error ("Creating %s", new_plan_file.File_Type ());
			}
			if (hhold_flag) {
				if (!hhold_file.Open (nfile)) {
					Error ("Creating New Household List");
				}
			}
			Copy_Plans ();
		}

		//---- write summary statistics ----

		Break_Check (8);

		Write (2, "Number of New Plan Files = %d", nfile);
		Write (1, "Number of Output Plans = %d", new_plan_file.Num_Plans ());
		Write (1, "Number of Output Records = %d", new_plan_file.Num_Records ());
		Write (1, "Number of Output Travelers = %d", new_plan_file.Num_Travelers ());
		Write (1, "Number of Output Trips = %d", new_plan_file.Num_Trips ());

		Break_Check (4);

		Write (2, "Number of Plans Deleted = %d", num_deleted);
		Write (1, "Number of Plans Replicated = %d", num_replicated);
		Write (1, "Number of Replicated Plans = %d", num_added);

		//---- update the vehicle file ----

		Copy_Vehicles ();
	}
	Exit_Stat (DONE);
}
