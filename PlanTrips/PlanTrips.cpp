//*********************************************************
//	PlanTrips.cpp - create trips from plans
//*********************************************************

#include "PlanTrips.hpp"

char * PlanTrips::PLAN_FILE					= "PLAN_FILE";
char * PlanTrips::PLAN_FORMAT				= "PLAN_FORMAT";
char * PlanTrips::NODE_LIST_PATHS			= "NODE_LIST_PATHS";
char * PlanTrips::HOUSEHOLD_LIST			= "HOUSEHOLD_LIST";
char * PlanTrips::NEW_PLAN_FILE				= "NEW_PLAN_FILE";
char * PlanTrips::NEW_PLAN_FORMAT			= "NEW_PLAN_FORMAT";
char * PlanTrips::NEW_HOUSEHOLD_LIST		= "NEW_HOUSEHOLD_LIST";
char * PlanTrips::TIME_OF_DAY_FORMAT		= "TIME_OF_DAY_FORMAT";
char * PlanTrips::CREATE_INDEPENDENT_TRIPS	= "CREATE_INDEPENDENT_TRIPS";

//---------------------------------------------------------
//	PlanTrips constructor
//---------------------------------------------------------

PlanTrips::PlanTrips (void) : Demand_Service ()
{
	Program ("PlanTrips");
	Version ("4.0.11");
	Title ("Create Trips from Plans");

	Network_File required_network [] = {
		PROCESS_LINK, END_NETWORK
	};
	Network_File optional_network [] = {
		DIRECTORY, END_NETWORK
	};
	Demand_File optional_demand [] = {
		TRIP, NEW_TRIP, ACTIVITY, NEW_ACTIVITY, VEHICLE, NEW_VEHICLE, END_DEMAND
	};

	char *keys [] = {
		PLAN_FILE,
		PLAN_FORMAT,
		NODE_LIST_PATHS,
		HOUSEHOLD_LIST,
		NEW_PLAN_FILE,
		NEW_PLAN_FORMAT,
		NEW_HOUSEHOLD_LIST,
		TIME_OF_DAY_FORMAT,
		CREATE_INDEPENDENT_TRIPS,
		NULL
	};
	
	char *reports [] = {
		"TIME_SHIFT_SUMMARY",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	num_trips = num_act = num_adjust = hhold_id = veh_id = 0;
	trip_flag = new_trip_flag = act_flag = new_act_flag = plan_flag = hhold_flag = hhlist_flag = false;
	create_flag = false;
	tot_adjust = 0.0;

	trip_file = NULL;
	act_file = NULL;

	plan_list.Initialize (sizeof (Plan_Data **), 20);
	parking_access.Initialize (sizeof (Location_Access));
	stop_access.Initialize (sizeof (Location_Access));

	memset (start_offset, '\0', sizeof (start_offset));
	memset (end_offset, '\0', sizeof (end_offset));
	memset (mid_offset, '\0', sizeof (mid_offset));
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	PlanTrips *exe = new PlanTrips ();

	return (exe->Start_Execution (commands, control));
}
