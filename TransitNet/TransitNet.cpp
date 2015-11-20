//*********************************************************
//	TransitNet.cpp - Transit Conversion Utility
//*********************************************************

#include "TransitNet.hpp"

char * TransitNet::PARK_AND_RIDE_FILE			= "PARK_AND_RIDE_FILE";
char * TransitNet::ZONE_EQUIVALENCE_FILE		= "ZONE_EQUIVALENCE_FILE";
char * TransitNet::STOP_SPACING_BY_AREATYPE		= "STOP_SPACING_BY_AREATYPE";
char * TransitNet::TRANSIT_TIME_PERIODS			= "TRANSIT_TIME_PERIODS";
char * TransitNet::TRANSIT_TRAVEL_TIME_FACTOR	= "TRANSIT_TRAVEL_TIME_FACTOR";
char * TransitNet::MINIMUM_DWELL_TIME			= "MINIMUM_DWELL_TIME";
char * TransitNet::INTERSECTION_STOP_TYPE		= "INTERSECTION_STOP_TYPE";
char * TransitNet::INTERSECTION_STOP_OFFSET		= "INTERSECTION_STOP_OFFSET";
char * TransitNet::TRANSIT_PROCESS_LINK_TIME	= "TRANSIT_PROCESS_LINK_TIME";
char * TransitNet::RANDOM_NUMBER_SEED			= "RANDOM_NUMBER_SEED";

//---------------------------------------------------------
//	TransitNet constructor
//---------------------------------------------------------

TransitNet::TransitNet (void) : Network_Service ()
{
	Program ("TransitNet");
	Version ("4.0.17");
	Title ("Transit Network Conversion Utility");

	Network_File required_network [] = {
		NODE, LINK, ACTIVITY_LOCATION, PROCESS_LINK, 
		ROUTE_HEADER, ROUTE_NODES,
		NEW_ACTIVITY_LOCATION, NEW_PROCESS_LINK, 
		NEW_TRANSIT_STOP, NEW_TRANSIT_ROUTE, NEW_TRANSIT_SCHEDULE, 
		NEW_TRANSIT_DRIVER, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, ZONE, PARKING, LANE_CONNECTIVITY,
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER,
		NEW_DIRECTORY, NEW_PARKING, END_NETWORK
	};

	char *keys [] = {
		PARK_AND_RIDE_FILE,
		ZONE_EQUIVALENCE_FILE,
		STOP_SPACING_BY_AREATYPE,
		TRANSIT_TIME_PERIODS,
		TRANSIT_TRAVEL_TIME_FACTOR,
		MINIMUM_DWELL_TIME,
		INTERSECTION_STOP_TYPE,
		INTERSECTION_STOP_OFFSET,
		TRANSIT_PROCESS_LINK_TIME,
		RANDOM_NUMBER_SEED,
		NULL
	};
	char *reports [] = {
		"FARE_ZONE_EQUIVALENCE",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);

	Report_List (reports);

	Key_List (keys);
	AB_Key_Flag (true);
	Notes_Name_Flag (true);

	parkride_flag = connect_flag = equiv_flag = dwell_flag = time_flag = speed_flag = false;

	naccess = nlocation = nparking = 0;
	route_edit = schedule_edit = driver_edit = 0;
	max_parking = max_access = max_location = max_stop = nparkride = 0;
	nstop = nroute = nschedule = ndriver = end_warnings = parking_warnings = connect_warnings = 0;
	min_dwell = 5;
	access_time = 5;
	stop_type = NEARSIDE;

	left_turn = compass.Num_Points () * -70 / 360;
	bus_code = Use_Code ("BUS");
	rail_code = Use_Code ("RAIL");
	stop_offset = 10;
	time_factor = NULL;

	node_array = (Node_Array *) new Node_Path_Array ();
	dir_array = (Dir_Array *) new Dir_Path_Array ();
}

//---------------------------------------------------------
//	TransitNet destructor
//---------------------------------------------------------

TransitNet::~TransitNet (void)
{
	if (time_factor != NULL) {
		delete [] time_factor;
		time_factor = NULL;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	TransitNet *exe = new TransitNet ();

	return (exe->Start_Execution (commands, control));
}
