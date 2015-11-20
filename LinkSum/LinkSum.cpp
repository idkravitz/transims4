//*********************************************************
//	LinkSum.cpp - Summarize Link Performance Data
//*********************************************************

#include "LinkSum.hpp"

char * LinkSum::PREVIOUS_LINK_DELAY_FILE		= "PREVIOUS_LINK_DELAY_FILE";
char * LinkSum::PREVIOUS_LINK_DELAY_FORMAT		= "PREVIOUS_LINK_DELAY_FORMAT";
char * LinkSum::SUMMARY_TIME_PERIODS			= "SUMMARY_TIME_PERIODS";
char * LinkSum::SUMMARY_TIME_INCREMENT			= "SUMMARY_TIME_INCREMENT";
char * LinkSum::MINIMUM_LINK_VOLUME				= "MINIMUM_LINK_VOLUME";
char * LinkSum::CONGESTED_TIME_RATIO			= "CONGESTED_TIME_RATIO";
char * LinkSum::SELECT_FACILITY_TYPES			= "SELECT_FACILITY_TYPES";
char * LinkSum::SELECT_SUBAREA_POLYGON			= "SELECT_SUBAREA_POLYGON";
char * LinkSum::SELECT_BY_LINK_GROUP			= "SELECT_BY_LINK_GROUP";
char * LinkSum::LINK_EQUIVALENCE_FILE			= "LINK_EQUIVALENCE_FILE";
char * LinkSum::DEFAULT_OUTPUT_FORMAT			= "DEFAULT_OUTPUT_FORMAT";
char * LinkSum::NEW_LINK_ACTIVITY_FILE			= "NEW_LINK_ACTIVITY_FILE";
char * LinkSum::NEW_LINK_ACTIVITY_FORMAT		= "NEW_LINK_ACTIVITY_FORMAT";
char * LinkSum::ACTIVITY_LOCATION_FIELDS		= "ACTIVITY_LOCATION_FIELDS";
char * LinkSum::NEW_ZONE_TRAVEL_FILE			= "NEW_ZONE_TRAVEL_FILE";
char * LinkSum::NEW_ZONE_TRAVEL_FORMAT			= "NEW_ZONE_TRAVEL_FORMAT";
char * LinkSum::NEW_GROUP_TRAVEL_FILE			= "NEW_GROUP_TRAVEL_FILE";
char * LinkSum::NEW_GROUP_TRAVEL_FORMAT			= "NEW_GROUP_TRAVEL_FORMAT";
char * LinkSum::ZONE_EQUIVALENCE_FILE			= "ZONE_EQUIVALENCE_FILE";
char * LinkSum::NEW_LINK_DIRECTION_FILE_x		= "NEW_LINK_DIRECTION_FILE_*";
char * LinkSum::NEW_LINK_DIRECTION_FORMAT_x		= "NEW_LINK_DIRECTION_FORMAT_*";
char * LinkSum::NEW_LINK_DIRECTION_FIELD_x		= "NEW_LINK_DIRECTION_FIELD_*";
char * LinkSum::NEW_LINK_DIRECTION_INDEX_x		= "NEW_LINK_DIRECTION_INDEX_*";
char * LinkSum::NEW_LINK_DATA_FILE_x			= "NEW_LINK_DATA_FILE_*";
char * LinkSum::NEW_LINK_DATA_FORMAT_x			= "NEW_LINK_DATA_FORMAT_*";
char * LinkSum::NEW_LINK_DATA_FIELD_x			= "NEW_LINK_DATA_FIELD_*";
char * LinkSum::NEW_PERFORMANCE_DATA_FILE		= "NEW_PERFORMANCE_DATA_FILE";
char * LinkSum::NEW_PERFORMANCE_DATA_FORMAT		= "NEW_PERFORMANCE_DATA_FORMAT";
//---- the following keys are for backward comparibility only ----
char * LinkSum::NEW_LINK_VOLUME_FILE			= "NEW_LINK_VOLUME_FILE";
char * LinkSum::NEW_LINK_VOLUME_FORMAT			= "NEW_LINK_VOLUME_FORMAT";
char * LinkSum::NEW_LINK_SPEED_FILE				= "NEW_LINK_SPEED_FILE";
char * LinkSum::NEW_LINK_SPEED_FORMAT			= "NEW_LINK_SPEED_FORMAT";
char * LinkSum::NEW_LINK_TRAVEL_TIME_FILE		= "NEW_LINK_TRAVEL_TIME_FILE";
char * LinkSum::NEW_LINK_TRAVEL_TIME_FORMAT		= "NEW_LINK_TRAVEL_TIME_FORMAT";
char * LinkSum::NEW_LINK_VC_RATIO_FILE			= "NEW_LINK_VC_RATIO_FILE";
char * LinkSum::NEW_LINK_VC_RATIO_FORMAT		= "NEW_LINK_VC_RATIO_FORMAT";
char * LinkSum::NEW_LINK_TIME_RATIO_FILE		= "NEW_LINK_TIME_RATIO_FILE";
char * LinkSum::NEW_LINK_TIME_RATIO_FORMAT		= "NEW_LINK_TIME_RATIO_FORMAT";

int	LinkSum::percent_break [NUM_PERCENTILES] = {50, 65, 70, 75, 80, 85, 90, 95, 99};

//---------------------------------------------------------
//	LinkSum constructor
//---------------------------------------------------------

LinkSum::LinkSum (void) : Demand_Service ()
{
	Program ("LinkSum");
	Version ("4.0.33");
	Title ("Summarize Link Performance Data");

	Network_File required_network [] = {
		LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, NODE, LANE_USE, LANE_CONNECTIVITY, ACTIVITY_LOCATION, END_NETWORK
	};
	
	Demand_File optional_demand [] = {
		PERFORMANCE, END_DEMAND
	};

	char *keys [] = {
		PREVIOUS_LINK_DELAY_FILE,
		PREVIOUS_LINK_DELAY_FORMAT,
		SUMMARY_TIME_PERIODS,
		SUMMARY_TIME_INCREMENT,
		MINIMUM_LINK_VOLUME,
		CONGESTED_TIME_RATIO,
		SELECT_FACILITY_TYPES,
		SELECT_SUBAREA_POLYGON,
		SELECT_BY_LINK_GROUP,
		LINK_EQUIVALENCE_FILE,
		DEFAULT_OUTPUT_FORMAT,
		NEW_LINK_ACTIVITY_FILE,
		NEW_LINK_ACTIVITY_FORMAT,
		ACTIVITY_LOCATION_FIELDS,
		NEW_ZONE_TRAVEL_FILE,
		NEW_ZONE_TRAVEL_FORMAT,
		NEW_GROUP_TRAVEL_FILE,
		NEW_GROUP_TRAVEL_FORMAT,
		ZONE_EQUIVALENCE_FILE,
		NEW_LINK_DIRECTION_FILE_x,
		NEW_LINK_DIRECTION_FORMAT_x,
		NEW_LINK_DIRECTION_FIELD_x,
		NEW_LINK_DIRECTION_INDEX_x,
		NEW_LINK_DATA_FILE_x,
		NEW_LINK_DATA_FORMAT_x,
		NEW_LINK_DATA_FIELD_x,
		NEW_PERFORMANCE_DATA_FILE,
		NEW_PERFORMANCE_DATA_FORMAT,
		//---- the following keys are for backward comparibility only ----
		NEW_LINK_VOLUME_FILE,
		NEW_LINK_VOLUME_FORMAT,
		NEW_LINK_SPEED_FILE,
		NEW_LINK_SPEED_FORMAT,
		NEW_LINK_TRAVEL_TIME_FILE,
		NEW_LINK_TRAVEL_TIME_FORMAT,
		NEW_LINK_VC_RATIO_FILE,
		NEW_LINK_VC_RATIO_FORMAT,
		NEW_LINK_TIME_RATIO_FILE,
		NEW_LINK_TIME_RATIO_FORMAT,
		NULL
	};
	char *reports [] = {
		"TOP_100_LINK_VOLUMES",
		"TOP_100_LANE_VOLUMES",
		"TOP_100_PERIOD_VOLUMES",
		"TOP_100_SPEED_REDUCTIONS",
		"TOP_100_TRAVEL_TIME_RATIOS",
		"TOP_100_VOLUME_CAPACITY_RATIOS",
		"TOP_100_TRAVEL_TIME_CHANGES",
		"TOP_100_VOLUME_CHANGES",
		"LINK_VOLUME_GREATER_THAN_*",
		"GROUP_VOLUME_GREATER_THAN_*",
		"PRINT_LINK_EQUIVALENCIES",
		"PRINT_ZONE_EQUIVALENCIES",
		"TRAVEL_TIME_DISTRIBUTION",
		"VOLUME_CAPACITY_RATIOS",
		"TRAVEL_TIME_CHANGES",
		"VOLUME_CHANGES",
		"LINK_GROUP_TRAVEL_TIME",
		"NETWORK_PERFORMANCE_SUMMARY",
		"NETWORK_PERFORMANCE_DETAILS",
		"RELATIVE_GAP_REPORT",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);

	previous_flag = group_select = detail_flag = false;
	subarea_flag = select_flag = activity_flag = zone_flag = group_flag = false;

	nerror = 0;
	min_volume = 2;
	increment = 900;
	num_inc = 96;
	cap_factor = 0.25;
	congested_ratio = 3.0;

	sum_bin = NULL;
	zone_vmt = zone_vht = NULL;

	dir_group.Initialize (sizeof (Dir_Group), 10);
	data_group.Initialize (sizeof (Data_Group), 10);
	link_location.Initialize (sizeof (Link_Location), 2);
}

//---------------------------------------------------------
//	LinkSum destructor
//---------------------------------------------------------

LinkSum::~LinkSum (void)
{
	int i;

	if (sum_bin != NULL) {
		for (i=0; i <= num_inc; i++) {
			if (sum_bin [i] != NULL) {
				delete [] sum_bin [i];
			}
		}
		delete [] sum_bin;
	}
	if (zone_vmt != NULL) {
		for (i=0; i < num_inc; i++) {
			if (zone_vmt [i] != NULL) {
				delete [] zone_vmt [i];
			}
		}
		delete [] zone_vmt;
	}
	if (zone_vht != NULL) {
		for (i=0; i < num_inc; i++) {
			if (zone_vht [i] != NULL) {
				delete [] zone_vht [i];
			}
		}
		delete [] zone_vht;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	LinkSum *exe = new LinkSum ();

	return (exe->Start_Execution (commands, control));
}

