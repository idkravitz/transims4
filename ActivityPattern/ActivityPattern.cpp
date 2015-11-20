//*********************************************************
//	ActivityPattern.cpp - Activity Generator
//*********************************************************

#include "ActivityPattern.hpp"

char * ActivityPattern::HOUSEHOLD_LIST				= "HOUSEHOLD_LIST";
char * ActivityPattern::TIME_OF_DAY_FORMAT			= "TIME_OF_DAY_FORMAT";
char * ActivityPattern::HOUSEHOLD_TYPE_SCRIPT		= "HOUSEHOLD_TYPE_SCRIPT";
char * ActivityPattern::SURVEY_HOUSEHOLD_FILE		= "SURVEY_HOUSEHOLD_FILE";
char * ActivityPattern::SURVEY_HOUSEHOLD_FORMAT		= "SURVEY_HOUSEHOLD_FORMAT";
char * ActivityPattern::SURVEY_HOUSEHOLD_WEIGHTS	= "SURVEY_HOUSEHOLD_WEIGHTS";
char * ActivityPattern::SURVEY_TYPE_SCRIPT			= "SURVEY_TYPE_SCRIPT";
char * ActivityPattern::SURVEY_POPULATION_FILE		= "SURVEY_POPULATION_FILE";
char * ActivityPattern::SURVEY_POPULATION_FORMAT	= "SURVEY_POPULATION_FORMAT";
char * ActivityPattern::SURVEY_ACTIVITY_FILE		= "SURVEY_ACTIVITY_FILE";
char * ActivityPattern::SURVEY_ACTIVITY_FORMAT		= "SURVEY_ACTIVITY_FORMAT";
char * ActivityPattern::ACTIVITY_PATTERN_FILE		= "ACTIVITY_PATTERN_FILE";
char * ActivityPattern::ACTIVITY_PATTERN_FORMAT		= "ACTIVITY_PATTERN_FORMAT";
char * ActivityPattern::NEW_PROBLEM_FILE			= "NEW_PROBLEM_FILE";
char * ActivityPattern::NEW_PROBLEM_FORMAT			= "NEW_PROBLEM_FORMAT";
char * ActivityPattern::NEW_HOUSEHOLD_MATCH_FILE	= "NEW_HOUSEHOLD_MATCH_FILE";
char * ActivityPattern::NEW_HOUSEHOLD_PERSON_COUNT	= "NEW_HOUSEHOLD_PERSON_COUNT";
char * ActivityPattern::NEW_ACTIVITY_PATTERN_FILE   = "NEW_ACTIVITY_PATTERN_FILE";
char * ActivityPattern::NEW_ACTIVITY_PATTERN_FORMAT	= "NEW_ACTIVITY_PATTERN_FORMAT";
char * ActivityPattern::TIME_SCHEDULE_VARIANCE		= "TIME_SCHEDULE_VARIANCE";
char * ActivityPattern::RANDOM_NUMBER_SEED			= "RANDOM_NUMBER_SEED";

//---------------------------------------------------------
//	ActivityPattern constructor
//---------------------------------------------------------

ActivityPattern::ActivityPattern (void) : Demand_Service (), Problem_Service ()
{
	Program ("ActivityPattern");
	Version ("4.0.2");
	Title ("Activity Generator");

	Network_File required_network [] = {
		NODE, LINK, PARKING, ACTIVITY_LOCATION, END_NETWORK
	};
	Network_File optional_network [] = {
		DIRECTORY, END_NETWORK
	};
	Demand_File required_demand [] = {
		HOUSEHOLD, POPULATION, VEHICLE_TYPE, VEHICLE, END_DEMAND
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
		ACTIVITY_PATTERN_FILE,
		ACTIVITY_PATTERN_FORMAT,
		NEW_PROBLEM_FILE,
		NEW_PROBLEM_FORMAT,
		NEW_HOUSEHOLD_MATCH_FILE,
		NEW_HOUSEHOLD_PERSON_COUNT,
		NEW_ACTIVITY_PATTERN_FILE,
		NEW_ACTIVITY_PATTERN_FORMAT,
		TIME_SCHEDULE_VARIANCE,
		RANDOM_NUMBER_SEED,
		NULL
	};

	char *reports [] = {
		"HOUSEHOLD_TYPE_SCRIPT",
		"HOUSEHOLD_TYPE_STACK",
		"HOUSEHOLD_TYPE_SUMMARY",
		"SURVEY_TYPE_SCRIPT",
		"SURVEY_TYPE_STACK",
		"SURVEY_TYPE_SUMMARY",
		"HOUSEHOLD_MATCH_REPORT",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	Check_Data (true);
	Renumber (true);

	hh_match = per_match = age_match = work_match = gender_match = nhhold = nperson = nwork = 0;
	num_act_gen = num_problem = survey_hhold = warning_count = max_variance = time_variance = 0;
	num_activity = num_tour = num_trip = nhh_list = 0;

	hhold_type_flag = survey_type_flag = weight_flag = problem_flag = false;
	hhlist_flag = false;
	trip_purpose_flag = tour_purpose_flag = update_flag = false;
	match_flag = count_flag = false;

	household_array = (Household_Array *) new HHold_Array ();
}

//---------------------------------------------------------
//	ActivityPattern destructor
//---------------------------------------------------------

ActivityPattern::~ActivityPattern (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ActivityPattern *exe = new ActivityPattern ();

	return (exe->Start_Execution (commands, control));
}

