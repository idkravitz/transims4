//*********************************************************
//	ExportTransit.cpp - Transit Conversion Utility
//*********************************************************

#include "ExportTransit.hpp"

char * ExportTransit::WRITE_DWELL_FIELD				= "WRITE_DWELL_FIELD";
char * ExportTransit::WRITE_TIME_FIELD				= "WRITE_TIME_FIELD";
char * ExportTransit::WRITE_SPEED_FIELD				= "WRITE_SPEED_FIELD";
char * ExportTransit::MINIMUM_DWELL_TIME			= "MINIMUM_DWELL_TIME";
char * ExportTransit::MAXIMUM_DWELL_TIME			= "MAXIMUM_DWELL_TIME";
char * ExportTransit::TRANSIT_TIME_PERIODS			= "TRANSIT_TIME_PERIODS";
char * ExportTransit::PERIOD_TRAVEL_TIMES			= "PERIOD_TRAVEL_TIMES";
char * ExportTransit::SELECT_TRANSIT_MODES			= "SELECT_TRANSIT_MODES";
char * ExportTransit::SELECT_TRANSIT_ROUTES			= "SELECT_TRANSIT_ROUTES";

//---------------------------------------------------------
//	ExportTransit constructor
//---------------------------------------------------------

ExportTransit::ExportTransit (void) : Network_Service ()
{
	Program ("ExportTransit");
	Version ("4.0.5");
	Title ("Convert Transit Network to Route Nodes");

	Network_File required_network [] = {
		NODE, LINK, TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, 
		NEW_ROUTE_HEADER, NEW_ROUTE_NODES, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, NEW_DIRECTORY, END_NETWORK
	};

	char *keys [] = {
		WRITE_DWELL_FIELD,
		WRITE_TIME_FIELD,
		WRITE_SPEED_FIELD,
		MINIMUM_DWELL_TIME,
		MAXIMUM_DWELL_TIME,
		TRANSIT_TIME_PERIODS,
		PERIOD_TRAVEL_TIMES,
		SELECT_TRANSIT_MODES,
		SELECT_TRANSIT_ROUTES,
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);

	Key_List (keys);
	Notes_Name_Flag (true);

	min_dwell = 5;			//---- 5 seconds ----
	max_dwell = 30;			//---- 30 seconds ----

	nroute = nnodes = num_periods = 0;
	time_flag = dwell_flag = ttime_flag = speed_flag = false;

	header_file = NULL;
	nodes_file = NULL;
		
	memset (select_modes, '\0', sizeof (select_modes));
}

//---------------------------------------------------------
//	ExportTransit destructor
//---------------------------------------------------------

ExportTransit::~ExportTransit (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ExportTransit *exe = new ExportTransit ();

	return (exe->Start_Execution (commands, control));
}
