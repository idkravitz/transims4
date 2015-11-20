//*********************************************************
//	ArcDelay.cpp - ArcView Delay Files
//*********************************************************

#include "ArcDelay.hpp"

char * ArcDelay::LINK_DATA_FILE				= "LINK_DATA_FILE";
char * ArcDelay::LINK_DIRECTION_FILE		= "LINK_DIRECTION_FILE";
char * ArcDelay::SYSTEM_EVENT_FILE			= "SYSTEM_EVENT_FILE";
char * ArcDelay::ARCVIEW_LINK_DATA_FILE		= "ARCVIEW_LINK_DATA_FILE";
char * ArcDelay::ARCVIEW_LINK_DIR_FILE		= "ARCVIEW_LINK_DIR_FILE";
char * ArcDelay::ARCVIEW_DELAY_FILE			= "ARCVIEW_DELAY_FILE";
char * ArcDelay::ARCVIEW_INTERSECTION_FILE	= "ARCVIEW_INTERSECTION_FILE";
char * ArcDelay::ARCVIEW_TURN_FILE			= "ARCVIEW_TURN_FILE";
char * ArcDelay::ARCVIEW_TRAFFIC_IMAGE		= "ARCVIEW_TRAFFIC_IMAGE";
char * ArcDelay::ARCVIEW_TRAFFIC_IMAGE_x	= "ARCVIEW_TRAFFIC_IMAGE_*";
char * ArcDelay::TRAFFIC_IMAGE_ATTRIBUTE	= "TRAFFIC_IMAGE_ATTRIBUTE";
char * ArcDelay::TRAFFIC_IMAGE_ATTRIBUTE_x	= "TRAFFIC_IMAGE_ATTRIBUTE_*";
char * ArcDelay::VEHICLE_TYPE_DISTRIBUTION	= "VEHICLE_TYPE_DISTRIBUTION";
char * ArcDelay::RANDOM_NUMBER_SEED			= "RANDOM_NUMBER_SEED";
char * ArcDelay::DRAW_VEHICLE_SHAPES		= "DRAW_VEHICLE_SHAPES";
char * ArcDelay::DRAW_NETWORK_LANES			= "DRAW_NETWORK_LANES";
char * ArcDelay::LANE_WIDTH					= "LANE_WIDTH";
char * ArcDelay::CENTER_ONEWAY_LINKS		= "CENTER_ONEWAY_LINKS";
char * ArcDelay::LINK_DIRECTION_OFFSET		= "LINK_DIRECTION_OFFSET";
char * ArcDelay::BANDWIDTH_FIELD			= "BANDWIDTH_FIELD";
char * ArcDelay::BANDWIDTH_SCALING_FACTOR	= "BANDWIDTH_SCALING_FACTOR";
char * ArcDelay::MINIMUM_BANDWIDTH_VALUE	= "MINIMUM_BANDWIDTH_VALUE";
char * ArcDelay::MINIMUM_BANDWIDTH_SIZE		= "MINIMUM_BANDWIDTH_SIZE";
char * ArcDelay::MAXIMUM_BANDWIDTH_SIZE		= "MAXIMUM_BANDWIDTH_SIZE";
char * ArcDelay::MAXIMUM_SHAPE_ANGLE		= "MAXIMUM_SHAPE_ANGLE";
char * ArcDelay::MINIMUM_SHAPE_LENGTH		= "MINIMUM_SHAPE_LENGTH";
char * ArcDelay::TIME_OF_DAY_FORMAT			= "TIME_OF_DAY_FORMAT";
char * ArcDelay::SELECT_TIME_PERIODS		= "SELECT_TIME_PERIODS";
char * ArcDelay::SELECT_TIME_INCREMENT		= "SELECT_TIME_INCREMENT";
char * ArcDelay::SELECT_SUBAREA_POLYGON		= "SELECT_SUBAREA_POLYGON";
char * ArcDelay::ADD_LINK_DIRECTION_INDEX	= "ADD_LINK_DIRECTION_INDEX";
char * ArcDelay::IGNORE_TIME_RANGE_FIELDS	= "IGNORE_TIME_RANGE_FIELDS";

//---------------------------------------------------------
//	ArcDelay constructor
//---------------------------------------------------------

ArcDelay::ArcDelay (void) : Demand_Service ()
{
	Program ("ArcDelay");
	Version ("4.0.17");
	Title ("ArcView Link Delay Files");

	Network_File required_network [] = {
		NODE, LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, SHAPE, LANE_CONNECTIVITY, LANE_USE, POCKET_LANE, PHASING_PLAN, END_NETWORK
	};

	Demand_File optional_demand [] = {
		PERFORMANCE, VEHICLE_TYPE, END_DEMAND
	};

	char *keys [] = {
		LINK_DATA_FILE,
		LINK_DIRECTION_FILE,
		SYSTEM_EVENT_FILE,
		ARCVIEW_LINK_DATA_FILE,
		ARCVIEW_LINK_DIR_FILE,
		ARCVIEW_DELAY_FILE,
		ARCVIEW_INTERSECTION_FILE,
		ARCVIEW_TURN_FILE,
		ARCVIEW_TRAFFIC_IMAGE,
		ARCVIEW_TRAFFIC_IMAGE_x,
		TRAFFIC_IMAGE_ATTRIBUTE,
		TRAFFIC_IMAGE_ATTRIBUTE_x,
		VEHICLE_TYPE_DISTRIBUTION,
		RANDOM_NUMBER_SEED,
		DRAW_VEHICLE_SHAPES,
		DRAW_NETWORK_LANES,
		LANE_WIDTH,
		CENTER_ONEWAY_LINKS,
		LINK_DIRECTION_OFFSET,
		BANDWIDTH_FIELD,
		BANDWIDTH_SCALING_FACTOR,
		MINIMUM_BANDWIDTH_VALUE,
		MINIMUM_BANDWIDTH_SIZE,
		MAXIMUM_BANDWIDTH_SIZE,
		MAXIMUM_SHAPE_ANGLE,
		MINIMUM_SHAPE_LENGTH,
		TIME_OF_DAY_FORMAT,
		SELECT_TIME_PERIODS,
		SELECT_TIME_INCREMENT,
		SELECT_SUBAREA_POLYGON,
		ADD_LINK_DIRECTION_INDEX,
		IGNORE_TIME_RANGE_FIELDS,
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);

	Lane_Use (true);
	projection.Add_Keys ();

	link_offset = 0.0;
	lane_width = 1.0;
	center_flag = lanes_flag = turn_flag = subarea_flag = index_flag = image_flag = shape_flag = false;
	delay_flag = link_dir_flag = link_data_flag = intersection_flag = width_flag = event_flag = false;
	width_field = green_field = yellow_field = red_field = split_field = 0;
	width_factor = 1.0;
	min_value = 0.0;
	min_width = 0.01;
	max_width = 1000.0;
	max_angle = 45;
	min_length = 5;

	image_data.Initialize (sizeof (Image_Data), 10);

	pocket_index.Initialize (sizeof (Pocket_Index), 2);

	signal_time.Initialize (sizeof (Signal_Time), 2);

	node_array = (Node_Array *) new Node_List_Array ();
}

//---------------------------------------------------------
//	ArcDelay destructor
//---------------------------------------------------------

ArcDelay::~ArcDelay (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ArcDelay *exe = new ArcDelay ();

	return (exe->Start_Execution (commands, control));
}
