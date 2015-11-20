//*********************************************************
//	SideFriction.cpp - Estimate Side Friction Delays
//*********************************************************

#include "SideFriction.hpp"

char * SideFriction::TIME_OF_DAY_FORMAT				= "TIME_OF_DAY_FORMAT";
char * SideFriction::TRANSIT_TIME_PERIODS			= "TRANSIT_TIME_PERIODS";
char * SideFriction::TRANSIT_LANE_BLOCK_TIMES		= "TRANSIT_LANE_BLOCK_TIMES";
char * SideFriction::TRANSIT_LANE_BLOCK_LENGTH		= "TRANSIT_LANE_BLOCK_LENGTH";
char * SideFriction::SELECT_TRANSIT_MODES			= "SELECT_TRANSIT_MODES";
char * SideFriction::SELECT_TRANSIT_ROUTES			= "SELECT_TRANSIT_ROUTES";
char * SideFriction::SELECT_SUBAREA_POLYGON			= "SELECT_SUBAREA_POLYGON";

//---------------------------------------------------------
//	SideFriction constructor
//---------------------------------------------------------

SideFriction::SideFriction (void) : Demand_Service ()
{
	Program ("SideFriction");
	Version ("4.0.0");
	Title ("Estimate Side Friction Delays");

	Network_File required_network [] = {
		LINK, NODE, NEW_LANE_USE, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, LANE_USE, SHAPE, TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, 
		TRANSIT_DRIVER, NEW_DIRECTORY, END_NETWORK
	};
	
	Demand_File required_demand [] = {
		END_DEMAND
	};

	char *keys [] = {
		TIME_OF_DAY_FORMAT,
		TRANSIT_TIME_PERIODS,
		TRANSIT_LANE_BLOCK_TIMES,
		TRANSIT_LANE_BLOCK_LENGTH,
		SELECT_TRANSIT_MODES,
		SELECT_TRANSIT_ROUTES,
		SELECT_SUBAREA_POLYGON,
		NULL
	};
	char *reports [] = {
		"DELAY_SUMMARY_REPORT",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);

	Key_List (keys);
	Report_List (reports);

	Lane_Use (true);
	
	new_file = NULL;

	num_use = new_use = 0;

	route_flag = subarea_flag = false;
	stop_length = Round (20);
	stop_times = NULL;
}

//---------------------------------------------------------
//	SideFriction destructor
//---------------------------------------------------------

SideFriction::~SideFriction (void)
{
	if (stop_times != NULL) {
		delete [] stop_times;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	SideFriction *exe = new SideFriction ();

	return (exe->Start_Execution (commands, control));
}

