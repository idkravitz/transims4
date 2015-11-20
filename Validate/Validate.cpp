//*********************************************************
//	Validate.cpp - Network Validation Utility
//*********************************************************

#include "Validate.hpp"

char * Validate::INPUT_VOLUME_FILE			= "INPUT_VOLUME_FILE";
char * Validate::TURN_VOLUME_FILE			= "TURN_VOLUME_FILE";
char * Validate::TRAFFIC_COUNT_FILE			= "TRAFFIC_COUNT_FILE";
char * Validate::TURN_COUNT_FILE			= "TURN_COUNT_FILE";
char * Validate::LINE_GROUP_COUNT_FILE		= "LINE_GROUP_COUNT_FILE";
char * Validate::STOP_GROUP_COUNT_FILE		= "STOP_GROUP_COUNT_FILE";
char * Validate::OUTPUT_VOLUME_FILE			= "OUTPUT_VOLUME_FILE";
char * Validate::SUMMARY_TIME_PERIODS		= "SUMMARY_TIME_PERIODS";
char * Validate::SUMMARY_TIME_INCREMENT		= "SUMMARY_TIME_INCREMENT";
char * Validate::ANALYSIS_METHOD			= "ANALYSIS_METHOD";
char * Validate::ADJUSTMENT_FACTOR			= "ADJUSTMENT_FACTOR";
char * Validate::LINK_EQUIVALENCE_FILE		= "LINK_EQUIVALENCE_FILE";
char * Validate::ZONE_EQUIVALENCE_FILE		= "ZONE_EQUIVALENCE_FILE";
char * Validate::LINE_EQUIVALENCE_FILE		= "LINE_EQUIVALENCE_FILE";
char * Validate::STOP_EQUIVALENCE_FILE		= "STOP_EQUIVALENCE_FILE";
char * Validate::FACILITY_TYPE_LABELS		= "FACILITY_TYPE_LABELS";
char * Validate::AREA_TYPE_LABELS			= "AREA_TYPE_LABELS";
char * Validate::SELECT_FACILITY_TYPES		= "SELECT_FACILITY_TYPES";

int Validate::volume_level [] = {
	1000, 2500, 5000, 7500, 10000, 25000, 50000, 75000, 100000, 500000, 0
};

//---------------------------------------------------------
//	Validate constructor
//---------------------------------------------------------

Validate::Validate (void) : Demand_Service ()
{
	Program ("Validate");
	Version ("4.0.18");
	Title ("Compare Volumes to Counts");

	Network_File required_network [] = {
		LINK, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, NODE, ZONE, POCKET_LANE, LANE_CONNECTIVITY, 
		SIGNALIZED_NODE, TIMING_PLAN, PHASING_PLAN, 
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER,
		END_NETWORK
	};

	Demand_File optional_demand [] = {
		LINK_DELAY, RIDERSHIP, END_DEMAND
	};

	char *keys [] = {
		INPUT_VOLUME_FILE,
		TRAFFIC_COUNT_FILE,
		TURN_VOLUME_FILE,
		TURN_COUNT_FILE,
		LINE_GROUP_COUNT_FILE,
		STOP_GROUP_COUNT_FILE,
		OUTPUT_VOLUME_FILE,
		SUMMARY_TIME_PERIODS,
		SUMMARY_TIME_INCREMENT,
		ANALYSIS_METHOD,
		ADJUSTMENT_FACTOR,
		LINK_EQUIVALENCE_FILE,
		ZONE_EQUIVALENCE_FILE,
		LINE_EQUIVALENCE_FILE,
		STOP_EQUIVALENCE_FILE,
		FACILITY_TYPE_LABELS,
		AREA_TYPE_LABELS,
		SELECT_FACILITY_TYPES,
		NULL
	};

	char *reports [] = {
		"VOLUME_LEVEL_SUMMARY",
		"FACILITY_TYPE_SUMMARY",
		"AREA_TYPE_SUMMARY",
		"ZONE_GROUP_SUMMARY",
		"LINK_GROUP_SUMMARY",
		"LINK_GROUP_DETAILS",
		"TURN_MOVEMENT_SUMMARY",
		"TURN_LEVEL_OF_SERVICE",
		"PRINT_ZONE_EQUIVALENCIES",
		"PRINT_LINK_EQUIVALENCIES",
		"LINE_GROUP_SUMMARY",
		"STOP_GROUP_SUMMARY",
		"BOARDING_GROUP_SUMMARY",
		"ALIGHTING_GROUP_SUMMARY",
		"PRINT_LINE_EQUIVALENCIES",
		"PRINT_STOP_EQUIVALENCIES",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);

	Page_Size (120, 47);

	delay_flag = link_flag = turn_flag = output_flag = zone_flag = method = line_flag = stop_flag = false;
	select_flag = false;
	factor = 1.0;
	hours = 1.0;
	street_name = NULL;

	compass.Set_Points (8);

	group_data.Data_Size (sizeof (Group_Data));
	area_type.Data_Size (sizeof (Label_Type));
	facility_type.Data_Size (sizeof (Label_Type));
}

//---------------------------------------------------------
//	Validate destructor
//---------------------------------------------------------

Validate::~Validate (void)
{
	if (street_name != NULL) {
		for (int i=0; i < link_data.Num_Records (); i++) {
			if (street_name [i] != NULL) {
				delete [] street_name [i];
			}
		}
		delete [] street_name;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	Validate *exe = new Validate ();

	return (exe->Start_Execution (commands, control));
}
