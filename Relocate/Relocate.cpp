//*********************************************************
//	Relocate.cpp - Summarize Trip File Demand
//*********************************************************

#include "Relocate.hpp"

char * Relocate::ZONE_RELOCATION_FILE		= "ZONE_RELOCATION_FILE";
char * Relocate::RANDOM_NUMBER_SEED			= "RANDOM_NUMBER_SEED";
char * Relocate::PLAN_FILE					= "PLAN_FILE";
char * Relocate::PLAN_FORMAT				= "PLAN_FORMAT";
char * Relocate::NODE_LIST_PATHS			= "NODE_LIST_PATHS";
char * Relocate::NEW_PLAN_FILE				= "NEW_PLAN_FILE";
char * Relocate::NEW_PLAN_FORMAT			= "NEW_PLAN_FORMAT";
char * Relocate::OLD_NODE_FILE				= "OLD_NODE_FILE";
char * Relocate::OLD_LINK_FILE				= "OLD_LINK_FILE";
char * Relocate::OLD_ACTIVITY_LOCATIONS		= "OLD_ACTIVITY_LOCATIONS";
char * Relocate::OLD_PARKING_FILE			= "OLD_PARKING_FILE";
char * Relocate::OLD_PROCESS_LINKS			= "OLD_PROCESS_LINKS";
char * Relocate::NEW_HOUSEHOLD_LIST			= "NEW_HOUSEHOLD_LIST";
char * Relocate::NEW_LOCATION_MAP_FILE		= "NEW_LOCATION_MAP_FILE";
char * Relocate::NEW_PARKING_MAP_FILE		= "NEW_PARKING_MAP_FILE";
char * Relocate::NEW_CHANGE_FILE			= "NEW_CHANGE_FILE";

//---------------------------------------------------------
//	Relocate constructor
//---------------------------------------------------------

Relocate::Relocate (void) : Demand_Service ()
{
	Program ("Relocate");
	Version ("4.0.13");
	Title ("Update Activity Locations");

	Network_File required_network [] = {
		ACTIVITY_LOCATION, END_NETWORK
	};
	Network_File optional_network [] = {
		DIRECTORY, LINK, NODE, PARKING, PROCESS_LINK, END_NETWORK
	};
	Demand_File optional_demand [] = {
		TRIP, ACTIVITY, VEHICLE, NEW_TRIP, NEW_ACTIVITY, NEW_VEHICLE, END_DEMAND
	};

	char *keys [] = {
		ZONE_RELOCATION_FILE,
		RANDOM_NUMBER_SEED,
		PLAN_FILE,
		PLAN_FORMAT,
		NODE_LIST_PATHS,
		NEW_PLAN_FILE,
		NEW_PLAN_FORMAT,
		OLD_NODE_FILE,
		OLD_LINK_FILE,
		OLD_ACTIVITY_LOCATIONS,
		OLD_PARKING_FILE,
		OLD_PROCESS_LINKS,
		NEW_HOUSEHOLD_LIST,
		NEW_LOCATION_MAP_FILE,
		NEW_PARKING_MAP_FILE,
		NEW_CHANGE_FILE,
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Enable_Partitions (true);
	AB_Key_Flag (true);

	activity_flag = trip_flag = plan_flag = hhlist_flag = vehicle_flag = loop_flag = false;
	loc_map_flag = park_map_flag = network_flag = zone_flag = change_flag = false;
	type_flag = true;

	total_in = total_out = total_update = total_hhold = 0;

	activity_map.Initialize (sizeof (Map_Data));
	parking_map.Initialize (sizeof (Map_Data));
	node_map.Initialize (sizeof (Map_Data));
	old_link_map.Initialize (sizeof (Map_Data));
	old_new_map.Initialize (sizeof (Map_Data));
	loc_park.Initialize (sizeof (Map_Data));
	veh_map.Initialize (sizeof (Map_Data));
	zone_weight.Initialize (sizeof (Zone_Weight), 2);
}

//---------------------------------------------------------
//	Relocate destructor
//---------------------------------------------------------

Relocate::~Relocate (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	Relocate *exe = new Relocate ();

	return (exe->Start_Execution (commands, control));
}
