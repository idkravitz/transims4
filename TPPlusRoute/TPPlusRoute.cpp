//*********************************************************
//	TPPlusRoute.cpp - TP+ Network Conversion
//*********************************************************

#include "TPPlusRoute.hpp"

char * TPPlusRoute::TRANSIT_TIME_PERIODS	= "TRANSIT_TIME_PERIODS";
char * TPPlusRoute::PERIOD_TRAVEL_TIMES		= "PERIOD_TRAVEL_TIMES";
char * TPPlusRoute::FIRST_ROUTE_NUMBER		= "FIRST_ROUTE_NUMBER";
char * TPPlusRoute::TPPLUS_LINE_FILE_x		= "TPPLUS_LINE_FILE_*";
char * TPPlusRoute::TPPLUS_PERIOD_MAP_x		= "TPPLUS_PERIOD_MAP_*";
char * TPPlusRoute::TPPLUS_FLIP_ROUTE_x		= "TPPLUS_FLIP_ROUTE_*";
char * TPPlusRoute::TPPLUS_MODE_MAP_x		= "TPPLUS_MODE_MAP_*";
char * TPPlusRoute::TPPLUS_NODE_MAP_FILE	= "TPPLUS_NODE_MAP_FILE";


//---------------------------------------------------------
//	TPPlusRoute constructor
//---------------------------------------------------------

TPPlusRoute::TPPlusRoute (void) : Network_Service ()
{
	Program ("TPPlusRoute");
	Version ("4.0.4");
	Title ("TPPlus Transit Network Conversion");

	Network_File required_network [] = {
		NEW_ROUTE_HEADER, NEW_ROUTE_NODES, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, NODE, LINK, ROUTE_HEADER, ROUTE_NODES, NEW_DIRECTORY, END_NETWORK
	};

	char *keys [] = {
		TRANSIT_TIME_PERIODS,
		PERIOD_TRAVEL_TIMES,
		FIRST_ROUTE_NUMBER,
		TPPLUS_LINE_FILE_x,
		TPPLUS_PERIOD_MAP_x,
		TPPLUS_FLIP_ROUTE_x,
		TPPLUS_MODE_MAP_x,
		TPPLUS_NODE_MAP_FILE,
		NULL
	};
	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);

	Key_List (keys);

	AB_Key_Flag (true);

	time_flag = link_flag = node_flag = input_flag = map_flag = false;
	route_id = 1;
	nroute = nnodes = num_periods = num_match = num_modes = 0;
	headway = mode_map = NULL;

	node_map.Initialize (sizeof (Node_Map), 100);
	
	file_group.Initialize (sizeof (File_Group), 10);
}

//---------------------------------------------------------
//	TPPlusRoute destructor
//---------------------------------------------------------

TPPlusRoute::~TPPlusRoute (void)
{
	File_Group *ptr;

	for (ptr = (File_Group *) file_group.First (); ptr; ptr = (File_Group *) file_group.Next ()) {
		if (ptr->line_file != NULL) {
			delete ptr->line_file;
		}
		if (ptr->period_map != NULL) {
			delete [] ptr->period_map;
		}
	}
	if (headway != NULL) {
		delete [] headway;
	}
	if (mode_map != NULL) {
		delete [] mode_map;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	TPPlusRoute *exe = new TPPlusRoute ();

	return (exe->Start_Execution (commands, control));
}

