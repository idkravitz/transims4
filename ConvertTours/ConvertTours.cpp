//*********************************************************
//	ConvertTours.cpp - Trip Table Conversion Utility
//*********************************************************

#include "ConvertTours.hpp"

char * ConvertTours::TOUR_DATA_FILE					= "TOUR_DATA_FILE";
char * ConvertTours::TOUR_DATA_FORMAT				= "TOUR_DATA_FORMAT";
char * ConvertTours::TOUR_HOUSEHOLD_FIELD			= "TOUR_HOUSEHOLD_FIELD";
char * ConvertTours::TOUR_PERSON_FIELD				= "TOUR_PERSON_FIELD";
char * ConvertTours::TOUR_NUMBER_FIELD				= "TOUR_NUMBER_FIELD";
char * ConvertTours::TOUR_PURPOSE_FIELD				= "TOUR_PURPOSE_FIELD";
char * ConvertTours::TOUR_MODE_FIELD				= "TOUR_MODE_FIELD";
char * ConvertTours::TOUR_ORIGIN_FIELD				= "TOUR_ORIGIN_FIELD";
char * ConvertTours::TOUR_DESTINATION_FIELD			= "TOUR_DESTINATION_FIELD";
char * ConvertTours::TOUR_STOP_OUT_FIELD			= "TOUR_STOP_OUT_FIELD";
char * ConvertTours::TOUR_STOP_IN_FIELD				= "TOUR_STOP_IN_FIELD";
char * ConvertTours::TOUR_START_FIELD				= "TOUR_START_FIELD";
char * ConvertTours::TOUR_RETURN_FIELD				= "TOUR_RETURN_FIELD";
char * ConvertTours::TOUR_GROUP_FIELD				= "TOUR_GROUP_FIELD";
char * ConvertTours::TOUR_GROUP_SCRIPT				= "TOUR_GROUP_SCRIPT";

char * ConvertTours::HOUSEHOLD_LIST					= "HOUSEHOLD_LIST";
char * ConvertTours::STARTING_HOUSEHOLD_ID			= "STARTING_HOUSEHOLD_ID";
char * ConvertTours::STARTING_VEHICLE_ID			= "STARTING_VEHICLE_ID";
char * ConvertTours::TIME_OF_DAY_FORMAT				= "TIME_OF_DAY_FORMAT";
char * ConvertTours::ADDITIONAL_TRAVEL_TIME			= "ADDITIONAL_TRAVEL_TIME";
char * ConvertTours::ZONE_LOCATION_MAP_FILE			= "ZONE_LOCATION_MAP_FILE";
char * ConvertTours::RANDOM_NUMBER_SEED				= "RANDOM_NUMBER_SEED";
char * ConvertTours::ZONE_EQUIVALENCE_FILE			= "ZONE_EQUIVALENCE_FILE";
char * ConvertTours::TRAVEL_TIME_FILE				= "TRAVEL_TIME_FILE";
char * ConvertTours::NEW_DIURNAL_FILE				= "NEW_DIURNAL_FILE";

char * ConvertTours::TRIP_TIME_FILE_x				= "TRIP_TIME_FILE_*";
char * ConvertTours::TRIP_TIME_FORMAT_x				= "TRIP_TIME_FORMAT_*";
char * ConvertTours::TRIP_TIME_FIELD_x				= "TRIP_TIME_FIELD_*";
char * ConvertTours::TIME_PERIOD_RANGE_x			= "TIME_PERIOD_RANGE_*";
char * ConvertTours::TIME_CONTROL_POINT_x			= "TIME_CONTROL_POINT_*";
char * ConvertTours::ORIGIN_WEIGHT_FIELD_x			= "ORIGIN_WEIGHT_FIELD_*";
char * ConvertTours::DESTINATION_WEIGHT_FIELD_x		= "DESTINATION_WEIGHT_FIELD_*";
char * ConvertTours::DISTANCE_WEIGHT_FLAG_x			= "DISTANCE_WEIGHT_FLAG_*";
char * ConvertTours::STOP_WEIGHT_FIELD_x			= "STOP_WEIGHT_FIELD_*";
char * ConvertTours::TRIP_PURPOSE_CODE_x			= "TRIP_PURPOSE_CODE_*";
char * ConvertTours::TRAVEL_MODE_CODE_x				= "TRAVEL_MODE_CODE_*";
char * ConvertTours::TRIP_STOP_DURATION_x			= "TRIP_STOP_DURATION_*";
char * ConvertTours::AVERAGE_TRAVEL_SPEED_x			= "AVERAGE_TRAVEL_SPEED_*";
char * ConvertTours::VEHICLE_TYPE_x					= "VEHICLE_TYPE_*";
char * ConvertTours::VEHICLE_SUBTYPE_x				= "VEHICLE_SUBTYPE_*";
char * ConvertTours::TRIP_SCALING_FACTOR_x			= "TRIP_SCALING_FACTOR_*";
char * ConvertTours::TRIP_ADJUSTMENT_FACTORS_x		= "TRIP_ADJUSTMENT_FACTORS_*";
char * ConvertTours::TRIP_ADJUSTMENT_FORMAT_x		= "TRIP_ADJUSTMENT_FORMAT_*";
char * ConvertTours::TIME_PERIOD_EQUIVALENCE_x		= "TIME_PERIOD_EQUIVALENCE_*";

//---------------------------------------------------------
//	ConvertTours constructor
//---------------------------------------------------------

ConvertTours::ConvertTours (void) : Demand_Service ()
{
	Program ("ConvertTours");
	Version ("4.0.15");
	Title ("Tour Data Conversion Utility");

	Network_File required_network [] = {
		ACTIVITY_LOCATION, PROCESS_LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, LINK, ZONE, END_NETWORK
	};

	Demand_File required_demand [] = {
		NEW_TRIP, NEW_HOUSEHOLD, NEW_POPULATION, NEW_VEHICLE, END_DEMAND
	};

	Demand_File optional_demand [] = {
		TRIP, HOUSEHOLD, POPULATION, VEHICLE, VEHICLE_TYPE, END_DEMAND
	};

	char *keys [] = {
		TOUR_DATA_FILE,
		TOUR_DATA_FORMAT,
		TOUR_HOUSEHOLD_FIELD,
		TOUR_PERSON_FIELD,
		TOUR_NUMBER_FIELD,
		TOUR_PURPOSE_FIELD,
		TOUR_MODE_FIELD,
		TOUR_ORIGIN_FIELD,
		TOUR_DESTINATION_FIELD,
		TOUR_STOP_OUT_FIELD,
		TOUR_STOP_IN_FIELD,
		TOUR_START_FIELD,
		TOUR_RETURN_FIELD,
		TOUR_GROUP_FIELD,
		TOUR_GROUP_SCRIPT,
		HOUSEHOLD_LIST,
		STARTING_HOUSEHOLD_ID,
		STARTING_VEHICLE_ID,
		TIME_OF_DAY_FORMAT,
		ADDITIONAL_TRAVEL_TIME,
		ZONE_LOCATION_MAP_FILE,
		RANDOM_NUMBER_SEED,
		ZONE_EQUIVALENCE_FILE,
		TRAVEL_TIME_FILE,
		NEW_DIURNAL_FILE,
		TRIP_TIME_FILE_x,
		TRIP_TIME_FORMAT_x,
		TRIP_TIME_FIELD_x,
		TIME_PERIOD_RANGE_x,
		TIME_CONTROL_POINT_x,
		ORIGIN_WEIGHT_FIELD_x,
		DESTINATION_WEIGHT_FIELD_x,
		DISTANCE_WEIGHT_FLAG_x,
		STOP_WEIGHT_FIELD_x,
		TRIP_PURPOSE_CODE_x,
		TRAVEL_MODE_CODE_x,
		TRIP_STOP_DURATION_x,
		AVERAGE_TRAVEL_SPEED_x,
		VEHICLE_TYPE_x,
		VEHICLE_SUBTYPE_x,
		TRIP_SCALING_FACTOR_x,
		TRIP_ADJUSTMENT_FACTORS_x,
		TRIP_ADJUSTMENT_FORMAT_x,
		TIME_PERIOD_EQUIVALENCE_x,
		NULL
	};
	char *reports [] = {
		"TOUR_GROUP_SCRIPT",
		"TOUR_GROUP_STACK",
		"PRINT_ZONE_EQUIVALENCIES",
		"TIME_PERIOD_EQUIVALENCE",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);

	equiv_flag = skim_flag = zone_flag = script_flag = diurnal_flag = zone_loc_flag = hhlist_flag = false;

	num_zone = 0;
	additional_time = 600;
	num_trip = tot_trips = tot_errors = nhhold = nperson = 0;
	tot_add = tot_del = 0.0;
	skim_ptr = NULL;

	trip_copy = hhold_copy = pop_copy = veh_copy = 0;
	max_hh_in = max_veh_in = max_hhlist = 0;

	location_array = (Location_Array *) new Convert_Location_Array ();
}

//---------------------------------------------------------
//	ConvertTours destructor
//---------------------------------------------------------

ConvertTours::~ConvertTours (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ConvertTours *exe = new ConvertTours ();

	return (exe->Start_Execution (commands, control));
}
