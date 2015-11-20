//*********************************************************
//	CheckSurvey.cpp - Check the Household Travel Survey
//*********************************************************

#include "CheckSurvey.hpp"

char * CheckSurvey::HOUSEHOLD_LIST				= "HOUSEHOLD_LIST";
char * CheckSurvey::TIME_OF_DAY_FORMAT			= "TIME_OF_DAY_FORMAT";
char * CheckSurvey::HOUSEHOLD_TYPE_SCRIPT		= "HOUSEHOLD_TYPE_SCRIPT";
char * CheckSurvey::SURVEY_HOUSEHOLD_FILE		= "SURVEY_HOUSEHOLD_FILE";
char * CheckSurvey::SURVEY_HOUSEHOLD_FORMAT		= "SURVEY_HOUSEHOLD_FORMAT";
char * CheckSurvey::SURVEY_HOUSEHOLD_WEIGHTS	= "SURVEY_HOUSEHOLD_WEIGHTS";
char * CheckSurvey::SURVEY_TYPE_SCRIPT			= "SURVEY_TYPE_SCRIPT";
char * CheckSurvey::SURVEY_POPULATION_FILE		= "SURVEY_POPULATION_FILE";
char * CheckSurvey::SURVEY_POPULATION_FORMAT	= "SURVEY_POPULATION_FORMAT";
char * CheckSurvey::SURVEY_ACTIVITY_FILE		= "SURVEY_ACTIVITY_FILE";
char * CheckSurvey::SURVEY_ACTIVITY_FORMAT		= "SURVEY_ACTIVITY_FORMAT";
char * CheckSurvey::NEW_SURVEY_ACTIVITY_FILE	= "NEW_SURVEY_ACTIVITY_FILE";
char * CheckSurvey::NEW_SURVEY_ACTIVITY_FORMAT	= "NEW_SURVEY_ACTIVITY_FORMAT";
char * CheckSurvey::NEW_PROBLEM_FILE			= "NEW_PROBLEM_FILE";
char * CheckSurvey::NEW_PROBLEM_FORMAT			= "NEW_PROBLEM_FORMAT";
char * CheckSurvey::PURPOSE_ANCHOR_ORDER		= "PURPOSE_ANCHOR_ORDER";
char * CheckSurvey::AVERAGE_TRAVEL_SPEED		= "AVERAGE_TRAVEL_SPEED";
char * CheckSurvey::ADDITIONAL_TRAVEL_TIME		= "ADDITIONAL_TRAVEL_TIME";
char * CheckSurvey::MAXIMUM_WALK_TIME			= "MAXIMUM_WALK_TIME";
char * CheckSurvey::MAXIMUM_TRIP_TIME			= "MAXIMUM_TRIP_TIME";
char * CheckSurvey::RANDOM_NUMBER_SEED			= "RANDOM_NUMBER_SEED";

//---------------------------------------------------------
//	CheckSurvey constructor
//---------------------------------------------------------

CheckSurvey::CheckSurvey (void) : Demand_Service (), Problem_Service ()
{
	Program ("CheckSurvey");
	Version ("4.0.4");
	Title ("Activity Generator");

	Demand_File required_demand [] = {
		NEW_ACTIVITY, END_DEMAND
	};

	char *keys [] = {
		HOUSEHOLD_LIST,
		TIME_OF_DAY_FORMAT,
		HOUSEHOLD_TYPE_SCRIPT,
		SURVEY_HOUSEHOLD_FILE,
		SURVEY_HOUSEHOLD_FORMAT,
		SURVEY_HOUSEHOLD_WEIGHTS,
		SURVEY_TYPE_SCRIPT,
		SURVEY_POPULATION_FILE,
		SURVEY_POPULATION_FORMAT,
		SURVEY_ACTIVITY_FILE,
		SURVEY_ACTIVITY_FORMAT,
		NEW_SURVEY_ACTIVITY_FILE,
		NEW_SURVEY_ACTIVITY_FORMAT,
		NEW_PROBLEM_FILE,
		NEW_PROBLEM_FORMAT,
		PURPOSE_ANCHOR_ORDER,
		AVERAGE_TRAVEL_SPEED,
		ADDITIONAL_TRAVEL_TIME,
		MAXIMUM_WALK_TIME,
		MAXIMUM_TRIP_TIME,
		RANDOM_NUMBER_SEED,
		NULL
	};

	char *reports [] = {
		"SURVEY_TYPE_SCRIPT",
		"SURVEY_TYPE_STACK",
		"SURVEY_TYPE_SUMMARY",
		"TRIP_LENGTH_SUMMARY",
		"TOUR_LENGTH_SUMMARY",
		"TRIP_PURPOSE_SUMMARY",
		"TOUR_PURPOSE_SUMMARY",
		"MODE_LENGTH_SUMMARY",
		"MODE_PURPOSE_SUMMARY",
		NULL
	};

	Required_Demand_Files (required_demand);

	Key_List (keys);
	Report_List (reports);

	for (int i=1; i < MAX_MODE; i++) {
		avg_speed [i] = Round (10.0);
		add_time [i] = 300;
	}
	max_walk = 2400;
	max_trip = 7200;

	nhhold = nperson = nwork = 0;
	num_act_gen = num_problem = nproblem = 0;
	num_activity = num_tour = num_trip = nhh_list = num_new = 0;
	survey_type_flag = weight_flag = problem_flag = hhlist_flag = false;
	length_flag = tour_flag = trip_purpose_flag = tour_purpose_flag = false;
	mode_flag = mode_purpose_flag = new_act_flag = false;
}

//---------------------------------------------------------
//	CheckSurvey destructor
//---------------------------------------------------------

CheckSurvey::~CheckSurvey (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	CheckSurvey *exe = new CheckSurvey ();

	return (exe->Start_Execution (commands, control));
}

