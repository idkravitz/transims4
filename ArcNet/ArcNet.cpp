//*********************************************************
//	ArcNet.cpp - Network Conversion Utility
//*********************************************************

#include "ArcNet.hpp"

char * ArcNet::SUBZONE_DATA_FILE				= "SUBZONE_DATA_FILE";
char * ArcNet::ARCVIEW_DIRECTORY				= "ARCVIEW_DIRECTORY";
char * ArcNet::DRAW_NETWORK_LANES				= "DRAW_NETWORK_LANES";
char * ArcNet::LANE_WIDTH						= "LANE_WIDTH";
char * ArcNet::CENTER_ONEWAY_LINKS				= "CENTER_ONEWAY_LINKS";
char * ArcNet::LINK_DIRECTION_OFFSET			= "LINK_DIRECTION_OFFSET";
char * ArcNet::DRAW_AB_DIRECTION				= "DRAW_AB_DIRECTION";
char * ArcNet::POCKET_LANE_SIDE_OFFSET			= "POCKET_LANE_SIDE_OFFSET";
char * ArcNet::PARKING_SIDE_OFFSET				= "PARKING_SIDE_OFFSET";
char * ArcNet::ACTIVITY_LOCATION_SIDE_OFFSET	= "ACTIVITY_LOCATION_SIDE_OFFSET";
char * ArcNet::UNSIGNALIZED_NODE_SIDE_OFFSET	= "UNSIGNALIZED_NODE_SIDE_OFFSET";
char * ArcNet::UNSIGNALIZED_NODE_SETBACK		= "UNSIGNALIZED_NODE_SETBACK";
char * ArcNet::TRANSIT_STOP_SIDE_OFFSET			= "TRANSIT_STOP_SIDE_OFFSET";
char * ArcNet::TRANSIT_DIRECTION_OFFSET			= "TRANSIT_DIRECTION_OFFSET";
char * ArcNet::TRANSIT_TIME_PERIODS				= "TRANSIT_TIME_PERIODS";
char * ArcNet::TRANSIT_OVERLAP_FLAG				= "TRANSIT_OVERLAP_FLAG";
char * ArcNet::VISUALIZER_GUIDEWAY_FILE			= "VISUALIZER_GUIDEWAY_FILE";
char * ArcNet::DRAW_ONEWAY_ARROWS				= "DRAW_ONEWAY_ARROWS";
char * ArcNet::ONEWAY_ARROW_LENGTH				= "ONEWAY_ARROW_LENGTH";
char * ArcNet::ONEWAY_ARROW_SIDE_OFFSET			= "ONEWAY_ARROW_SIDE_OFFSET";
char * ArcNet::SELECT_TIME_PERIOD				= "SELECT_TIME_PERIOD";
char * ArcNet::USE_FIXED_SETBACK_FOR_ARCS       = "USE_FIXED_SETBACK_FOR_ARCS";
char * ArcNet::FIXED_SETBACK_DISTANCE           = "FIXED_SETBACK_DISTANCE";
char * ArcNet::USE_CONNECTIVITY_ARCS            = "USE_CONNECTIVITY_ARCS";


char ArcNet::shapename [] = "";

//---------------------------------------------------------
//	ArcNet constructor
//---------------------------------------------------------

ArcNet::ArcNet (void) : Network_Service ()
{
	Program ("ArcNet");
	Version ("4.0.23");
	Title ("Network Conversion Utility");

	Network_File required [] = {
		NODE, LINK, END_NETWORK
	};

	Network_File optional [] = {
		DIRECTORY, ZONE, SHAPE, POCKET_LANE, LANE_USE, 
		ACTIVITY_LOCATION, PARKING, PROCESS_LINK, 
		LANE_CONNECTIVITY, TURN_PROHIBITION, TOLL, UNSIGNALIZED_NODE, 
		SIGNALIZED_NODE, TIMING_PLAN, PHASING_PLAN, DETECTOR, SIGNAL_COORDINATOR,
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, 
		ROUTE_HEADER, ROUTE_NODES, END_NETWORK
	};

	char *keys [] = {
		SUBZONE_DATA_FILE,
		ARCVIEW_DIRECTORY,
		VISUALIZER_GUIDEWAY_FILE,
		DRAW_NETWORK_LANES,
		LANE_WIDTH,
		CENTER_ONEWAY_LINKS,
		LINK_DIRECTION_OFFSET,
		DRAW_AB_DIRECTION,
		POCKET_LANE_SIDE_OFFSET,
		PARKING_SIDE_OFFSET,
		ACTIVITY_LOCATION_SIDE_OFFSET,
		UNSIGNALIZED_NODE_SIDE_OFFSET,
		UNSIGNALIZED_NODE_SETBACK,
		TRANSIT_STOP_SIDE_OFFSET,
		TRANSIT_DIRECTION_OFFSET,
		TRANSIT_TIME_PERIODS,
		TRANSIT_OVERLAP_FLAG,
		DRAW_ONEWAY_ARROWS,
		ONEWAY_ARROW_LENGTH,
		ONEWAY_ARROW_SIDE_OFFSET,
		SELECT_TIME_PERIOD,
		USE_CONNECTIVITY_ARCS,
		USE_FIXED_SETBACK_FOR_ARCS,
		FIXED_SETBACK_DISTANCE,
		NULL
	};

	rounded_connectivity = false;
	setback_override_flag = false;
	setback_distance = 18.0;

	Required_Network_Files (required);
	Optional_Network_Files (optional);

	Key_List (keys);
	Notes_Name_Flag (true);

	projection.Add_Keys ();

	link_offset = 0.0;
	pocket_side = 2.0;
	parking_side = 5.0;
	location_side = 15.0;
	sign_side = 10.0;
	sign_setback = 25.0;
	stop_side = 5.0;
	route_offset = 0.0;
	lane_width = 3.5;
	arrow_length = lane_width * 2.0;
	arrow_side = lane_width / 2.0;

	route_flag = schedule_flag = driver_flag = route_node_flag = lanes_flag = center_flag = false;
	timing_flag = coordinator_flag = guideway_flag = arrow_flag = direction_flag = subzone_flag = false;
	select_time = false;
	overlap_flag = true;

	run_field = 0;
}

//---------------------------------------------------------
//	ArcNet destructor
//---------------------------------------------------------

ArcNet::~ArcNet (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ArcNet *exe = new ArcNet ();

	return (exe->Start_Execution (commands, control));
}

