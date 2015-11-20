//*********************************************************
//	IntControl.cpp - Intersection Control Utility
//*********************************************************

#include "IntControl.hpp"

char * IntControl::INPUT_SIGN_FILE			= "INPUT_SIGN_FILE";
char * IntControl::INPUT_SIGNAL_FILE		= "INPUT_SIGNAL_FILE";
char * IntControl::DELETE_NODE_CONTROL_FILE	= "DELETE_NODE_CONTROL_FILE";
char * IntControl::ADD_NO_CONTROL_RECORDS	= "ADD_NO_CONTROL_RECORDS";
char * IntControl::PRINT_SIGN_WARNINGS		= "PRINT_SIGN_WARNINGS";
char * IntControl::PRINT_MERGE_WARNINGS		= "PRINT_MERGE_WARNINGS";
char * IntControl::SIGNAL_CYCLE_LENGTH		= "SIGNAL_CYCLE_LENGTH";
char * IntControl::MINIMUM_PHASE_TIME		= "MINIMUM_PHASE_TIME";
char * IntControl::YELLOW_PHASE_TIME		= "YELLOW_PHASE_TIME";
char * IntControl::RED_CLEAR_PHASE_TIME		= "RED_CLEAR_PHASE_TIME";
char * IntControl::SIGNAL_DETECTOR_LENGTH	= "SIGNAL_DETECTOR_LENGTH";	
char * IntControl::POCKET_LANE_FACTOR		= "POCKET_LANE_FACTOR";
char * IntControl::GENERAL_GREEN_FACTOR		= "GENERAL_GREEN_FACTOR";
char * IntControl::EXTENDED_GREEN_FACTOR	= "EXTENDED_GREEN_FACTOR";
char * IntControl::SIGNAL_SPLIT_METHOD		= "SIGNAL_SPLIT_METHOD";
char * IntControl::MINIMUM_LANE_CAPACITY	= "MINIMUM_LANE_CAPACITY";
char * IntControl::MAXIMUM_LANE_CAPACITY	= "MAXIMUM_LANE_CAPACITY";
char * IntControl::TIME_OF_DAY_FORMAT		= "TIME_OF_DAY_FORMAT";
char * IntControl::TIME_PERIOD_BREAKS		= "TIME_PERIOD_BREAKS";
char * IntControl::TIME_PERIOD_RANGE		= "TIME_PERIOD_RANGE";
char * IntControl::TURN_MOVEMENT_FILE		= "TURN_MOVEMENT_FILE";
char * IntControl::LINK_DELAY_FILE			= "LINK_DELAY_FILE";

//---------------------------------------------------------
//	IntControl constructor
//---------------------------------------------------------

IntControl::IntControl (void) : Network_Service ()
{
	Program ("IntControl");
	Version ("4.0.19");
	Title ("Intersection Traffic Controls");

	Network_File required_network [] = {
		NODE, LINK, POCKET_LANE, LANE_CONNECTIVITY, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, SHAPE, LANE_USE, UNSIGNALIZED_NODE, 
		SIGNALIZED_NODE, TIMING_PLAN, PHASING_PLAN, DETECTOR, SIGNAL_COORDINATOR,
		NEW_DIRECTORY, NEW_UNSIGNALIZED_NODE, NEW_SIGNALIZED_NODE, 
		NEW_TIMING_PLAN, NEW_PHASING_PLAN, NEW_DETECTOR, NEW_SIGNAL_COORDINATOR, END_NETWORK
	};

	char *keys [] = {
		INPUT_SIGN_FILE,
		INPUT_SIGNAL_FILE,
		DELETE_NODE_CONTROL_FILE,
		ADD_NO_CONTROL_RECORDS,
		PRINT_SIGN_WARNINGS,
		PRINT_MERGE_WARNINGS,
		SIGNAL_CYCLE_LENGTH,
		MINIMUM_PHASE_TIME,
		YELLOW_PHASE_TIME,
		RED_CLEAR_PHASE_TIME,
		SIGNAL_DETECTOR_LENGTH,
		POCKET_LANE_FACTOR,
		GENERAL_GREEN_FACTOR,
		EXTENDED_GREEN_FACTOR,
		SIGNAL_SPLIT_METHOD,
		MINIMUM_LANE_CAPACITY,
		MAXIMUM_LANE_CAPACITY,
		TIME_OF_DAY_FORMAT,
		TIME_PERIOD_BREAKS,
		TIME_PERIOD_RANGE,
		TURN_MOVEMENT_FILE,
		LINK_DELAY_FILE,
		NULL
	};
	char *reports [] = {
		"SIGNAL_TIMING_UPDATES",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);

	Key_List (keys);
	Report_List (reports);

	method = 1;
	cycle_len = 60;
	yellow = 3;
	red_clear = 1;
	min_phase = 5;
	min_cap = 500;
	max_cap = 1500;

	pocket_factor = 0.5;
	general_factor = 1.0;
	detector_length = Round (20.0);		//---- 20 meters ----
	extend_factor = 0.6;

	delay_flag = range_flag = no_control_flag = signal_flag = sign_flag = delete_flag = update_flag = false;
	detector_flag = coord_flag = update_report = regen_flag = false;
	merge_flag = warning_flag = input_sign_flag = input_signal_flag = time_flag = turn_flag = false;
	nsign = nsignal = ntiming = nphasing = ndetector = ncoord = 0;
	max_timing = max_detector = max_coord = 0;
	sign_index = signal_index = phasing_index = 0;

	time_range.Format ("24_HOUR_CLOCK");

	node_array = (Node_Array *) new Node_List_Array ();
	dir_array = (Dir_Array *) new Dir_Connect_Array ();
	connect_array = (Connect_Array *) new Connect_Time_Array ();
}

//---------------------------------------------------------
//	IntControl destructor
//---------------------------------------------------------

IntControl::~IntControl (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	IntControl *exe = new IntControl ();

	return (exe->Start_Execution (commands, control));
}

