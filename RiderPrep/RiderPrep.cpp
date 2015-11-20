//*********************************************************
//	RiderPrep.cpp - Prepare Transit Ridership Data
//*********************************************************

#include "RiderPrep.hpp"

char * RiderPrep::SUBAREA_RIDERSHIP_FILE	= "SUBAREA_RIDERSHIP_FILE";
char * RiderPrep::SUBAREA_RIDERSHIP_FORMAT	= "SUBAREA_RIDERSHIP_FORMAT";
char * RiderPrep::OUTPUT_ALL_TRANSIT_STOPS	= "OUTPUT_ALL_TRANSIT_STOPS";
char * RiderPrep::TIME_OF_DAY_FORMAT		= "TIME_OF_DAY_FORMAT";

//---------------------------------------------------------
//	RiderPrep constructor
//---------------------------------------------------------

RiderPrep::RiderPrep (void) : Demand_Service ()
{
	Program ("RiderPrep");
	Version ("4.0.0");
	Title ("Prepare Transit Ridership Data");

	Network_File required_network [] = {
		LINK, NODE, TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, 
		TRANSIT_DRIVER, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, END_NETWORK
	};
	
	Demand_File required_demand [] = {
		RIDERSHIP, NEW_RIDERSHIP, END_DEMAND
	};

	char *keys [] = {
		SUBAREA_RIDERSHIP_FILE,
		SUBAREA_RIDERSHIP_FORMAT,
		OUTPUT_ALL_TRANSIT_STOPS,
		TIME_OF_DAY_FORMAT,
		NULL
	};
	char *reports [] = {
		"LINE_RIDERSHIP_REPORT",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);

	Key_List (keys);
	Report_List (reports);
	Notes_Name_Flag (true);

	stops_flag = false;

	line_array = (Line_Array *) new Rider_Array ();
}

//---------------------------------------------------------
//	RiderPrep destructor
//---------------------------------------------------------

RiderPrep::~RiderPrep (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	RiderPrep *exe = new RiderPrep ();

	return (exe->Start_Execution (commands, control));
}

