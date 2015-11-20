//*********************************************************
//	TripPrep.cpp - Prepare Trip and Activity Files
//*********************************************************

#include "TripPrep.hpp"

char * TripPrep::HOUSEHOLD_LIST					= "HOUSEHOLD_LIST";
char * TripPrep::MERGE_TRIP_FILE				= "MERGE_TRIP_FILE";
char * TripPrep::MERGE_TRIP_FORMAT				= "MERGE_TRIP_FORMAT";
char * TripPrep::MERGE_ACTIVITY_FILE			= "MERGE_ACTIVITY_FILE";
char * TripPrep::MERGE_ACTIVITY_FORMAT			= "MERGE_ACTIVITY_FORMAT";
char * TripPrep::MERGE_VEHICLE_FILE				= "MERGE_VEHICLE_FILE";
char * TripPrep::MERGE_VEHICLE_FORMAT			= "MERGE_VEHICLE_FORMAT";
char * TripPrep::TIME_OF_DAY_FORMAT				= "TIME_OF_DAY_FORMAT";
char * TripPrep::SELECT_HOUSEHOLDS				= "SELECT_HOUSEHOLDS";
char * TripPrep::SELECT_TIME_PERIODS			= "SELECT_TIME_PERIODS";
char * TripPrep::SELECT_TRIP_MODES				= "SELECT_TRIP_MODES";
char * TripPrep::SELECT_TRIP_PURPOSES			= "SELECT_TRIP_PURPOSES";
char * TripPrep::SELECT_ORIGIN_ZONES			= "SELECT_ORIGIN_ZONES";
char * TripPrep::SELECT_DESTINATION_ZONES		= "SELECT_DESTINATION_ZONES";
char * TripPrep::SELECT_ORIGIN_POLYGON			= "SELECT_ORIGIN_POLYGON";
char * TripPrep::SELECT_DESTINATION_POLYGON		= "SELECT_DESTINATION_POLYGON";
char * TripPrep::SELECT_PROBABILITY_FILE		= "SELECT_PROBABILITY_FILE";
char * TripPrep::SELECT_PROBABILITY_FORMAT		= "SELECT_PROBABILITY_FORMAT";
char * TripPrep::ZONE_EQUIVALENCE_FILE			= "ZONE_EQUIVALENCE_FILE";
char * TripPrep::TIME_PERIOD_EQUIVALENCE		= "TIME_PERIOD_EQUIVALENCE";
char * TripPrep::SELECTION_PERCENTAGE			= "SELECTION_PERCENTAGE";
char * TripPrep::RANDOM_NUMBER_SEED				= "RANDOM_NUMBER_SEED";
char * TripPrep::START_TIME_SHIFT				= "START_TIME_SHIFT";
char * TripPrep::DATA_PROCESSING_SCRIPT			= "DATA_PROCESSING_SCRIPT";
char * TripPrep::OUTPUT_ALL_RECORDS				= "OUTPUT_ALL_RECORDS";
char * TripPrep::CREATE_INDEPENDENT_TRIPS		= "CREATE_INDEPENDENT_TRIPS";
char * TripPrep::NEW_HOUSEHOLD_LIST				= "NEW_HOUSEHOLD_LIST";
char * TripPrep::STARTING_HOUSEHOLD_ID			= "STARTING_HOUSEHOLD_ID";
char * TripPrep::STARTING_VEHICLE_ID			= "STARTING_VEHICLE_ID";
char * TripPrep::ZONE_TARGET_FILE				= "ZONE_TARGET_FILE";
char * TripPrep::TARGET_HOUSEHOLD_RANGE			= "TARGET_HOUSEHOLD_RANGE";
char * TripPrep::MOVE_ORIGIN_FILE				= "MOVE_ORIGIN_FILE";
char * TripPrep::MOVE_DESTINATION_FILE			= "MOVE_DESTINATION_FILE";
char * TripPrep::CONVERT_ACTIVITIES_TO_TRIPS	= "CONVERT_ACTIVITIES_TO_TRIPS";

//---------------------------------------------------------
//	TripPrep constructor
//---------------------------------------------------------

TripPrep::TripPrep (void) : Demand_Service ()
{
	Program ("TripPrep");
	Version ("4.0.10");
	Title ("Prepare Trip and Activity Files");

	Network_File optional_network [] = {
		DIRECTORY, ACTIVITY_LOCATION, PROCESS_LINK, END_NETWORK
	};
	Demand_File optional_demand [] = {
		TRIP, ACTIVITY, VEHICLE, NEW_TRIP, NEW_ACTIVITY, NEW_VEHICLE, END_DEMAND
	};

	char *keys [] = {
		HOUSEHOLD_LIST,
		MERGE_TRIP_FILE,
		MERGE_TRIP_FORMAT,
		MERGE_ACTIVITY_FILE,
		MERGE_ACTIVITY_FORMAT,
		MERGE_VEHICLE_FILE,
		MERGE_VEHICLE_FORMAT,
		TIME_OF_DAY_FORMAT,
		SELECT_HOUSEHOLDS,
		SELECT_TIME_PERIODS,
		SELECT_TRIP_MODES,
		SELECT_TRIP_PURPOSES,
		SELECT_ORIGIN_ZONES,
		SELECT_DESTINATION_ZONES,
		SELECT_ORIGIN_POLYGON,
		SELECT_DESTINATION_POLYGON,
		SELECT_PROBABILITY_FILE,
		SELECT_PROBABILITY_FORMAT,
		ZONE_EQUIVALENCE_FILE,
		TIME_PERIOD_EQUIVALENCE,
		SELECTION_PERCENTAGE,
		RANDOM_NUMBER_SEED,
		START_TIME_SHIFT,
		DATA_PROCESSING_SCRIPT,
		OUTPUT_ALL_RECORDS,
		CREATE_INDEPENDENT_TRIPS,
		NEW_HOUSEHOLD_LIST,
		STARTING_HOUSEHOLD_ID,
		STARTING_VEHICLE_ID,
		ZONE_TARGET_FILE,
		TARGET_HOUSEHOLD_RANGE,
		MOVE_ORIGIN_FILE,
		MOVE_DESTINATION_FILE,
		CONVERT_ACTIVITIES_TO_TRIPS,
		NULL
	};
	char *reports [] = {
		"PRINT_ZONE_EQUIVALENCIES",
		"TIME_PERIOD_EQUIVALENCE",
		"DATA_PROCESSING_SCRIPT",
		"DATA_PROCESSING_STACK",
		NULL
	};

	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	trip_flag = activity_flag = purpose_flag = prob_flag = shift_flag = false;
	hhlist_flag = output_flag = vehicle_flag = all_flag = false;
	split_flag = zone_equiv_flag = synod_flag = script_flag = false;	
	select_org_flag = select_des_flag = org_zone_flag = des_zone_flag = false;
	location_flag = factor_flag = create_flag = newhh_flag = convert_flag = false;
	merge_trip_flag = merge_act_flag = merge_veh_flag = move_org_flag = move_des_flag = false;

	nzone = 0;
	first_veh = veh_count = NULL;
	next_veh = NULL;
	synod = NULL;
	org_target = des_target = org_total = des_total = NULL;
	probability = 1.0;
	shift_size = 0;

	total_in = total_out = total_used = total_hhold = 0;

	hhold_id = veh_id = 1;

	move_org_map.Initialize (2 * sizeof (int), 10);
	move_des_map.Initialize (2 * sizeof (int), 10);
	move_veh_map.Initialize (2 * sizeof (int), 10);
}

//---------------------------------------------------------
//	TripPrep destructor
//---------------------------------------------------------

TripPrep::~TripPrep (void)
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
	TripPrep *exe = new TripPrep ();

	return (exe->Start_Execution (commands, control));
}
