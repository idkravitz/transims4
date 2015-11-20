//*********************************************************
//	ArcRider.cpp - Ridership File Display Utility
//*********************************************************

#include "ArcRider.hpp"

char * ArcRider::ARCVIEW_LINE_DEMAND_FILE		= "ARCVIEW_LINE_DEMAND_FILE";
char * ArcRider::ARCVIEW_LINE_GROUP_FILE		= "ARCVIEW_LINE_GROUP_FILE";
char * ArcRider::ARCVIEW_RIDERSHIP_FILE			= "ARCVIEW_RIDERSHIP_FILE";
char * ArcRider::ARCVIEW_STOP_DEMAND_FILE		= "ARCVIEW_STOP_DEMAND_FILE";
char * ArcRider::ARCVIEW_STOP_GROUP_FILE		= "ARCVIEW_STOP_GROUP_FILE";
char * ArcRider::ARCVIEW_RUN_CAPACITY_FILE		= "ARCVIEW_RUN_CAPACITY_FILE";
char * ArcRider::TIME_OF_DAY_FORMAT				= "TIME_OF_DAY_FORMAT";
char * ArcRider::SELECT_LINKS_x					= "SELECT_LINKS_*";
char * ArcRider::SELECT_TIME_PERIODS			= "SELECT_TIME_PERIODS";
char * ArcRider::SELECT_TIME_INCREMENT			= "SELECT_TIME_INCREMENT";
char * ArcRider::SELECT_TRANSIT_STOPS			= "SELECT_TRANSIT_STOPS";
char * ArcRider::SELECT_TRANSIT_ROUTES			= "SELECT_TRANSIT_ROUTES";
char * ArcRider::SELECT_TRANSIT_MODES			= "SELECT_TRANSIT_MODES";
char * ArcRider::TRANSIT_STOP_SIDE_OFFSET		= "TRANSIT_STOP_SIDE_OFFSET";
char * ArcRider::TRANSIT_DIRECTION_OFFSET		= "TRANSIT_DIRECTION_OFFSET";
char * ArcRider::MAXIMUM_SHAPE_ANGLE			= "MAXIMUM_SHAPE_ANGLE";
char * ArcRider::MINIMUM_SHAPE_LENGTH			= "MINIMUM_SHAPE_LENGTH";
char * ArcRider::RIDERSHIP_SCALING_FACTOR		= "RIDERSHIP_SCALING_FACTOR";
char * ArcRider::MINIMUM_RIDERSHIP_VALUE		= "MINIMUM_RIDERSHIP_VALUE";
char * ArcRider::MINIMUM_RIDERSHIP_SIZE			= "MINIMUM_RIDERSHIP_SIZE";
char * ArcRider::MAXIMUM_RIDERSHIP_SIZE			= "MAXIMUM_RIDERSHIP_SIZE";
char * ArcRider::SET_WIDTH_USING_RUNS			= "SET_WIDTH_USING_RUNS";
char * ArcRider::SET_WIDTH_USING_LOAD_FACTOR	= "SET_WIDTH_USING_LOAD_FACTOR";
char * ArcRider::STOP_EQUIVALENCE_FILE			= "STOP_EQUIVALENCE_FILE";
char * ArcRider::LINE_EQUIVALENCE_FILE			= "LINE_EQUIVALENCE_FILE";

//---------------------------------------------------------
//	ArcRider constructor
//---------------------------------------------------------

ArcRider::ArcRider (void) : Demand_Service ()
{
	Program ("ArcRider");
	Version ("4.0.6");
	Title ("Ridership File Display Utility");

	Network_File required_network [] = {
		NODE, LINK, TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, SHAPE, END_NETWORK
	};

	Demand_File required_demand [] = {
		RIDERSHIP, VEHICLE_TYPE, END_DEMAND
	};

	char *keys [] = {
		ARCVIEW_LINE_DEMAND_FILE,
		ARCVIEW_LINE_GROUP_FILE,
		ARCVIEW_RIDERSHIP_FILE,
		ARCVIEW_STOP_DEMAND_FILE,
		ARCVIEW_STOP_GROUP_FILE,
		ARCVIEW_RUN_CAPACITY_FILE,
		TIME_OF_DAY_FORMAT,
		SELECT_LINKS_x,
		SELECT_TIME_PERIODS,
		SELECT_TIME_INCREMENT,
		SELECT_TRANSIT_STOPS,
		SELECT_TRANSIT_ROUTES,
		SELECT_TRANSIT_MODES,
		TRANSIT_STOP_SIDE_OFFSET,
		TRANSIT_DIRECTION_OFFSET,
		MAXIMUM_SHAPE_ANGLE,
		MINIMUM_SHAPE_LENGTH,
		RIDERSHIP_SCALING_FACTOR,
		MINIMUM_RIDERSHIP_VALUE,
		MINIMUM_RIDERSHIP_SIZE,
		MAXIMUM_RIDERSHIP_SIZE,
		SET_WIDTH_USING_RUNS,
		SET_WIDTH_USING_LOAD_FACTOR,
		STOP_EQUIVALENCE_FILE,
		LINE_EQUIVALENCE_FILE,
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);

	Key_List (keys);
	Notes_Name_Flag (true);

	projection.Add_Keys ();

	num_line = num_sum = num_rider = num_stop = num_group = num_cap = 0;

	stop_side = 5.0;
	route_offset = 0.0;
	max_angle = 45;
	min_length = 5;
	load_factor = 0.0;
	min_load = 1;
	min_rider = 0.01;
	max_rider = 1000.0;

	type_flag = true;
	rider_flag = load_flag = on_off_flag = demand_flag = group_flag = false;
	line_flag = sum_flag = mode_flag = runs_flag = factor_flag = false;

	load_data = NULL;
	board_data = NULL;
	alight_data = NULL;
	stop_list = NULL;
	enter_data = NULL;
	start_data = NULL;
	end_data = NULL;

	line_array = (Line_Array *) new Rider_Array ();
}

//---------------------------------------------------------
//	ArcRider destructor
//---------------------------------------------------------

ArcRider::~ArcRider (void)
{
	Data_Range *range;

	for (range = (Data_Range *) links.First (); range != NULL; range = (Data_Range *) links.Next ()) {
		delete range;
	}

	if (rider_flag) {
		if (load_data != NULL) {
			delete [] load_data;
		}
		if (stop_list != NULL) {
			delete [] stop_list;
		}
		if (enter_data != NULL) {
			delete [] enter_data;
		}
	}
	if (rider_flag || on_off_flag) {
		if (board_data != NULL) {
			delete [] board_data;
		}
		if (alight_data != NULL) {
			delete [] alight_data;
		}
		if (start_data != NULL) {
			delete [] start_data;
		}
		if (end_data != NULL) {
			delete [] end_data;
		}
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ArcRider *exe = new ArcRider ();

	return (exe->Start_Execution (commands, control));
}

