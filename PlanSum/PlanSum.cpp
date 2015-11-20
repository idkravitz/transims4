//*********************************************************
//	PlanSum.cpp - Summarize Plan File Demand
//*********************************************************

#include "PlanSum.hpp"

char * PlanSum::PLAN_FILE					= "PLAN_FILE";
char * PlanSum::PLAN_FORMAT					= "PLAN_FORMAT";
char * PlanSum::NODE_LIST_PATHS				= "NODE_LIST_PATHS";
char * PlanSum::HOUSEHOLD_LIST				= "HOUSEHOLD_LIST";
char * PlanSum::HOUSEHOLD_PERSON_COUNT		= "HOUSEHOLD_PERSON_COUNT";
char * PlanSum::SUMMARY_TIME_PERIODS		= "SUMMARY_TIME_PERIODS";
char * PlanSum::SUMMARY_TIME_INCREMENT		= "SUMMARY_TIME_INCREMENT";
char * PlanSum::NEW_PLAN_FILE				= "NEW_PLAN_FILE";
char * PlanSum::NEW_LINK_VOLUME_FILE		= "NEW_LINK_VOLUME_FILE";
char * PlanSum::NEW_LINK_VOLUME_FORMAT		= "NEW_LINK_VOLUME_FORMAT";
char * PlanSum::KEEP_INPUT_TRAVEL_TIMES		= "KEEP_INPUT_TRAVEL_TIMES";
char * PlanSum::EQUATION_PARAMETERS_x		= "EQUATION_PARAMETERS_*";
char * PlanSum::LINK_EQUIVALENCE_FILE		= "LINK_EQUIVALENCE_FILE";
char * PlanSum::NEW_TRIP_TIME_FILE			= "NEW_TRIP_TIME_FILE";
char * PlanSum::NEW_ZONE_SKIM_FILE			= "NEW_ZONE_SKIM_FILE";
char * PlanSum::NEW_ZONE_SKIM_FORMAT		= "NEW_ZONE_SKIM_FORMAT";
char * PlanSum::SKIM_MODE_SELECTION			= "SKIM_MODE_SELECTION";
char * PlanSum::SKIM_TOTAL_TIME				= "SKIM_TOTAL_TIME";
char * PlanSum::SKIM_TRIP_LENGTH			= "SKIM_TRIP_LENGTH";
char * PlanSum::SKIM_TIME_CONTROL_POINT		= "SKIM_TIME_CONTROL_POINT";
char * PlanSum::NEAREST_NEIGHBOR_FACTOR		= "NEAREST_NEIGHBOR_FACTOR";
char * PlanSum::NEW_TRIP_TABLE_FILE			= "NEW_TRIP_TABLE_FILE";
char * PlanSum::NEW_TRIP_TABLE_FORMAT		= "NEW_TRIP_TABLE_FORMAT";
char * PlanSum::TRIP_MODE_SELECTION			= "TRIP_MODE_SELECTION";
char * PlanSum::ZONE_EQUIVALENCE_FILE		= "ZONE_EQUIVALENCE_FILE";
char * PlanSum::NEW_TURN_MOVEMENT_FILE		= "NEW_TURN_MOVEMENT_FILE";
char * PlanSum::NEW_TURN_MOVEMENT_FORMAT	= "NEW_TURN_MOVEMENT_FORMAT";
char * PlanSum::TURN_NODE_RANGE				= "TURN_NODE_RANGE";
char * PlanSum::STOP_EQUIVALENCE_FILE		= "STOP_EQUIVALENCE_FILE";
char * PlanSum::OUTPUT_ALL_TRANSIT_STOPS	= "OUTPUT_ALL_TRANSIT_STOPS";

//---------------------------------------------------------
//	PlanSum constructor
//---------------------------------------------------------

PlanSum::PlanSum (void) : Demand_Service ()
{
	Program ("PlanSum");
	Version ("4.0.59");
	Title ("Summarize Plan File Demand");

	Network_File optional_network [] = {
		DIRECTORY, LINK, NODE, LANE_USE, PARKING, ACTIVITY_LOCATION, PROCESS_LINK,
		LANE_CONNECTIVITY, TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, 
		TRANSIT_DRIVER, END_NETWORK
	};
	Demand_File optional_demand [] = {
		LINK_DELAY, VEHICLE_TYPE, NEW_LINK_DELAY, NEW_RIDERSHIP, END_DEMAND
	};

	char *keys [] = {
		PLAN_FILE,
		PLAN_FORMAT,
		NODE_LIST_PATHS,
		HOUSEHOLD_LIST,
		HOUSEHOLD_PERSON_COUNT,
		SUMMARY_TIME_PERIODS,
		SUMMARY_TIME_INCREMENT,
		NEW_PLAN_FILE,
		NEW_LINK_VOLUME_FILE,
		NEW_LINK_VOLUME_FORMAT,
		KEEP_INPUT_TRAVEL_TIMES,
		EQUATION_PARAMETERS_x,
		LINK_EQUIVALENCE_FILE,
		NEW_TRIP_TIME_FILE,
		NEW_ZONE_SKIM_FILE,
		NEW_ZONE_SKIM_FORMAT,
		SKIM_MODE_SELECTION,
		SKIM_TOTAL_TIME,
		SKIM_TRIP_LENGTH,
		SKIM_TIME_CONTROL_POINT,
		NEAREST_NEIGHBOR_FACTOR,
		NEW_TRIP_TABLE_FILE,
		NEW_TRIP_TABLE_FORMAT,
		TRIP_MODE_SELECTION,
		ZONE_EQUIVALENCE_FILE,
		NEW_TURN_MOVEMENT_FILE,
		NEW_TURN_MOVEMENT_FORMAT,
		TURN_NODE_RANGE,
		STOP_EQUIVALENCE_FILE,
		OUTPUT_ALL_TRANSIT_STOPS,
		NULL
	};
	char *reports [] = {
		"TOP_100_V/C_RATIOS",
		"ALL_V/C_RATIOS_GREATER_THAN_*",
		"LINK_GROUP_V/C_RATIOS_*",
		"PRINT_LINK_EQUIVALENCIES",
		"PRINT_ZONE_EQUIVALENCIES",
		"PRINT_STOP_EQUIVALENCIES",
		"TRANSIT_RIDERSHIP_SUMMARY",
		"TRANSIT_STOP_SUMMARY",
		"TRANSIT_TRANSFER_SUMMARY",
		"TRANSIT_TRANSFER_DETAILS",
		"TRANSIT_STOP_GROUP_SUMMARY",
		"TRANSIT_PASSENGER_SUMMARY",
		"TRANSIT_LINK_GROUP_SUMMARY",
		"TRIP_TIME_REPORT",
		"TRAVEL_SUMMARY_REPORT",
		NULL
	};

	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	AB_Key_Flag  (true);

	volume_flag = time_flag = skim_flag = trip_flag = zone_flag = transit_flag = passenger_flag = false;
	delay_in = delay_out = turn_flag = turn_delay = hhlist_flag = travel_flag = update_flag = false;
	time_skim_flag = xfer_flag = xfer_detail = neighbor_flag = length_flag = count_flag = stops_flag = false;
	type_flag = vc_flag = true;

	layer = 1;

	increment = 900;
	num_inc = 96;
	cap_factor = 0.25;
	neighbor_factor = 0.5;
	skim_point = 2;

	movement_data = NULL;
	node_range = NULL;
	boardings = NULL;

	max_rail = 0;
	xfer_label [0] = '\0';

	stop_array = (Stop_Array *) new Transfer_Array ();
}

//---------------------------------------------------------
//	PlanSum destructor
//---------------------------------------------------------

PlanSum::~PlanSum (void)
{
	if (movement_data != NULL) {
		for (int i=1; i <= num_inc; i++) {
			if (movement_data [i] != NULL) {
				delete movement_data [i];
			}
		}
		delete movement_data;
	}
	if (node_range != NULL) {
		delete node_range;
	}
	if ((xfer_flag || xfer_detail) && boardings != NULL) {
		int num = 1;
		if (xfer_detail) {
			num = time_periods.Num_Ranges () * 2 + 1;
		}
		for (int i=0; i < num; i++) {
			if (boardings [i] != NULL) {
				delete [] boardings [i];
			}
		}
		delete [] boardings;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	PlanSum *exe = new PlanSum ();

	return (exe->Start_Execution (commands, control));
}
