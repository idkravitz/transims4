//*********************************************************
//	GISNet.cpp - Arcview Network Conversion
//*********************************************************

#include "GISNet.hpp"

char * GISNet::GIS_NODE_FILE		= "GIS_NODE_FILE";
char * GISNet::GIS_LINK_FILE		= "GIS_LINK_FILE";
char * GISNet::CONVERSION_SCRIPT	= "CONVERSION_SCRIPT";
char * GISNet::FIRST_LINK_NUMBER	= "FIRST_LINK_NUMBER";
char * GISNet::FIRST_NODE_NUMBER	= "FIRST_NODE_NUMBER";
char * GISNet::MAXIMUM_SHAPE_ANGLE	= "MAXIMUM_SHAPE_ANGLE";
char * GISNet::MINIMUM_SHAPE_LENGTH	= "MINIMUM_SHAPE_LENGTH";
char * GISNet::UPDATE_LINK_LENGTHS	= "UPDATE_LINK_LENGTHS";

//---------------------------------------------------------
//	GISNet constructor
//---------------------------------------------------------

GISNet::GISNet (void) : Network_Service ()
{
	Program ("GISNet");
	Version ("4.0.9");
	Title ("GIS Network Conversion");

	Network_File required_network [] = {
		NEW_NODE, NEW_LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		NEW_DIRECTORY, NEW_SHAPE, END_NETWORK
	};

	char *keys [] = {
		GIS_NODE_FILE,
		GIS_LINK_FILE,
		CONVERSION_SCRIPT,
		FIRST_LINK_NUMBER,
		FIRST_NODE_NUMBER,
		MAXIMUM_SHAPE_ANGLE,
		MINIMUM_SHAPE_LENGTH,
		UPDATE_LINK_LENGTHS,
		NULL
	};

	char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);

	Key_List (keys);
	Report_List (reports);

	projection.Add_Keys ();

	convert_flag = node_flag = update_flag = false;
	first_link = first_node = 1;
	max_angle = 45;
	min_length = 5;
	
	nlink = nnode = nshape = nrec = num_in = 0;
	diff = Round (1.0);		//---- one meter resolution ----

	link_file = NULL;
	node_file = NULL;
	shape_file = NULL;

	node_array = new Node_Array ();
}

//---------------------------------------------------------
//	GISNet destructor
//---------------------------------------------------------

GISNet::~GISNet (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	GISNet *exe = new GISNet ();

	return (exe->Start_Execution (commands, control));
}

