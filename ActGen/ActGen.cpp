//*********************************************************
//	ActGen.cpp - Activity Generator
//*********************************************************

#include "ActGen.hpp"

char * ActGen::HOUSEHOLD_LIST				= "HOUSEHOLD_LIST";
char * ActGen::TIME_OF_DAY_FORMAT			= "TIME_OF_DAY_FORMAT";
char * ActGen::HOUSEHOLD_TYPE_SCRIPT		= "HOUSEHOLD_TYPE_SCRIPT";
char * ActGen::SURVEY_HOUSEHOLD_FILE		= "SURVEY_HOUSEHOLD_FILE";
char * ActGen::SURVEY_HOUSEHOLD_FORMAT		= "SURVEY_HOUSEHOLD_FORMAT";
char * ActGen::SURVEY_HOUSEHOLD_WEIGHTS		= "SURVEY_HOUSEHOLD_WEIGHTS";
char * ActGen::SURVEY_TYPE_SCRIPT			= "SURVEY_TYPE_SCRIPT";
char * ActGen::SURVEY_POPULATION_FILE		= "SURVEY_POPULATION_FILE";
char * ActGen::SURVEY_POPULATION_FORMAT		= "SURVEY_POPULATION_FORMAT";
char * ActGen::SURVEY_ACTIVITY_FILE			= "SURVEY_ACTIVITY_FILE";
char * ActGen::SURVEY_ACTIVITY_FORMAT		= "SURVEY_ACTIVITY_FORMAT";
char * ActGen::NEW_PROBLEM_FILE				= "NEW_PROBLEM_FILE";
char * ActGen::NEW_PROBLEM_FORMAT			= "NEW_PROBLEM_FORMAT";
char * ActGen::NEW_HOUSEHOLD_MATCH_FILE		= "NEW_HOUSEHOLD_MATCH_FILE";
char * ActGen::NEW_HOUSEHOLD_PERSON_COUNT	= "NEW_HOUSEHOLD_PERSON_COUNT";
char * ActGen::NEW_ACTIVITY_PATTERN_FILE	= "NEW_ACTIVITY_PATTERN_FILE";
char * ActGen::NEW_ACTIVITY_PATTERN_FORMAT	= "NEW_ACTIVITY_PATTERN_FORMAT";
char * ActGen::NEW_TRIP_TIME_FILE			= "NEW_TRIP_TIME_FILE";
char * ActGen::NEW_TRIP_DISTANCE_FILE		= "NEW_TRIP_DISTANCE_FILE";
char * ActGen::DISTANCE_CALCULATION			= "DISTANCE_CALCULATION";
char * ActGen::MAXIMUM_WALK_DISTANCE		= "MAXIMUM_WALK_DISTANCE";
char * ActGen::AVERAGE_TRAVEL_SPEED			= "AVERAGE_TRAVEL_SPEED";
char * ActGen::ADDITIONAL_TRAVEL_TIME		= "ADDITIONAL_TRAVEL_TIME";
char * ActGen::RANDOM_NUMBER_SEED			= "RANDOM_NUMBER_SEED";
char * ActGen::TIME_SCHEDULE_VARIANCE		= "TIME_SCHEDULE_VARIANCE";
char * ActGen::ACTIVITY_PURPOSE_RANGE_x		= "ACTIVITY_PURPOSE_RANGE_*";
char * ActGen::ACTIVITY_ANCHOR_FLAG_x		= "ACTIVITY_ANCHOR_FLAG_*";
char * ActGen::SCHEDULE_CONSTRAINT_x		= "SCHEDULE_CONSTRAINT_*";
char * ActGen::ZONE_BASED_METHOD_x			= "ZONE_BASED_METHOD_*";
char * ActGen::LOCATION_CHOICE_SCRIPT_x		= "LOCATION_CHOICE_SCRIPT_*";
char * ActGen::ZONE_WEIGHT_FIELD_x			= "ZONE_WEIGHT_FIELD_*";
char * ActGen::ZONE_WEIGHT_FACTOR_x			= "ZONE_WEIGHT_FACTOR_*";
char * ActGen::BALANCING_FACTOR_FIELD_x		= "BALANCING_FACTOR_FIELD_*";
char * ActGen::LOCATION_WEIGHT_FIELD_x		= "LOCATION_WEIGHT_FIELD_*";
char * ActGen::LOCATION_WEIGHT_FACTOR_x		= "LOCATION_WEIGHT_FACTOR_*";
char * ActGen::SKIM_TIME_FIELD_x			= "SKIM_TIME_FIELD_*";
char * ActGen::MODE_DISTANCE_FACTORS_x		= "MODE_DISTANCE_FACTORS_*";
char * ActGen::MODE_TIME_FACTORS_x			= "MODE_TIME_FACTORS_*";
char * ActGen::ZONE_SKIM_FILE_x				= "ZONE_SKIM_FILE_*";
char * ActGen::ZONE_SKIM_FORMAT_x			= "ZONE_SKIM_FORMAT_*";
char * ActGen::TIME_PERIOD_EQUIVALENCE_x	= "TIME_PERIOD_EQUIVALENCE_*";
char * ActGen::SKIM_MEMORY_FACTOR_x			= "SKIM_MEMORY_FACTOR_*";
char * ActGen::TRAVEL_TIME_FIELD_MODE_x		= "TRAVEL_TIME_FIELD_MODE_*";
char * ActGen::BALANCING_FACTOR_FILE		= "BALANCING_FACTOR_FILE";
char * ActGen::BALANCING_FACTOR_FORMAT		= "BALANCING_FACTOR_FORMAT";

//---------------------------------------------------------
//	ActGen constructor
//---------------------------------------------------------

ActGen::ActGen (void) : Demand_Service (), Problem_Service ()
{
	Program ("ActGen");
	Version ("4.0.36");
	Title ("Activity Generator");

	Network_File required_network [] = {
		NODE, LINK, PARKING, ACTIVITY_LOCATION, PROCESS_LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, ZONE, TRANSIT_STOP, 
		END_NETWORK
	};

	Demand_File required_demand [] = {
		HOUSEHOLD, POPULATION, VEHICLE_TYPE, VEHICLE, NEW_ACTIVITY, END_DEMAND
	};

	Demand_File optional_demand [] = {
		ACTIVITY, END_DEMAND
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
		NEW_PROBLEM_FILE,
		NEW_PROBLEM_FORMAT,
		NEW_HOUSEHOLD_MATCH_FILE,
		NEW_HOUSEHOLD_PERSON_COUNT,
		NEW_ACTIVITY_PATTERN_FILE,
		NEW_ACTIVITY_PATTERN_FORMAT,
		NEW_TRIP_TIME_FILE,
		NEW_TRIP_DISTANCE_FILE,
		DISTANCE_CALCULATION,
		MAXIMUM_WALK_DISTANCE,
		AVERAGE_TRAVEL_SPEED,
		ADDITIONAL_TRAVEL_TIME,
		TIME_SCHEDULE_VARIANCE,
		RANDOM_NUMBER_SEED,
		ACTIVITY_PURPOSE_RANGE_x,
		ACTIVITY_ANCHOR_FLAG_x,
		SCHEDULE_CONSTRAINT_x,
		ZONE_BASED_METHOD_x,
		LOCATION_CHOICE_SCRIPT_x,
		ZONE_WEIGHT_FIELD_x,
		ZONE_WEIGHT_FACTOR_x,
		BALANCING_FACTOR_FIELD_x,
		LOCATION_WEIGHT_FIELD_x,
		LOCATION_WEIGHT_FACTOR_x,
		SKIM_TIME_FIELD_x,
		MODE_DISTANCE_FACTORS_x,
		MODE_TIME_FACTORS_x,
		ZONE_SKIM_FILE_x,
		ZONE_SKIM_FORMAT_x,
		TIME_PERIOD_EQUIVALENCE_x,
		SKIM_MEMORY_FACTOR_x,
		TRAVEL_TIME_FIELD_MODE_x,
		BALANCING_FACTOR_FILE,
		BALANCING_FACTOR_FORMAT,
		NULL
	};

	char *reports [] = {
		"HOUSEHOLD_TYPE_SCRIPT",
		"HOUSEHOLD_TYPE_STACK",
		"HOUSEHOLD_TYPE_SUMMARY",
		"SURVEY_TYPE_SCRIPT",
		"SURVEY_TYPE_STACK",
		"SURVEY_TYPE_SUMMARY",
		"LOCATION_CHOICE_SCRIPT",
		"LOCATION_CHOICE_STACK",
		"LOCATION_CHOICE_DETAILS_*",
		"TIME_PERIOD_EQUIVALENCE",
		"HOUSEHOLD_MATCH_REPORT",
		"TRIP_LENGTH_SUMMARY",
		"TOUR_LENGTH_SUMMARY",
		"TRIP_PURPOSE_SUMMARY",
		"TOUR_PURPOSE_SUMMARY",
		"MODE_LENGTH_SUMMARY",
		"MODE_PURPOSE_SUMMARY",
		"ACTIVITY_PROBLEM_REPORT",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	Check_Data (true);
	Renumber (true);

	db_files = NULL;
	num_db_files = 0;

	memset (purpose_map, '\0', sizeof (purpose_map));

	for (int i=1; i < MAX_MODE; i++) {
		avg_speed [i] = Round (10.0);
		add_time [i] = 600;
		time_field [i] = 0;
	}
	
	distance_method = STRAIGHT;

	start_field = end_field = duration_field = purpose_field = mode_field = utility_field = 0;
	stops1_field = stops2_field = subtours_field = distance1_field = distance2_field = 0;
	budget1_field = budget2_field = max_variance = time_variance = 0;
	min_fac1_field = max_fac1_field = min_fac2_field = max_fac2_field = 0;
	zone1_field = zone2_field = loc1_field = loc2_field = purp1_field = purp2_field = 0;

	hh_match = per_match = age_match = work_match = gender_match = nhhold = nperson = nwork = 0;
	num_act_gen = num_problem = survey_hhold = zero_zone = warning_count = num_pattern = 0;
	num_activity = num_tour = num_trip = purpose_number = max_purpose = nhh_list = 0;
	max_walk = 2000;

	hhold_type_flag = survey_type_flag = choice_flag = weight_flag = problem_flag = false;
	hhlist_flag = zone_flag = skim_flag = length_flag = tour_flag = warning_flag = false;
	dump_flag = dump_list_flag = trip_purpose_flag = tour_purpose_flag = update_flag = false;
	match_flag = mode_flag = mode_purpose_flag = time_length_flag = distance_length_flag = false;
	new_pattern_flag = balance_flag = count_flag = false;
	act_problem_flag = false;

	household_array = (Household_Array *) new HHold_Array ();
	location_array = (Location_Array *) new Choice_Array ();
	zone_array = (Zone_Array *) new Zone_List_Array ();
}

//---------------------------------------------------------
//	ActGen destructor
//---------------------------------------------------------

ActGen::~ActGen (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ActGen *exe = new ActGen ();

	return (exe->Start_Execution (commands, control));
}

