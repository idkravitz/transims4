//*********************************************************
//	Activity_Problem.cpp - write a activity problem
//*********************************************************

#include "ActGen.hpp"

//---------------------------------------------------------
//	Activity_Problem
//---------------------------------------------------------

void ActGen::Activity_Problem (Problem_Type problem, Activity_Data *act_ptr)
{
	Location_Data *location_ptr;
	Vehicle_Data *vehicle_ptr;
	HHold_Data *household_ptr;

	if (act_ptr->Problem () > 0) return;

	if (problem == LOCATION_PROBLEM) {
		if (act_ptr->Mode () == ALL_WALK) {
			problem = WALK_LOC_PROBLEM;
		} else if (act_ptr->Mode () == BICYCLE) {
			problem = BIKE_LOC_PROBLEM;
		} else if (act_ptr->Mode () == TRANSIT || act_ptr->Mode () == RAIL_TRANSIT ||
			act_ptr->Mode () == PNR_OUT || act_ptr->Mode () == PNR_IN ||
			act_ptr->Mode () == KNR_OUT || act_ptr->Mode () == KNR_IN) {
			problem = TRANSIT_LOC_PROBLEM;
		}
	}
	Set_Problem (problem);
	act_ptr->Problem (problem);
	num_problem++;

	household_ptr = household_data.Get (act_ptr->Household ());

	if (household_ptr != NULL) {
		household_ptr->Problem_Flag (true);
	}
	if (problem_flag) {

		problem_file.Household (act_ptr->Household ());
		problem_file.Person (act_ptr->Person ());

		problem_file.Trip (act_ptr->Activity ());
		problem_file.Mode (act_ptr->Mode ());
		problem_file.Problem (problem);

		problem_file.Start (activity_time.Format_Step (act_ptr->Start_Time ()));
		problem_file.End (activity_time.Format_Step (act_ptr->End_Time ()));

		problem_file.Location (act_ptr->Location ());

		if (act_ptr->Location () > 0) {
			location_ptr = location_data [act_ptr->Location ()];
			if (location_ptr != NULL) {
				problem_file.Location (location_ptr->ID ());
			}
		}

		problem_file.Vehicle (act_ptr->Vehicle ());

		if (act_ptr->Vehicle () > 0) {
			vehicle_ptr = vehicle_data [act_ptr->Vehicle ()];
			if (vehicle_ptr != NULL) {
				problem_file.Vehicle (vehicle_ptr->ID ());
			}
		}
		problem_file.Survey (survey_hhold);

		problem_file.Write ();
	}
}
