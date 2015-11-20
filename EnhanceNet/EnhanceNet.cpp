//*********************************************************
//	EnhanceNet.cpp - Intersection Control Utility
//*********************************************************

#include "EnhanceNet.hpp"

//---------------------------------------------------------
//	EnhanceNet constructor
//---------------------------------------------------------

EnhanceNet::EnhanceNet (void) : Network_Service ()
{
	Program ("EnhanceNet");
	Version ("4.0.1");
	Title ("Enhance Network Files");

	Network_File required_network [] = {
		NODE, LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, SHAPE, POCKET_LANE, LANE_CONNECTIVITY,
		NEW_DIRECTORY, NEW_LINK, NEW_LANE_CONNECTIVITY, END_NETWORK
	};

	char *keys [] = {
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);

	Key_List (keys);

	connect_flag = type_flag = false;

	node_array = (Node_Array *) new Node_List_Array ();
	dir_array = (Dir_Array *) new Dir_Connect_Array ();
}

//---------------------------------------------------------
//	EnhanceNet destructor
//---------------------------------------------------------

EnhanceNet::~EnhanceNet (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	EnhanceNet *exe = new EnhanceNet ();

	return (exe->Start_Execution (commands, control));
}

