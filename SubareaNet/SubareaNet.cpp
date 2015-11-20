//*********************************************************
//	SubareaNet.cpp - Create a subarea network files
//*********************************************************

#include "SubareaNet.hpp"

char * SubareaNet::SUBAREA_BOUNDARY_POLYGON		= "SUBAREA_BOUNDARY_POLYGON";
char * SubareaNet::EXTERNAL_OFFSET_LENGTH		= "EXTERNAL_OFFSET_LENGTH";

//---------------------------------------------------------
//	SubareaNet constructor
//---------------------------------------------------------

SubareaNet::SubareaNet (void) : Network_Service ()
{
	Program ("SubareaNet");
	Version ("4.0.11");
	Title ("Subarea Network Files");

	Network_File required_network [] = {
		NODE, LINK, POCKET_LANE, LANE_CONNECTIVITY, PARKING, ACTIVITY_LOCATION, PROCESS_LINK, 
		NEW_NODE, NEW_LINK, NEW_POCKET_LANE, NEW_LANE_CONNECTIVITY, NEW_PARKING, 
		NEW_ACTIVITY_LOCATION, NEW_PROCESS_LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, ZONE, SHAPE, LANE_USE, TURN_PROHIBITION, TOLL,  
		UNSIGNALIZED_NODE, SIGNALIZED_NODE, TIMING_PLAN, PHASING_PLAN, DETECTOR, SIGNAL_COORDINATOR, 
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER,
		NEW_DIRECTORY, NEW_ZONE, NEW_SHAPE, NEW_LANE_USE, NEW_TURN_PROHIBITION, NEW_TOLL,  
		NEW_UNSIGNALIZED_NODE, NEW_SIGNALIZED_NODE, NEW_TIMING_PLAN, NEW_PHASING_PLAN, 
		NEW_DETECTOR, NEW_SIGNAL_COORDINATOR, 
		NEW_TRANSIT_STOP, NEW_TRANSIT_ROUTE, NEW_TRANSIT_SCHEDULE, NEW_TRANSIT_DRIVER,
		END_NETWORK
	};

	char *keys [] = {
		SUBAREA_BOUNDARY_POLYGON,
		EXTERNAL_OFFSET_LENGTH,
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);

	Key_List (keys);

	transit_flag = false;
	max_location = max_parking = max_access = max_stop = max_route = max_zone = 0;
	new_location = new_parking = new_access = new_stop = new_route = 0;
	nnode = nlink = nshort = nboundary = nshape = npocket = nconnect = 0;
	nlocation = nparking = naccess = nlane_use = nturn = ntoll = 0;
	nsign = nsignal = ntiming = nphasing = ndetector = ncoord = 0;
	nstop = nroute = nschedule = ndriver = 0;

	external_offset = Round (15);		//---- 15 meters ----	
}

//---------------------------------------------------------
//	SubareaNet destructor
//---------------------------------------------------------

SubareaNet::~SubareaNet (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	SubareaNet *exe = new SubareaNet ();

	return (exe->Start_Execution (commands, control));
}
