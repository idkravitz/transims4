//*********************************************************
//	Activity_Problem.cpp - write a activity problem
//*********************************************************

#include "CheckSurvey.hpp"

//---------------------------------------------------------
//	Activity_Problem
//---------------------------------------------------------

void CheckSurvey::Activity_Problem (Problem_Type problem, Activity_Data *act_ptr, int weight)
{
	if (act_ptr->Problem () > 0) return;
				
	Set_Problem (problem);
	act_ptr->Problem (problem);
	num_problem++;

	if (problem_flag) {
		problem_file.Household (act_ptr->Household ());
		problem_file.Person (act_ptr->Person ());

		problem_file.Trip (act_ptr->Activity ());
		problem_file.Mode (act_ptr->Mode ());
		problem_file.Problem (problem);

		problem_file.Start (activity_time.Format_Step (act_ptr->Start_Time ()));
		problem_file.End (activity_time.Format_Step (act_ptr->End_Time ()));

		problem_file.Location (act_ptr->Location ());
		problem_file.Vehicle (act_ptr->Vehicle ());

		problem_file.Survey (weight);

		problem_file.Write ();
	}
}
