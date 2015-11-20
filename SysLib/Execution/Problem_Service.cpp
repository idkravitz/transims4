//*********************************************************
//	Problem_Service.cpp - Summarize Problem Messages
//*********************************************************

#include "Problem_Service.hpp"

#include "Utility.hpp"

//---- problem type codes ----

char * Problem_Service::problem_text [] = {
	"Total", "Path Building", "Time Schedule", "Zero Node", "Vehicle Type", 
	"Path Circuity", "Travel Mode", "Vehicle Access", "Walk Distance",
	"Wait Time", "Walk Access", "Path Size", "Park-&-Ride Lot", "Bike Distance",
	"Departure Time", "Arrival Time", "Link Access", "Lane Connectivity",
	"Parking Access", "Lane Merging", "Lane Changing", "Turning Speed",
	"Pocket Merge", "Vehicle Spacing", "Traffic Control", "Access Restriction", 
	"Transit Stop", "Activity Location", "Vehicle Passenger", "Activity Duration", 
	"Kiss-&-Ride Lot", "Vehicle ID", "Data Sort", "Walk Location", "Bike Location",
	"Transit Location", "Person Match", "Transit Capacity", "Transit Dwell", NULL
};

Problem_Service::Problem_Type  Problem_Service::problem_code [] = {
	TOTAL_PROBLEM, PATH_PROBLEM, TIME_PROBLEM, NODE_PROBLEM, TYPE_PROBLEM, 
	DIST_PROBLEM, MODE_PROBLEM, ACCESS_PROBLEM, WALK_PROBLEM, 
	WAIT_PROBLEM, LINK_PROBLEM, SIZE_PROBLEM, PARK_PROBLEM, BIKE_PROBLEM, 
	DEPARTURE_PROBLEM, ARRIVAL_PROBLEM, LINK_ACCESS_PROBLEM, CONNECT_PROBLEM, 
	PARKING_PROBLEM, MERGE_PROBLEM, LANE_PROBLEM, TURN_PROBLEM, 
	POCKET_PROBLEM, SPACING_PROBLEM, CONTROL_PROBLEM, USE_PROBLEM, 
	STOP_PROBLEM, LOCATION_PROBLEM, PASSENGER_PROBLEM, DURATION_PROBLEM,
	KISS_PROBLEM, VEHICLE_PROBLEM, SORT_PROBLEM, WALK_LOC_PROBLEM, BIKE_LOC_PROBLEM,
	TRANSIT_LOC_PROBLEM, MATCH_PROBLEM, BOARDING_PROBLEM, DWELL_PROBLEM
};

//---------------------------------------------------------
//	Problem_Service constructor with parameters
//---------------------------------------------------------

Problem_Service::Problem_Service (int _max_problems) : Static_Service ()
{
	Max_Problems (_max_problems);

	memset (num_problem, '\0', sizeof (num_problem));
}

//---------------------------------------------------------
//	Problem_Service destructor
//---------------------------------------------------------

Problem_Service::~Problem_Service (void)
{
}

//---------------------------------------------------------
//	Set_Problem
//---------------------------------------------------------

int Problem_Service::Set_Problem (Problem_Type problem)
{
	if (problem < MAX_PROBLEM) {
		num_problem [problem]++;
		num_problem [0]++;
	}
	if ((num_problem [0] > max_problems && max_problems > 0) || problem == MAX_PROBLEM) {
		Report_Problems ();
		if (exe != NULL) exe->Error ("Maximum Number of Problems Exceeded");
	}
	return (problem);
}

//---------------------------------------------------------
//	Report_Problems
//---------------------------------------------------------

void Problem_Service::Report_Problems (void)
{
	int i, num;
	double percent;

	if (num_problem [0] == 0 || exe == NULL) return;

	percent = 100.0 / num_problem [0];

	for (i=1, num=2; i < MAX_PROBLEM; i++) {
		if (num_problem [i] > 0) num++;
	}
	exe->Break_Check (num);

	exe->Write (2, "Total Number of Problems = %d", num_problem [0]);

	for (i=1; i < MAX_PROBLEM; i++) {
		num = num_problem [i];

		if (num > 0) {
			exe->Write (1, "Number of %s (#%d) Problems = %d (%.1lf%%)", problem_text [i], i, num, num * percent);
		}
	}
}

//---------------------------------------------------------
//	Problem_Code
//---------------------------------------------------------

Problem_Service::Problem_Type  Problem_Service::Problem_Code (char *text)
{
	int i;
	char string [40];
	
	str_cpy (string, sizeof (string), text);

	for (i=1; i < (int) strlen (string); i++) {
		if (string [i] == '_') {
			string [i] = ' ';
			i++;
		} else if (string [i] >= 'A' || string [i] <= 'Z') {
			string [i] = string [i] - 'A' + 'a';
		}
	}

	for (i=0; problem_text [i] != NULL; i++) {
		if (str_cmp (string, problem_text [i]) == 0) {
			return (problem_code [i]);
		}
	}
	exe->Error ("Converting Problem Type %s", text);

	return (TOTAL_PROBLEM);
}

char * Problem_Service::Problem_Code (Problem_Service::Problem_Type code)
{
	for (int i=0; problem_text [i] != NULL; i++) {
		if (code == problem_code [i]) {
			return (problem_text [i]);
		}
	}
	exe->Error ("Converting Problem Code %d", code);
	return (NULL);
}
