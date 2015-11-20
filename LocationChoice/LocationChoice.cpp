//*********************************************************
//	LocationChoice.cpp - Activity Location Generator
//*********************************************************

#include "LocationChoice.hpp"

char * LocationChoice::HOUSEHOLD_LIST				= "HOUSEHOLD_LIST";
char * LocationChoice::ACTIVITY_PATTERN_FILE		= "ACTIVITY_PATTERN_FILE";
char * LocationChoice::ACTIVITY_PATTERN_FORMAT		= "ACTIVITY_PATTERN_FORMAT";
char * LocationChoice::TIME_OF_DAY_FORMAT			= "TIME_OF_DAY_FORMAT";
char * LocationChoice::NEW_PROBLEM_FILE				= "NEW_PROBLEM_FILE";
char * LocationChoice::NEW_PROBLEM_FORMAT			= "NEW_PROBLEM_FORMAT";
char * LocationChoice::NEW_TRIP_TIME_FILE			= "NEW_TRIP_TIME_FILE";
char * LocationChoice::NEW_TRIP_DISTANCE_FILE		= "NEW_TRIP_DISTANCE_FILE";
char * LocationChoice::DISTANCE_CALCULATION			= "DISTANCE_CALCULATION";
char * LocationChoice::MAXIMUM_WALK_DISTANCE		= "MAXIMUM_WALK_DISTANCE";
char * LocationChoice::AVERAGE_TRAVEL_SPEED			= "AVERAGE_TRAVEL_SPEED";
char * LocationChoice::ADDITIONAL_TRAVEL_TIME		= "ADDITIONAL_TRAVEL_TIME";
char * LocationChoice::RANDOM_NUMBER_SEED			= "RANDOM_NUMBER_SEED";
char * LocationChoice::ACTIVITY_PURPOSE_RANGE_x		= "ACTIVITY_PURPOSE_RANGE_*";
char * LocationChoice::ACTIVITY_ANCHOR_FLAG_x		= "ACTIVITY_ANCHOR_FLAG_*";
char * LocationChoice::SCHEDULE_CONSTRAINT_x		= "SCHEDULE_CONSTRAINT_*";
char * LocationChoice::ZONE_BASED_METHOD_x			= "ZONE_BASED_METHOD_*";
char * LocationChoice::LOCATION_CHOICE_SCRIPT_x		= "LOCATION_CHOICE_SCRIPT_*";
char * LocationChoice::ZONE_WEIGHT_FIELD_x			= "ZONE_WEIGHT_FIELD_*";
char * LocationChoice::ZONE_WEIGHT_FACTOR_x			= "ZONE_WEIGHT_FACTOR_*";
char * LocationChoice::BALANCING_FACTOR_FIELD_x		= "BALANCING_FACTOR_FIELD_*";
char * LocationChoice::LOCATION_WEIGHT_FIELD_x		= "LOCATION_WEIGHT_FIELD_*";
char * LocationChoice::LOCATION_WEIGHT_FACTOR_x		= "LOCATION_WEIGHT_FACTOR_*";
char * LocationChoice::SKIM_TIME_FIELD_x			= "SKIM_TIME_FIELD_*";
char * LocationChoice::MODE_DISTANCE_FACTORS_x		= "MODE_DISTANCE_FACTORS_*";
char * LocationChoice::MODE_TIME_FACTORS_x			= "MODE_TIME_FACTORS_*";
char * LocationChoice::ZONE_SKIM_FILE_x				= "ZONE_SKIM_FILE_*";
char * LocationChoice::ZONE_SKIM_FORMAT_x			= "ZONE_SKIM_FORMAT_*";
char * LocationChoice::TIME_PERIOD_EQUIVALENCE_x	= "TIME_PERIOD_EQUIVALENCE_*";
char * LocationChoice::SKIM_MEMORY_FACTOR_x	        = "SKIM_MEMORY_FACTOR_*";
char * LocationChoice::TRAVEL_TIME_FIELD_MODE_x		= "TRAVEL_TIME_FIELD_MODE_*";
char * LocationChoice::BALANCING_FACTOR_FILE		= "BALANCING_FACTOR_FILE";
char * LocationChoice::BALANCING_FACTOR_FORMAT		= "BALANCING_FACTOR_FORMAT";

//---------------------------------------------------------
//	LocationChoice constructor
//---------------------------------------------------------

LocationChoice::LocationChoice (void) : Demand_Service (), Problem_Service ()
{
	Program ("LocationChoice");
	Version ("4.0.6");
	Title ("Location Choice Model");

	Network_File required_network [] = {
		NODE, LINK, ACTIVITY_LOCATION, END_NETWORK
	};
	Network_File optional_network [] = {
		DIRECTORY, ZONE, END_NETWORK
	};
	Demand_File required_demand [] = {
		NEW_ACTIVITY, END_DEMAND
	};
	Demand_File optional_demand [] = {
		HOUSEHOLD, ACTIVITY, END_DEMAND
	};

	char *keys [] = {
		HOUSEHOLD_LIST,
		TIME_OF_DAY_FORMAT,
		ACTIVITY_PATTERN_FILE,
		ACTIVITY_PATTERN_FORMAT,
		NEW_PROBLEM_FILE,
		NEW_PROBLEM_FORMAT,
		NEW_TRIP_TIME_FILE,
		NEW_TRIP_DISTANCE_FILE,
		DISTANCE_CALCULATION,
		MAXIMUM_WALK_DISTANCE,
		AVERAGE_TRAVEL_SPEED,
		ADDITIONAL_TRAVEL_TIME,
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
		"LOCATION_CHOICE_SCRIPT",
		"LOCATION_CHOICE_STACK",
		"LOCATION_CHOICE_DETAILS_*",
		"TIME_PERIOD_EQUIVALENCE",
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
	budget1_field = budget2_field = 0;
	min_fac1_field = max_fac1_field = min_fac2_field = max_fac2_field = 0;
	zone1_field = zone2_field = loc1_field = loc2_field = purp1_field = purp2_field = 0;

	max_walk = 2000;
	num_act_gen = num_problem = zero_zone = warning_count = 0;
	num_activity = num_tour = num_trip = purpose_number = max_purpose = nhh_list = 0;

	choice_flag = problem_flag = hhold_flag = update_flag = balance_flag = false;
	hhlist_flag = zone_flag = skim_flag = length_flag = tour_flag = warning_flag = false;
	dump_flag = dump_list_flag = trip_purpose_flag = tour_purpose_flag = false;
	mode_flag = mode_purpose_flag = time_length_flag = distance_length_flag = false;
	act_problem_flag = false;

	location_array = (Location_Array *) new Choice_Array ();
	zone_array = (Zone_Array *) new Zone_List_Array ();
}

//---------------------------------------------------------
//	LocationChoice destructor
//---------------------------------------------------------

LocationChoice::~LocationChoice (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	LocationChoice *exe = new LocationChoice ();

	return (exe->Start_Execution (commands, control));
}

