//*********************************************************
//	ConvertTrips.cpp - Trip Table Conversion Utility
//*********************************************************

#include "ConvertTrips.hpp"

char * ConvertTrips::STARTING_HOUSEHOLD_ID			= "STARTING_HOUSEHOLD_ID";
char * ConvertTrips::STARTING_VEHICLE_ID			= "STARTING_VEHICLE_ID";
char * ConvertTrips::TIME_OF_DAY_FORMAT				= "TIME_OF_DAY_FORMAT";
char * ConvertTrips::ADDITIONAL_TRAVEL_TIME			= "ADDITIONAL_TRAVEL_TIME";
char * ConvertTrips::ZONE_LOCATION_MAP_FILE			= "ZONE_LOCATION_MAP_FILE";
char * ConvertTrips::RANDOM_NUMBER_SEED				= "RANDOM_NUMBER_SEED";
char * ConvertTrips::ZONE_EQUIVALENCE_FILE			= "ZONE_EQUIVALENCE_FILE";
char * ConvertTrips::TRAVEL_TIME_FILE				= "TRAVEL_TIME_FILE";
char * ConvertTrips::TRIP_TABLE_FILE_x				= "TRIP_TABLE_FILE_*";
char * ConvertTrips::TRIP_TABLE_FORMAT_x			= "TRIP_TABLE_FORMAT_*";
char * ConvertTrips::TRIP_SCALING_FACTOR_x			= "TRIP_SCALING_FACTOR_*";
char * ConvertTrips::TRIP_TIME_FILE_x				= "TRIP_TIME_FILE_*";
char * ConvertTrips::TRIP_TIME_FORMAT_x				= "TRIP_TIME_FORMAT_*";
char * ConvertTrips::TRIP_TIME_SCRIPT_x				= "TRIP_TIME_SCRIPT_*";
char * ConvertTrips::TRIP_TIME_FIELD_x				= "TRIP_TIME_FIELD_*";
char * ConvertTrips::TIME_PERIOD_RANGE_x			= "TIME_PERIOD_RANGE_*";
char * ConvertTrips::TIME_CONTROL_POINT_x			= "TIME_CONTROL_POINT_*";
char * ConvertTrips::RETURN_TRIP_OFFSET_x			= "RETURN_TRIP_OFFSET_*";
char * ConvertTrips::ORIGIN_WEIGHT_FIELD_x			= "ORIGIN_WEIGHT_FIELD_*";
char * ConvertTrips::DESTINATION_WEIGHT_FIELD_x		= "DESTINATION_WEIGHT_FIELD_*";
char * ConvertTrips::DISTANCE_WEIGHT_FLAG_x			= "DISTANCE_WEIGHT_FLAG_*";
char * ConvertTrips::TRIP_PURPOSE_CODE_x			= "TRIP_PURPOSE_CODE_*";
char * ConvertTrips::TRAVEL_MODE_CODE_x				= "TRAVEL_MODE_CODE_*";
char * ConvertTrips::AVERAGE_TRAVEL_SPEED_x			= "AVERAGE_TRAVEL_SPEED_*";
char * ConvertTrips::VEHICLE_TYPE_x					= "VEHICLE_TYPE_*";
char * ConvertTrips::VEHICLE_SUBTYPE_x				= "VEHICLE_SUBTYPE_*";
char * ConvertTrips::TRIP_ADJUSTMENT_FACTORS_x		= "TRIP_ADJUSTMENT_FACTORS_*";
char * ConvertTrips::TRIP_ADJUSTMENT_FORMAT_x		= "TRIP_ADJUSTMENT_FORMAT_*";
char * ConvertTrips::TIME_PERIOD_EQUIVALENCE_x		= "TIME_PERIOD_EQUIVALENCE_*";

//---------------------------------------------------------
//	ConvertTrips constructor
//---------------------------------------------------------

ConvertTrips::ConvertTrips (void) : Demand_Service ()
{
	Program ("ConvertTrips");
	Version ("4.0.21");
	Title ("Trip Table Conversion Utility");

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
		STARTING_HOUSEHOLD_ID,
		STARTING_VEHICLE_ID,
		TIME_OF_DAY_FORMAT,
		ADDITIONAL_TRAVEL_TIME,
		ZONE_LOCATION_MAP_FILE,
		RANDOM_NUMBER_SEED,
		ZONE_EQUIVALENCE_FILE,
		TRAVEL_TIME_FILE,
		TRIP_TABLE_FILE_x,
		TRIP_TABLE_FORMAT_x,
		TRIP_SCALING_FACTOR_x,
		TRIP_TIME_FILE_x,
		TRIP_TIME_FORMAT_x,
		TRIP_TIME_SCRIPT_x,
		TRIP_TIME_FIELD_x,
		TIME_PERIOD_RANGE_x,
		TIME_CONTROL_POINT_x,
		RETURN_TRIP_OFFSET_x,
		ORIGIN_WEIGHT_FIELD_x,
		DESTINATION_WEIGHT_FIELD_x,
		DISTANCE_WEIGHT_FLAG_x,
		TRIP_PURPOSE_CODE_x,
		TRAVEL_MODE_CODE_x,
		AVERAGE_TRAVEL_SPEED_x,
		VEHICLE_TYPE_x,
		VEHICLE_SUBTYPE_x,
		TRIP_ADJUSTMENT_FACTORS_x,
		TRIP_ADJUSTMENT_FORMAT_x,
		TIME_PERIOD_EQUIVALENCE_x,
		NULL
	};
	char *reports [] = {
		"TRIP_TIME_SCRIPT",
		"TRIP_TIME_STACK",
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

	equiv_flag = skim_flag = zone_flag = zone_loc_flag = false;

	num_zone = 0;
	additional_time = 600;
	num_trip = tot_trips = tot_errors = nperson = 0;
	tot_add = tot_del = 0.0;
	group_number = 0;
	skim_ptr = NULL;

	trip_copy = hhold_copy = pop_copy = veh_copy = 0;
	max_hh_in = max_veh_in = 0;

	location_array = (Location_Array *) new Convert_Location_Array ();
}

//---------------------------------------------------------
//	ConvertTrips destructor
//---------------------------------------------------------

ConvertTrips::~ConvertTrips (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ConvertTrips *exe = new ConvertTrips ();

	return (exe->Start_Execution (commands, control));
}
