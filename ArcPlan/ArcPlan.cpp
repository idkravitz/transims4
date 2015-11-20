//*********************************************************
//	ArcPlan.cpp - Plan File Display Utility
//*********************************************************

#include "ArcPlan.hpp"

char * ArcPlan::PLAN_FILE						= "PLAN_FILE";
char * ArcPlan::PLAN_FORMAT						= "PLAN_FORMAT";
char * ArcPlan::PROBLEM_FILE					= "PROBLEM_FILE";
char * ArcPlan::PROBLEM_FORMAT					= "PROBLEM_FORMAT";
char * ArcPlan::NODE_LIST_PATHS					= "NODE_LIST_PATHS";
char * ArcPlan::HOUSEHOLD_LIST					= "HOUSEHOLD_LIST";
char * ArcPlan::ARCVIEW_PLAN_FILE				= "ARCVIEW_PLAN_FILE";
char * ArcPlan::ARCVIEW_PROBLEM_FILE			= "ARCVIEW_PROBLEM_FILE";
char * ArcPlan::ARCVIEW_BANDWIDTH_FILE			= "ARCVIEW_BANDWIDTH_FILE";
char * ArcPlan::ARCVIEW_TIME_CONTOUR			= "ARCVIEW_TIME_CONTOUR";
char * ArcPlan::ARCVIEW_DISTANCE_CONTOUR		= "ARCVIEW_DISTANCE_CONTOUR";
char * ArcPlan::ARCVIEW_ACCESSIBILITY_FILE		= "ARCVIEW_ACCESSIBILITY_FILE";
char * ArcPlan::ARCVIEW_RIDERSHIP_FILE			= "ARCVIEW_RIDERSHIP_FILE";
char * ArcPlan::ARCVIEW_STOP_DEMAND_FILE		= "ARCVIEW_STOP_DEMAND_FILE";
char * ArcPlan::ARCVIEW_STOP_GROUP_FILE			= "ARCVIEW_STOP_GROUP_FILE";
char * ArcPlan::ARCVIEW_PARKING_DEMAND_FILE		= "ARCVIEW_PARKING_DEMAND_FILE";
char * ArcPlan::TIME_OF_DAY_FORMAT				= "TIME_OF_DAY_FORMAT";
char * ArcPlan::SELECT_TRAVELERS				= "SELECT_TRAVELERS";
char * ArcPlan::SELECT_NODES_x					= "SELECT_NODES_*";
char * ArcPlan::SELECT_LINKS_x					= "SELECT_LINKS_*";
char * ArcPlan::SELECT_TIME_PERIODS				= "SELECT_TIME_PERIODS";
char * ArcPlan::SELECT_LOCATIONS				= "SELECT_LOCATIONS";
char * ArcPlan::SELECT_PARKING_LOTS				= "SELECT_PARKING_LOTS";
char * ArcPlan::SELECT_TRANSIT_STOPS			= "SELECT_TRANSIT_STOPS";
char * ArcPlan::SELECT_TRANSIT_ROUTES			= "SELECT_TRANSIT_ROUTES";
char * ArcPlan::SELECT_TRANSIT_MODES			= "SELECT_TRANSIT_MODES";
char * ArcPlan::SELECT_PROBLEM_TYPES			= "SELECT_PROBLEM_TYPES";
char * ArcPlan::SELECT_RANDOM_PERCENTAGE		= "SELECT_RANDOM_PERCENTAGE";
char * ArcPlan::RANDOM_NUMBER_SEED				= "RANDOM_NUMBER_SEED";
char * ArcPlan::LINK_DIRECTION_OFFSET			= "LINK_DIRECTION_OFFSET";
char * ArcPlan::PARKING_SIDE_OFFSET				= "PARKING_SIDE_OFFSET";
char * ArcPlan::ACTIVITY_LOCATION_SIDE_OFFSET	= "ACTIVITY_LOCATION_SIDE_OFFSET";
char * ArcPlan::TRANSIT_STOP_SIDE_OFFSET		= "TRANSIT_STOP_SIDE_OFFSET";
char * ArcPlan::TRANSIT_DIRECTION_OFFSET		= "TRANSIT_DIRECTION_OFFSET";
char * ArcPlan::BANDWIDTH_SCALING_FACTOR		= "BANDWIDTH_SCALING_FACTOR";
char * ArcPlan::MINIMUM_BANDWIDTH_VALUE			= "MINIMUM_BANDWIDTH_VALUE";
char * ArcPlan::MINIMUM_BANDWIDTH_SIZE			= "MINIMUM_BANDWIDTH_SIZE";
char * ArcPlan::MAXIMUM_BANDWIDTH_SIZE			= "MAXIMUM_BANDWIDTH_SIZE";
char * ArcPlan::MAXIMUM_SHAPE_ANGLE				= "MAXIMUM_SHAPE_ANGLE";
char * ArcPlan::MINIMUM_SHAPE_LENGTH			= "MINIMUM_SHAPE_LENGTH";
char * ArcPlan::CONTOUR_TIME_INCREMENTS			= "CONTOUR_TIME_INCREMENTS";
char * ArcPlan::CONTOUR_DISTANCE_INCREMENTS		= "CONTOUR_DISTANCE_INCREMENTS";
char * ArcPlan::RIDERSHIP_SCALING_FACTOR		= "RIDERSHIP_SCALING_FACTOR";
char * ArcPlan::MINIMUM_RIDERSHIP_VALUE			= "MINIMUM_RIDERSHIP_VALUE";
char * ArcPlan::MINIMUM_RIDERSHIP_SIZE			= "MINIMUM_RIDERSHIP_SIZE";
char * ArcPlan::MAXIMUM_RIDERSHIP_SIZE			= "MAXIMUM_RIDERSHIP_SIZE";
char * ArcPlan::STOP_EQUIVALENCE_FILE			= "STOP_EQUIVALENCE_FILE";

//---------------------------------------------------------
//	ArcPlan constructor
//---------------------------------------------------------

ArcPlan::ArcPlan (void) : Demand_Service ()
{
	Program ("ArcPlan");
	Version ("4.0.21");
	Title ("Plan File Display Utility");

	Network_File required_network [] = {
		NODE, LINK, ACTIVITY_LOCATION, PARKING, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, SHAPE, LANE_CONNECTIVITY,
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_DRIVER, END_NETWORK
	};

	Demand_File optional_demand [] = {
		LINK_DELAY, END_DEMAND
	};

	char *keys [] = {
		PLAN_FILE,
		PLAN_FORMAT,
		PROBLEM_FILE,
		PROBLEM_FORMAT,
		NODE_LIST_PATHS,
		HOUSEHOLD_LIST,
		ARCVIEW_PLAN_FILE,
		ARCVIEW_PROBLEM_FILE,
		ARCVIEW_BANDWIDTH_FILE,
		ARCVIEW_TIME_CONTOUR,
		ARCVIEW_DISTANCE_CONTOUR,
		ARCVIEW_ACCESSIBILITY_FILE,
		ARCVIEW_RIDERSHIP_FILE,
		ARCVIEW_STOP_DEMAND_FILE,
		ARCVIEW_STOP_GROUP_FILE,
		ARCVIEW_PARKING_DEMAND_FILE,
		TIME_OF_DAY_FORMAT,
		SELECT_TRAVELERS,
		SELECT_NODES_x,
		SELECT_LINKS_x,
		SELECT_TIME_PERIODS,
		SELECT_LOCATIONS,
		SELECT_PARKING_LOTS,
		SELECT_TRANSIT_STOPS,
		SELECT_TRANSIT_ROUTES,
		SELECT_TRANSIT_MODES,
		SELECT_PROBLEM_TYPES,
		SELECT_RANDOM_PERCENTAGE,
		RANDOM_NUMBER_SEED,
		LINK_DIRECTION_OFFSET,
		PARKING_SIDE_OFFSET,
		ACTIVITY_LOCATION_SIDE_OFFSET,
		TRANSIT_STOP_SIDE_OFFSET,
		TRANSIT_DIRECTION_OFFSET,
		BANDWIDTH_SCALING_FACTOR,
		MINIMUM_BANDWIDTH_VALUE,
		MINIMUM_BANDWIDTH_SIZE,
		MAXIMUM_BANDWIDTH_SIZE,
		MAXIMUM_SHAPE_ANGLE,
		MINIMUM_SHAPE_LENGTH,
		CONTOUR_TIME_INCREMENTS,
		CONTOUR_DISTANCE_INCREMENTS,
		RIDERSHIP_SCALING_FACTOR,
		MINIMUM_RIDERSHIP_VALUE,
		MINIMUM_RIDERSHIP_SIZE,
		MAXIMUM_RIDERSHIP_SIZE,
		STOP_EQUIVALENCE_FILE,
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Enable_Partitions (true);

	projection.Add_Keys ();

	AB_Key_Flag (true);

	num_out = num_problem = max_problem = num_width = num_time = num_distance = num_access = 0;
	num_rider = num_stop = num_group = num_parking = 0;

	link_offset = 0.0;
	parking_side = 5.0;
	location_side = 15.0;
	stop_side = 5.0;
	route_offset = 0.0;
	width_factor = 1.0;
	min_value = 0;
	min_width = 0.01;
	max_width = 1000.0;
	max_angle = 45;
	min_length = 5;
	load_factor = 0.0;
	min_load = 0;
	min_rider = 0.01;
	max_rider = 1000.0;
	random_share = 0.1;

	type_flag = true;
	route_flag = driver_flag = stop_flag = problem_flag = problem_out = access_flag = false;
	plan_flag = hhlist_flag = width_flag = time_flag = distance_flag = turn_flag = false;
	rider_flag = load_flag = on_off_flag = demand_flag = group_flag = parking_flag = false;
	mode_flag = random_flag = false;
	
	problem_type = NULL;

	width_data = NULL;
	link_flag = NULL;
	load_data = NULL;
	board_data = NULL;
	alight_data = NULL;
	stop_list = NULL;
	parking_out = NULL;
	parking_in = NULL;
}

//---------------------------------------------------------
//	ArcPlan destructor
//---------------------------------------------------------

ArcPlan::~ArcPlan (void)
{
	Data_Range *range;

	for (range = (Data_Range *) nodes.First (); range != NULL; range = (Data_Range *) nodes.Next ()) {
		delete range;
	}
	for (range = (Data_Range *) links.First (); range != NULL; range = (Data_Range *) links.Next ()) {
		delete range;
	}
	if (problem_type != NULL) {
		delete [] problem_type;
	}
	if (width_flag && width_data != NULL) {
		delete [] width_data;
	}
	if (time_flag || distance_flag) {
		if (link_flag != NULL) {
			delete [] link_flag;
		}
	}
	if (rider_flag) {
		if (load_data != NULL) {
			delete [] load_data;
		}
		if (stop_list != NULL) {
			delete [] stop_list;
		}
	}
	if (rider_flag || on_off_flag) {
		if (board_data != NULL) {
			delete [] board_data;
		}
		if (alight_data != NULL) {
			delete [] alight_data;
		}
	}
	if (parking_flag) {
		if (parking_out != NULL) {
			delete [] parking_out;
		}
		if (parking_in != NULL) {
			delete [] parking_in;
		}
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ArcPlan *exe = new ArcPlan ();

	return (exe->Start_Execution (commands, control));
}

