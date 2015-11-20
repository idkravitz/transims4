//*********************************************************
//	DynusTNet.cpp - Convert Network to DynusT XML format
//*********************************************************

#include "DynusTNet.hpp"

char * DynusTNet::TIME_OF_DAY_FORMAT		= "TIME_OF_DAY_FORMAT";
char * DynusTNet::SELECT_TIME_PERIOD		= "SELECT_TIME_PERIOD";
char * DynusTNet::DYNUST_PROJECT_NAME		= "DYNUST_PROJECT_NAME";
char * DynusTNet::DYNUST_NETWORK_DIRECTORY	= "DYNUST_NETWORK_DIRECTORY";
char * DynusTNet::SATURATION_FLOW_RATES		= "SATURATION_FLOW_RATES";
char * DynusTNet::SERVICE_FLOW_RATES		= "SERVICE_FLOW_RATES";

//---------------------------------------------------------
//	DynusTNet constructor
//---------------------------------------------------------

DynusTNet::DynusTNet (void) : Demand_Service ()
{
	Program ("DynusTNet");
	Version ("4.0.4");
	Title ("Convert Network to DynusT Format");

	Network_File required_network [] = {
		NODE, LINK, LANE_CONNECTIVITY, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, ZONE, SHAPE, POCKET_LANE, UNSIGNALIZED_NODE, 
		PARKING, ACTIVITY_LOCATION, PROCESS_LINK, SIGNALIZED_NODE, 
		TIMING_PLAN, PHASING_PLAN, DETECTOR, SIGNAL_COORDINATOR, 
		END_NETWORK
	};

	Demand_File optional_demand [] = {
		VEHICLE_TYPE, END_DEMAND
	};

	char *keys [] = {
		TIME_OF_DAY_FORMAT,
		SELECT_TIME_PERIOD,
		DYNUST_PROJECT_NAME,
		DYNUST_NETWORK_DIRECTORY,
		SATURATION_FLOW_RATES,
		SERVICE_FLOW_RATES,
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	AB_Key_Flag (true);
	
	min_x = min_y = MAX_INTEGER;
	name_flag = false;

	node_array = (Node_Array *) new Node_List_Array ();
	link_array = (Link_Array *) new Link_Name_Array ();
	dir_array = (Dir_Array *) new Dir_List_Array ();

	for (int i=0; i <= EXTERNAL; i++) {
		sat_flow [i] = 1800;
		ser_flow [i] = 2200;
	}
	sat_flow [FREEWAY] = 2200;
	ser_flow [FREEWAY] = 2400;
}

//---------------------------------------------------------
//	DynusTNet destructor
//---------------------------------------------------------

DynusTNet::~DynusTNet (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	DynusTNet *exe = new DynusTNet ();

	return (exe->Start_Execution (commands, control));
}

