//*********************************************************
//	SubareaPlans.cpp - Create a Subarea Plan file
//*********************************************************

#include "SubareaPlans.hpp"

char * SubareaPlans::PLAN_FILE						= "PLAN_FILE";
char * SubareaPlans::PLAN_FORMAT					= "PLAN_FORMAT";
char * SubareaPlans::NODE_LIST_PATHS				= "NODE_LIST_PATHS";
char * SubareaPlans::SUBAREA_BOUNDARY_POLYGON		= "SUBAREA_BOUNDARY_POLYGON";
char * SubareaPlans::EXTERNAL_OFFSET_LENGTH			= "EXTERNAL_OFFSET_LENGTH";
char * SubareaPlans::NEW_PLAN_FILE					= "NEW_PLAN_FILE";
char * SubareaPlans::NEW_PLAN_FORMAT				= "NEW_PLAN_FORMAT";
char * SubareaPlans::NEW_HOUSEHOLD_LIST				= "NEW_HOUSEHOLD_LIST";
char * SubareaPlans::RETIME_EARLY_ARRIVALS			= "RETIME_EARLY_ARRIVALS";
char * SubareaPlans::ZONE_EQUIVALENCE_FILE			= "ZONE_EQUIVALENCE_FILE";
char * SubareaPlans::TIME_PERIOD_EQUIVALENCE		= "TIME_PERIOD_EQUIVALENCE";
char * SubareaPlans::TIME_OF_DAY_FORMAT				= "TIME_OF_DAY_FORMAT";
char * SubareaPlans::SELECT_TIME_PERIODS			= "SELECT_TIME_PERIODS";

//---------------------------------------------------------
//	SubareaPlans constructor
//---------------------------------------------------------

SubareaPlans::SubareaPlans (void) : Demand_Service ()
{
	Program ("SubareaPlans");
	Version ("4.0.33");
	Title ("Subarea Plan and Vehicle Files");

	Network_File required_network [] = {
		NODE, LINK, ACTIVITY_LOCATION, PARKING, PROCESS_LINK, END_NETWORK
	};
	Network_File optional_network [] = {
		DIRECTORY, LANE_CONNECTIVITY, NEW_DIRECTORY, 
		NEW_ACTIVITY_LOCATION, NEW_PARKING, NEW_PROCESS_LINK, 
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER,
		NEW_TRANSIT_STOP, NEW_TRANSIT_ROUTE, NEW_TRANSIT_SCHEDULE, NEW_TRANSIT_DRIVER,
		END_NETWORK
	};
	Demand_File required_demand [] = {
		VEHICLE, NEW_VEHICLE, END_DEMAND
	};
	Demand_File optional_demand [] = {
		LINK_DELAY, NEW_BOUNDARY_SPEED, END_DEMAND
	};

	char *keys [] = {
		PLAN_FILE,
		PLAN_FORMAT,
		NODE_LIST_PATHS,
		SUBAREA_BOUNDARY_POLYGON,
		EXTERNAL_OFFSET_LENGTH,
		NEW_PLAN_FILE,
		NEW_PLAN_FORMAT,
		NEW_HOUSEHOLD_LIST,
		RETIME_EARLY_ARRIVALS,
		ZONE_EQUIVALENCE_FILE,
		TIME_PERIOD_EQUIVALENCE,
		SELECT_TIME_PERIODS,
		NULL
	};
	char *reports [] = {
		"TRIP_SUMMARY_REPORT",
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
	Enable_Partitions (true);

	AB_Key_Flag  (true);

	trips = NULL;

	type_flag = true;
	transit_flag = output_flag = range_flag = false;
	delay_flag = location_flag = parking_flag = access_flag = extend_flag = early_flag = false;
	trip_flag = zone_flag = time_flag = turn_flag = hhlist_flag = boundary_flag = false;
	nlink = nshort = nlocation = nparking = naccess = num_adjust = total_adjust = 0;
	max_location = max_parking = max_access = max_vehicle = max_zone = 0;
	num_in_in = num_out_in = num_in_out = num_out_out = 0;
	leg_in_in = leg_out_in = leg_in_out = leg_out_out = 0;

	max_stop = max_route = 0;
	new_stop = new_route = 0;
	nstop = nroute = nschedule = ndriver = 0;
	current_traveler = current_trip = current_leg = boarding_stop = last_location = last_time = 0;

	external_offset = Round (15);		//---- 15 meters ----

	boundary.Data_Size (sizeof (Boundary));

	Compress_Vehicles (false);
}

//---------------------------------------------------------
//	SubareaPlans destructor
//---------------------------------------------------------

SubareaPlans::~SubareaPlans (void)
{
	if (trip_flag && trips != NULL) {
		int p, i, num_zone, num_time;

		if (zone_flag) {
			num_zone = zone_equiv.Num_Groups ();
		} else {
			num_zone = 2;
		}
		if (time_flag) {
			num_time = time_equiv.Num_Periods ();
		} else {
			num_time = 1;
		}

		for (p=1; p <= num_time; p++) {
			for (i=1; i <= num_zone; i++) {
				if (trips [p] [i] != NULL) {
					delete [] trips [p] [i];
				}
			}
			if (trips [p] != NULL) {
				delete [] trips [p];
			}
		}
		delete [] trips;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	SubareaPlans *exe = new SubareaPlans ();

	return (exe->Start_Execution (commands, control));
}
