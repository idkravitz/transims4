//*********************************************************
//	Network_Service.cpp - network data class
//*********************************************************

#include "Network_Service.hpp"

#include "Utility.hpp"

#include <stdarg.h>

char * Network_Service::CREATE_NOTES_AND_NAME_FIELDS	= "CREATE_NOTES_AND_NAME_FIELDS";

char * Network_Service::file_error = "%s file is required for %s file processing";

//---------------------------------------------------------
//	Network_Service constructor
//---------------------------------------------------------

Network_Service::Network_Service (void) : Execution_Service (), Network_Code ()
{
	static char *keys [] = {
		"NET_DIRECTORY",
		"NET_NODE_TABLE",
		"NET_ZONE_TABLE",
		"NET_SHAPE_TABLE",
		"NET_LINK_TABLE",
		"NET_POCKET_LANE_TABLE",
		"NET_LANE_USE_TABLE",
		"NET_TOLL_TABLE",
		"NET_LANE_CONNECTIVITY_TABLE",
		"NET_TURN_PROHIBITION_TABLE",
		"NET_PARKING_TABLE",
		"NET_ACTIVITY_LOCATION_TABLE",
		"NET_PROCESS_LINK_TABLE",
		"NET_UNSIGNALIZED_NODE_TABLE",
		"NET_SIGNALIZED_NODE_TABLE",
		"NET_PHASING_PLAN_TABLE",
		"NET_TIMING_PLAN_TABLE",
		"NET_DETECTOR_TABLE",
		"NET_SIGNAL_COORDINATOR_TABLE",
		"NET_TRANSIT_STOP_TABLE",
		"NET_TRANSIT_FARE_TABLE",
		"NET_TRANSIT_ROUTE_TABLE",
		"NET_TRANSIT_SCHEDULE_TABLE",
		"NET_TRANSIT_DRIVER_TABLE",
		"ROUTE_HEADER_FILE",
		"ROUTE_NODES_FILE",
		"NEW_DIRECTORY",
		"NEW_NODE_TABLE",
		"NEW_ZONE_TABLE",
		"NEW_SHAPE_TABLE",
		"NEW_LINK_TABLE",
		"NEW_POCKET_LANE_TABLE",
		"NEW_LANE_USE_TABLE",
		"NEW_TOLL_TABLE",
		"NEW_LANE_CONNECTIVITY_TABLE",
		"NEW_TURN_PROHIBITION_TABLE",
		"NEW_PARKING_TABLE",
		"NEW_ACTIVITY_LOCATION_TABLE",
		"NEW_PROCESS_LINK_TABLE",
		"NEW_UNSIGNALIZED_NODE_TABLE",
		"NEW_SIGNALIZED_NODE_TABLE",
		"NEW_PHASING_PLAN_TABLE",
		"NEW_TIMING_PLAN_TABLE",
		"NEW_DETECTOR_TABLE",
		"NEW_SIGNAL_COORDINATOR_TABLE",
		"NEW_TRANSIT_STOP_TABLE",
		"NEW_TRANSIT_FARE_TABLE",
		"NEW_TRANSIT_ROUTE_TABLE",
		"NEW_TRANSIT_SCHEDULE_TABLE",
		"NEW_TRANSIT_DRIVER_TABLE",
		"NEW_ROUTE_HEADER_FILE",
		"NEW_ROUTE_NODES_FILE",
		CREATE_NOTES_AND_NAME_FIELDS,
		NULL
	};

	static char *formats [] = {
		"NET_DEFAULT_FORMAT",
		"NET_NODE_FORMAT",
		"NET_ZONE_FORMAT",
		"NET_SHAPE_FORMAT",
		"NET_LINK_FORMAT",
		"NET_POCKET_LANE_FORMAT",
		"NET_LANE_USE_FORMAT",
		"NET_TOLL_FORMAT",
		"NET_LANE_CONNECTIVITY_FORMAT",
		"NET_TURN_PROHIBITION_FORMAT",
		"NET_PARKING_FORMAT",
		"NET_ACTIVITY_LOCATION_FORMAT",
		"NET_PROCESS_LINK_FORMAT",
		"NET_UNSIGNALIZED_NODE_FORMAT",
		"NET_SIGNALIZED_NODE_FORMAT",
		"NET_PHASING_PLAN_FORMAT",
		"NET_TIMING_PLAN_FORMAT",
		"NET_DETECTOR_FORMAT",
		"NET_SIGNAL_COORDINATOR_FORMAT",
		"NET_TRANSIT_STOP_FORMAT",
		"NET_TRANSIT_FARE_FORMAT",
		"NET_TRANSIT_ROUTE_FORMAT",
		"NET_TRANSIT_SCHEDULE_FORMAT",
		"NET_TRANSIT_DRIVER_FORMAT",
		"ROUTE_HEADER_FORMAT",
		"ROUTE_NODES_FORMAT",
		"NEW_DEFAULT_FORMAT",
		"NEW_NODE_FORMAT",
		"NEW_ZONE_FORMAT",
		"NEW_SHAPE_FORMAT",
		"NEW_LINK_FORMAT",
		"NEW_POCKET_LANE_FORMAT",
		"NEW_LANE_USE_FORMAT",
		"NEW_TOLL_FORMAT",
		"NEW_LANE_CONNECTIVITY_FORMAT",
		"NEW_TURN_PROHIBITION_FORMAT",
		"NEW_PARKING_FORMAT",
		"NEW_ACTIVITY_LOCATION_FORMAT",
		"NEW_PROCESS_LINK_FORMAT",
		"NEW_UNSIGNALIZED_NODE_FORMAT",
		"NEW_SIGNALIZED_NODE_FORMAT",
		"NEW_PHASING_PLAN_FORMAT",
		"NEW_TIMING_PLAN_FORMAT",
		"NEW_DETECTOR_FORMAT",
		"NEW_SIGNAL_COORDINATOR_FORMAT",
		"NEW_TRANSIT_STOP_FORMAT",
		"NEW_TRANSIT_FARE_FORMAT",
		"NEW_TRANSIT_ROUTE_FORMAT",
		"NEW_TRANSIT_SCHEDULE_FORMAT",
		"NEW_TRANSIT_DRIVER_FORMAT",
		"NEW_ROUTE_HEADER_FORMAT",
		"NEW_ROUTE_NODES_FORMAT",
		NULL
	};

	Key_List (keys);
	Key_List (formats);

	Check_Data (false);
	Renumber (false);
	AB_Key_Flag (false);
	Lane_Use (false);
	Notes_Name_Flag (false);
	Set_Projection (NULL);

	Resolution (10);
	compass.Set_Points (360);
	Bearing_Offset (Round (15));

	//---- initialize the file information ----

	for (int i=0; i < END_NETWORK; i++) {
		network_file [i].key = keys [i];
		network_file [i].format = formats [i];
		network_file [i].file = NULL;
		network_file [i].flag = false;
		network_file [i].option = false;
		network_file [i].data = false;
		network_file [i].access = (i < NEW_DIRECTORY) ? READ : CREATE;

		Disable_Control_Key (keys [i]);
		Disable_Control_Key (formats [i]);
	}
	num_fare_zone = 0;
	zero_flag = false;

	network_directory = new char [1];
	network_directory [0] = '\0';
	
	new_directory = new char [1];
	new_directory [0] = '\0';

	node_array = NULL;
	zone_array = NULL;
	shape_array = NULL;
	link_array = NULL;
	dir_array = NULL;
	ab_key_array = NULL;
	pocket_array = NULL;
	lane_use_array = NULL;
	link_use_array = NULL;
	toll_array = NULL;
	connect_array = NULL;
	turn_array = NULL;
	parking_array = NULL;
	location_array = NULL;
	access_array = NULL;
	sign_array = NULL;
	signal_array = NULL;
	timing_array = NULL;
	phasing_array = NULL;
	detector_array = NULL;
	coordinator_array = NULL;
	stop_array = NULL;
	fare_array = NULL;
	line_array = NULL;
	driver_array = NULL;
	route_array = NULL;

	control_flag = false;
}

//---------------------------------------------------------
//	Network_Service destructor
//---------------------------------------------------------

Network_Service::~Network_Service (void)
{
	for (int i=0; i < END_NETWORK; i++) {
		if (network_file [i].file != NULL) {
			delete network_file [i].file;
			network_file [i].file = NULL;
		}
	}
	if (network_directory != NULL) {
		delete [] network_directory;
		network_directory = NULL;
	}
	if (new_directory != NULL) {
		delete [] new_directory;
		new_directory = NULL;
	}
	if (node_array != NULL) {
		delete node_array;
		node_array = NULL;
	}
	if (zone_array != NULL) {
		delete zone_array;
		zone_array = NULL;
	}
	if (shape_array != NULL) {
		delete shape_array;
		shape_array = NULL;
	}
	if (link_array != NULL) {
		delete link_array;
		link_array = NULL;
	}
	if (dir_array != NULL) {
		delete dir_array;
		dir_array = NULL;
	}
	if (ab_key_array != NULL) {
		delete ab_key_array;
		ab_key_array = NULL;
	}
	if (pocket_array != NULL) {
		delete pocket_array;
		pocket_array = NULL;
	}
	if (lane_use_array != NULL) {
		delete lane_use_array;
		lane_use_array = NULL;
	}
	if (link_use_array != NULL) {
		delete link_use_array;
		link_use_array = NULL;
	}
	if (toll_array != NULL) {
		delete toll_array;
		toll_array = NULL;
	}
	if (connect_array != NULL) {
		delete connect_array;
		connect_array = NULL;
	}
	if (turn_array != NULL) {
		delete turn_array;
		turn_array = NULL;
	}
	if (parking_array != NULL) {
		delete parking_array;
		parking_array = NULL;
	}
	if (location_array != NULL) {
		delete location_array;
		location_array = NULL;
	}
	if (access_array != NULL) {
		delete access_array;
		access_array = NULL;
	}
	if (sign_array != NULL) {
		delete sign_array;
		sign_array = NULL;
	}
	if (signal_array != NULL) {
		delete signal_array;
		signal_array = NULL;
	}
	if (timing_array != NULL) {
		delete timing_array;
		timing_array = NULL;
	}
	if (phasing_array != NULL) {
		delete phasing_array;
		phasing_array = NULL;
	}
	if (detector_array != NULL) {
		delete detector_array;
		detector_array = NULL;
	}
	if (coordinator_array != NULL) {
		delete coordinator_array;
		coordinator_array = NULL;
	}
	if (stop_array != NULL) {
		delete stop_array;
		stop_array = NULL;
	}
	if (fare_array != NULL) {
		delete fare_array;
		fare_array = NULL;
	}
	if (line_array != NULL) {
		delete line_array;
		line_array = NULL;
	}
	if (driver_array != NULL) {
		delete driver_array;
		driver_array = NULL;
	}
	if (route_array != NULL) {
		delete route_array;
		route_array = NULL;
	}
}

//---------------------------------------------------------
//	Required_Network_Files
//---------------------------------------------------------

void Network_Service::Required_Network_Files (Network_File files [])
{
	for (int i=0; files [i] != END_NETWORK; i++) {
		File_Data *file = network_file + files [i];

		file->flag = true;
		Activate_Control_Key (file->key);
		Activate_Control_Key (file->format);
	}
}

//---------------------------------------------------------
//	Optional_Network_Files
//---------------------------------------------------------

void Network_Service::Optional_Network_Files (Network_File files [])
{
	for (int i=0; files [i] != END_NETWORK; i++) {
		File_Data *file = network_file + files [i];

		file->option = true;
		Activate_Control_Key (file->key);
		Activate_Control_Key (file->format);
	}
}

//---------------------------------------------------------
//	Resolution
//---------------------------------------------------------

void Network_Service::Resolution (int value)
{
	if (value < 1 || value > 100) {
		Error ("Resolution %d is Out of Range (1..100)", value);
	}
	resolution = value;
	res_round = value / 2;
}

//-----------------------------------------------------------
//	Network_Directory
//-----------------------------------------------------------

void Network_Service::Network_Directory (char *dir) 
{ 
	if (dir != NULL) {
		delete [] network_directory;

		int len = (int) strlen (dir);

		network_directory = new char [len + 2];

		if (network_directory == NULL) {
			Error ("Insufficient Memory for Network Directory");
		}
		str_cpy (network_directory, len + 2, dir);

		Fix_Directory (network_directory);
	} else {
		network_directory [0] = '\0';
	}
}

//-----------------------------------------------------------
//	New_Directory
//-----------------------------------------------------------

void Network_Service::New_Directory (char *dir) 
{ 
	if (dir != NULL) {
		delete [] new_directory;

		int len = (int) strlen (dir);

		new_directory = new char [len + 2];

		if (new_directory == NULL) {
			Error ("Insufficient Memory for New Network Directory");
		}
		str_cpy (new_directory, len + 2, dir);

		Fix_Directory (new_directory);
	} else {
		new_directory [0] = '\0';
	}
}

//-----------------------------------------------------------
//	Network_Copy_Header
//-----------------------------------------------------------

bool Network_Service::Network_Copy_Header (Network_File from, Network_File to)
{ 
	if (Network_File_Flag (from) && Network_File_Flag (to)) {
		Db_Header *file, *new_file;

		file = (Db_Header *) Network_Db_Base (from);
		new_file = (Db_Header *) Network_Db_Base (to);

		new_file->Clear_Fields ();
		new_file->Replicate_Fields (file);

		return (new_file->Write_Header ());
	}
	return (false);
}

//-----------------------------------------------------------
//	Check_Message
//-----------------------------------------------------------

bool Network_Service::Check_Message (char *format, ...) 
{ 
	char text [STRING_BUFFER];

	if (format == NULL) {
		Error (format);
	}
	va_list  args;
	va_start (args, format);
	str_args (text, sizeof (text), format, args);
	va_end (args);

	if (Check_Data ()) {
		Error (text);
	} else {
		Warning (text);
	}
	return (false);
}
