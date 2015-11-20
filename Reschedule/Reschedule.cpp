//*********************************************************
//	Reschedule.cpp - reschedule transit routes
//*********************************************************

#include "Reschedule.hpp"

char * Reschedule::SELECT_TRANSIT_MODES			= "SELECT_TRANSIT_MODES";
char * Reschedule::SELECT_TRANSIT_ROUTES		= "SELECT_TRANSIT_ROUTES";
char * Reschedule::SELECT_TIME_PERIODS			= "SELECT_TIME_PERIODS";
char * Reschedule::TIME_PERIOD_METHOD			= "TIME_PERIOD_METHOD";
char * Reschedule::TIME_OF_DAY_FORMAT			= "TIME_OF_DAY_FORMAT";
char * Reschedule::TRANSIT_TIME_PERIODS			= "TRANSIT_TIME_PERIODS";
char * Reschedule::TRANSIT_TRAVEL_TIME_FACTOR	= "TRANSIT_TRAVEL_TIME_FACTOR";
char * Reschedule::MINIMUM_DWELL_TIME			= "MINIMUM_DWELL_TIME";
char * Reschedule::MAXIMUM_DWELL_TIME			= "MAXIMUM_DWELL_TIME";
char * Reschedule::NEW_DISTRIBUTION_FILE		= "NEW_DISTRIBUTION_FILE";
char * Reschedule::RANDOM_SCHEDULE_SHIFT		= "RANDOM_SCHEDULE_SHIFT";
char * Reschedule::RANDOM_NUMBER_SEED			= "RANDOM_NUMBER_SEED";
char * Reschedule::RUN_START_TIME_FILE			= "RUN_START_TIME_FILE";
char * Reschedule::TRANSIT_SERVICE_FACTOR		= "TRANSIT_SERVICE_FACTOR";
char * Reschedule::SEGMENT_SCHEDULE_FACTOR		= "SEGMENT_SCHEDULE_FACTOR";
char * Reschedule::LINK_EQUIVALENCE_FILE		= "LINK_EQUIVALENCE_FILE";

//---------------------------------------------------------
//	Reschedule constructor
//---------------------------------------------------------

Reschedule::Reschedule (void) : Demand_Service ()
{
	Program ("Reschedule");
	Version ("4.0.12");
	Title ("Reschedule Transit Routes");

	Network_File required_network [] = {
		LINK, NODE, TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, 
		NEW_TRANSIT_SCHEDULE, END_NETWORK
	};
	Network_File optional_network [] = {
		DIRECTORY, ROUTE_HEADER, LANE_CONNECTIVITY, NEW_DIRECTORY, END_NETWORK
	};
	Demand_File optional_demand [] = {
		LINK_DELAY, RIDERSHIP, VEHICLE_TYPE, END_DEMAND
	};

	char *keys [] = {
		SELECT_TRANSIT_MODES,
		SELECT_TRANSIT_ROUTES,
		SELECT_TIME_PERIODS,
		TIME_PERIOD_METHOD,
		TIME_OF_DAY_FORMAT,
		TRANSIT_TIME_PERIODS,
		TRANSIT_TRAVEL_TIME_FACTOR,
		MINIMUM_DWELL_TIME,
		MAXIMUM_DWELL_TIME,
		NEW_DISTRIBUTION_FILE,
		RANDOM_SCHEDULE_SHIFT,
		RANDOM_NUMBER_SEED,
		RUN_START_TIME_FILE,
		TRANSIT_SERVICE_FACTOR,
		SEGMENT_SCHEDULE_FACTOR,
		LINK_EQUIVALENCE_FILE,
		NULL
	};
	char *reports [] = {
		"TOTAL_CHANGE_DISTRIBUTION",
		"PERIOD_CHANGE_DISTRIBUTIONS",
		"TIME_PERIOD_SUMMARY",
		"PRINT_LINK_EQUIVALENCIES",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);

	min_dwell = NULL;
	max_dwell = NULL;
	time_factor = NULL;
	delay_flag = turn_delay = diff_flag = rider_flag = veh_type_flag = route_flag = period_flag = false;
	header_flag = run_flag = service_flag = equiv_flag = false;

	method = RUN_MID;

	service_factor = link_factor = 1.0;
	
	line_array = (Line_Array *) new Rider_Array ();

	time_diff.Report_Numbers (TOTAL_CHANGE, PERIOD_CHANGE, PERIOD_SUM);
}

//---------------------------------------------------------
//	Reschedule destructor
//---------------------------------------------------------

Reschedule::~Reschedule (void)
{
	if (time_factor != NULL) {
		delete [] time_factor;
		time_factor = NULL;
	}
	if (min_dwell != NULL) {
		delete [] min_dwell;
		min_dwell = NULL;
	}
	if (max_dwell != NULL) {
		delete [] max_dwell;
		max_dwell = NULL;
	}
	time_diff.Clear ();
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	Reschedule *exe = new Reschedule ();

	return (exe->Start_Execution (commands, control));
}
