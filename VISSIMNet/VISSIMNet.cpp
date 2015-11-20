//*********************************************************
//	VISSIMNet.cpp - Convert Network to VISSIM XML format
//*********************************************************

#include "VISSIMNet.hpp"

char * VISSIMNet::TIME_OF_DAY_FORMAT	= "TIME_OF_DAY_FORMAT";
char * VISSIMNet::SELECT_TIME_PERIOD	= "SELECT_TIME_PERIOD";
char * VISSIMNet::NEW_VISSIM_XML_FILE	= "NEW_VISSIM_XML_FILE";

//---------------------------------------------------------
//	VISSIMNet constructor
//---------------------------------------------------------

VISSIMNet::VISSIMNet (void) : Demand_Service ()
{
	Program ("VISSIMNet");
	Version ("4.0.7");
	Title ("Convert Network to VISSIM XML Format");

	Network_File required_network [] = {
		NODE, LINK, LANE_CONNECTIVITY, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, SHAPE, POCKET_LANE, UNSIGNALIZED_NODE, 
		PARKING, ACTIVITY_LOCATION, PROCESS_LINK, SIGNALIZED_NODE, 
		TIMING_PLAN, PHASING_PLAN, DETECTOR, SIGNAL_COORDINATOR, 
		NEW_NODE, NEW_LINK, NEW_LANE_CONNECTIVITY,
		NEW_DIRECTORY, NEW_SHAPE, NEW_POCKET_LANE, NEW_UNSIGNALIZED_NODE, 
		NEW_PARKING, NEW_ACTIVITY_LOCATION, NEW_PROCESS_LINK, NEW_SIGNALIZED_NODE,
		NEW_TIMING_PLAN, NEW_PHASING_PLAN, NEW_DETECTOR, NEW_SIGNAL_COORDINATOR, 
		END_NETWORK
	};

	Demand_File optional_demand [] = {
		VEHICLE_TYPE, END_DEMAND
	};

	char *keys [] = {
		TIME_OF_DAY_FORMAT,
		SELECT_TIME_PERIOD,
		NEW_VISSIM_XML_FILE,
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);

	new_flag = false;

	node_array = (Node_Array *) new Node_List_Array ();
	link_array = (Link_Array *) new Link_Name_Array ();
	dir_array = (Dir_Array *) new Dir_List_Array ();

	link_splits.Initialize (sizeof (Link_Splits));
}

//---------------------------------------------------------
//	VISSIMNet destructor
//---------------------------------------------------------

VISSIMNet::~VISSIMNet (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	VISSIMNet *exe = new VISSIMNet ();

	return (exe->Start_Execution (commands, control));
}

