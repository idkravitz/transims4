//*********************************************************
//	CoordMatch.cpp - coordinate adjustment and equivalence
//*********************************************************

#include "CoordMatch.hpp"

char * CoordMatch::REFERENCE_NODE_FILE		= "REFERENCE_NODE_FILE";
char * CoordMatch::NODE_MAP_FILE			= "NODE_MAP_FILE";
char * CoordMatch::NEW_NODE_MAP_FILE		= "NEW_NODE_MAP_FILE";
char * CoordMatch::MAXIMUM_SNAP_DISTANCE	= "MAXIMUM_SNAP_DISTANCE";
char * CoordMatch::REPLACE_NODE_NUMBERS		= "REPLACE_NODE_NUMBERS";

//---------------------------------------------------------
//	CoordMatch constructor
//---------------------------------------------------------

CoordMatch::CoordMatch (void) : Network_Service ()
{
	Program ("CoordMatch");
	Version ("4.0.0");
	Title ("Coordinate Adjustment and Equivalence");

	Network_File required_network [] = {
		NODE, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, NEW_DIRECTORY, NEW_NODE, END_NETWORK
	};

	char *keys [] = {
		REFERENCE_NODE_FILE,
		NODE_MAP_FILE,
		NEW_NODE_MAP_FILE,
		MAXIMUM_SNAP_DISTANCE,
		REPLACE_NODE_NUMBERS,
		NULL
	};
	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);

	Key_List (keys);

	num_new_nodes = num_new_maps = num_match = 0;
	ref_node_flag = node_map_flag = new_map_flag = new_node_flag = replace_flag = false;
	max_distance = 1000;

	node_map.Initialize (sizeof (Node_Map), 100);
}

//---------------------------------------------------------
//	CoordMatch destructor
//---------------------------------------------------------

CoordMatch::~CoordMatch (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	CoordMatch *exe = new CoordMatch ();

	return (exe->Start_Execution (commands, control));
}

