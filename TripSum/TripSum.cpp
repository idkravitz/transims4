//*********************************************************
//	TripSum.cpp - Summarize Trip File Demand
//*********************************************************

#include "TripSum.hpp"

char * TripSum::HOUSEHOLD_LIST				= "HOUSEHOLD_LIST";
char * TripSum::HOUSEHOLD_PERSON_COUNT		= "HOUSEHOLD_PERSON_COUNT";
char * TripSum::TIME_OF_DAY_FORMAT			= "TIME_OF_DAY_FORMAT";
char * TripSum::SUMMARY_TIME_PERIODS		= "SUMMARY_TIME_PERIODS";
char * TripSum::SUMMARY_TIME_INCREMENT		= "SUMMARY_TIME_INCREMENT";
char * TripSum::SUMMARY_LENGTH_INCREMENT	= "SUMMARY_LENGTH_INCREMENT";
char * TripSum::SELECT_TRIP_MODES			= "SELECT_TRIP_MODES";
char * TripSum::SELECT_TRIP_PURPOSES		= "SELECT_TRIP_PURPOSES";
char * TripSum::SELECT_ORIGIN_ZONES			= "SELECT_ORIGIN_ZONES";
char * TripSum::SELECT_DESTINATION_ZONES	= "SELECT_DESTINATION_ZONES";
char * TripSum::SELECT_ORIGIN_POLYGON		= "SELECT_ORIGIN_POLYGON";
char * TripSum::SELECT_DESTINATION_POLYGON	= "SELECT_DESTINATION_POLYGON";
char * TripSum::SELECT_PROBABILITY_FILE		= "SELECT_PROBABILITY_FILE";
char * TripSum::SELECT_PROBABILITY_FORMAT	= "SELECT_PROBABILITY_FORMAT";
char * TripSum::TIME_PERIOD_EQUIVALENCE		= "TIME_PERIOD_EQUIVALENCE";
char * TripSum::RANDOM_NUMBER_SEED			= "RANDOM_NUMBER_SEED";
char * TripSum::NEW_LINK_TRIP_END_FILE		= "NEW_LINK_TRIP_END_FILE";
char * TripSum::NEW_LINK_TRIP_END_FORMAT	= "NEW_LINK_TRIP_END_FORMAT";
char * TripSum::NEW_TIME_DISTRIBUTION		= "NEW_TIME_DISTRIBUTION";
char * TripSum::NEW_TRIP_TABLE_FILE			= "NEW_TRIP_TABLE_FILE";
char * TripSum::NEW_TRIP_TABLE_FORMAT		= "NEW_TRIP_TABLE_FORMAT";
char * TripSum::ZONE_EQUIVALENCE_FILE		= "ZONE_EQUIVALENCE_FILE";
char * TripSum::NEW_TRIP_TIME_FILE			= "NEW_TRIP_TIME_FILE";
char * TripSum::NEW_TRIP_DISTANCE_FILE		= "NEW_TRIP_DISTANCE_FILE";
char * TripSum::DISTANCE_CALCULATION		= "DISTANCE_CALCULATION";
char * TripSum::NEW_LOCATION_TRIP_END_FILE	= "NEW_LOCATION_TRIP_END_FILE";
char * TripSum::NEW_LOCATION_TRIP_END_FORMAT= "NEW_LOCATION_TRIP_END_FORMAT";
char * TripSum::NEW_ZONE_TRIP_END_FILE		= "NEW_ZONE_TRIP_END_FILE";
char * TripSum::NEW_ZONE_TRIP_END_FORMAT	= "NEW_ZONE_TRIP_END_FORMAT";
char * TripSum::CREATE_INDEPENDENT_TRIPS	= "CREATE_INDEPENDENT_TRIPS";
char * TripSum::NEW_HOUSEHOLD_LIST			= "NEW_HOUSEHOLD_LIST";
char * TripSum::STARTING_HOUSEHOLD_ID		= "STARTING_HOUSEHOLD_ID";
char * TripSum::STARTING_VEHICLE_ID			= "STARTING_VEHICLE_ID";
char * TripSum::ZONE_TARGET_FILE			= "ZONE_TARGET_FILE";
char * TripSum::TARGET_HOUSEHOLD_RANGE		= "TARGET_HOUSEHOLD_RANGE";

//---------------------------------------------------------
//	TripSum constructor
//---------------------------------------------------------

TripSum::TripSum (void) : Demand_Service ()
{
	Program ("TripSum");
	Version ("4.0.21");
	Title ("Summarize Trip File Demand");

	Network_File optional_network [] = {
		DIRECTORY, LINK, NODE, LANE_USE, ACTIVITY_LOCATION, PROCESS_LINK, END_NETWORK
	};
	Demand_File optional_demand [] = {
		TRIP, ACTIVITY, VEHICLE, NEW_TRIP, NEW_ACTIVITY, NEW_VEHICLE, END_DEMAND
	};

	char *keys [] = {
		HOUSEHOLD_LIST,
		HOUSEHOLD_PERSON_COUNT,
		TIME_OF_DAY_FORMAT,
		SUMMARY_TIME_PERIODS,
		SUMMARY_TIME_INCREMENT,
		SUMMARY_LENGTH_INCREMENT,
		SELECT_TRIP_MODES,
		SELECT_TRIP_PURPOSES,
		SELECT_ORIGIN_ZONES,
		SELECT_DESTINATION_ZONES,
		SELECT_ORIGIN_POLYGON,
		SELECT_DESTINATION_POLYGON,
		SELECT_PROBABILITY_FILE,
		SELECT_PROBABILITY_FORMAT,
		TIME_PERIOD_EQUIVALENCE,
		RANDOM_NUMBER_SEED,
		NEW_LINK_TRIP_END_FILE,
		NEW_LINK_TRIP_END_FORMAT,
		NEW_TIME_DISTRIBUTION,
		NEW_TRIP_TABLE_FILE,
		NEW_TRIP_TABLE_FORMAT,
		ZONE_EQUIVALENCE_FILE,
		NEW_TRIP_TIME_FILE,
		NEW_TRIP_DISTANCE_FILE,
		DISTANCE_CALCULATION,
		NEW_LOCATION_TRIP_END_FILE,
		NEW_LOCATION_TRIP_END_FORMAT,
		NEW_ZONE_TRIP_END_FILE,
		NEW_ZONE_TRIP_END_FORMAT,
		CREATE_INDEPENDENT_TRIPS,
		NEW_HOUSEHOLD_LIST,
		STARTING_HOUSEHOLD_ID,
		STARTING_VEHICLE_ID,
		ZONE_TARGET_FILE,
		TARGET_HOUSEHOLD_RANGE,
		NULL
	};
	char *reports [] = {
		"TOP_100_LINK_TRIP_ENDS",
		"TOP_100_LANE_TRIP_ENDS",
		"TOP_100_TRIP/CAPACITY_RATIOS",
		"PRINT_ZONE_EQUIVALENCIES",
		"TRIP_TIME_REPORT",
		"TRIP_LENGTH_SUMMARY",
		"TRIP_PURPOSE_SUMMARY",
		"MODE_LENGTH_SUMMARY",
		"MODE_PURPOSE_SUMMARY",
		"TIME_PERIOD_EQUIVALENCE",
		NULL
	};

	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	trip_flag = activity_flag = volume_flag = link_flag = time_flag = table_flag = zone_flag = purpose_flag = false;
	hhlist_flag = output_flag = distance_length_flag = time_length_flag = vehicle_flag = false;
	length_flag = trip_purpose_flag = mode_flag = mode_purpose_flag = split_flag = count_flag = false;	
	select_org_flag = select_des_flag = org_zone_flag = des_zone_flag = false;
	location_flag = factor_flag = loc_end_flag = zone_end_flag = create_flag = newhh_flag = false;

	synod_flag = false;
	nzone = 0;
	first_veh = veh_count = NULL;
	next_veh = NULL;
	synod = NULL;
	org_target = des_target = org_total = des_total = NULL;

	distance_method = STRAIGHT;
	total_in = total_out = total_used = total_hhold = 0;

	hhold_id = veh_id = 1;
	increment = 900;
	num_inc = 96;

	dir_array = (Dir_Array *) new TTime_Array ();
}

//---------------------------------------------------------
//	TripSum destructor
//---------------------------------------------------------

TripSum::~TripSum (void)
{
	if (nzone > 0) {
		for (int i=1; i < nzone; i++) {
			delete [] synod [i];
			delete [] first_veh [i];
			delete [] veh_count [i];
		}
		delete [] synod;
		delete [] first_veh;
		delete [] veh_count;

		delete [] next_veh;
		delete [] org_target;
		delete [] des_target;
		delete [] org_total;
		delete [] des_total;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	TripSum *exe = new TripSum ();

	return (exe->Start_Execution (commands, control));
}
