//*********************************************************
//	LineSum.cpp - Summarize Transit Line Performance Data
//*********************************************************

#include "LineSum.hpp"

char * LineSum::SELECTION_DESCRIPTION			= "SELECTION_DESCRIPTION";
char * LineSum::SUMMARY_TIME_PERIODS			= "SUMMARY_TIME_PERIODS";
char * LineSum::SUMMARY_TIME_INCREMENT			= "SUMMARY_TIME_INCREMENT";
char * LineSum::TIME_PERIOD_METHOD				= "TIME_PERIOD_METHOD";
char * LineSum::SELECT_TRANSIT_MODES			= "SELECT_TRANSIT_MODES";
char * LineSum::SELECT_TRANSIT_ROUTES			= "SELECT_TRANSIT_ROUTES";
char * LineSum::SELECT_SUBAREA_POLYGON			= "SELECT_SUBAREA_POLYGON";
char * LineSum::SELECTION_DESCRIPTION_x			= "SELECTION_DESCRIPTION_*";
char * LineSum::SUMMARY_TIME_PERIODS_x			= "SUMMARY_TIME_PERIODS_*";
char * LineSum::SUMMARY_TIME_INCREMENT_x		= "SUMMARY_TIME_INCREMENT_*";
char * LineSum::TIME_PERIOD_METHOD_x			= "TIME_PERIOD_METHOD_*";
char * LineSum::SELECT_TRANSIT_MODES_x			= "SELECT_TRANSIT_MODES_*";
char * LineSum::SELECT_TRANSIT_ROUTES_x			= "SELECT_TRANSIT_ROUTES_*";
char * LineSum::SELECT_SUBAREA_POLYGON_x		= "SELECT_SUBAREA_POLYGON_*";
char * LineSum::STOP_EQUIVALENCE_FILE			= "STOP_EQUIVALENCE_FILE";
char * LineSum::LINE_EQUIVALENCE_FILE			= "LINE_EQUIVALENCE_FILE";
char * LineSum::LINK_EQUIVALENCE_FILE			= "LINK_EQUIVALENCE_FILE";
char * LineSum::NEW_RUN_SCHEDULE_FILE			= "NEW_RUN_SCHEDULE_FILE";
char * LineSum::NEW_RUN_CAPACITY_FILE			= "NEW_RUN_CAPACITY_FILE";

//---------------------------------------------------------
//	LineSum constructor
//---------------------------------------------------------

LineSum::LineSum (void) : Demand_Service ()
{
	Program ("LineSum");
	Version ("4.0.11");
	Title ("Summarize Transit Line Performance Data");

	Network_File required_network [] = {
		LINK, NODE, TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, 
		TRANSIT_DRIVER, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, SHAPE, END_NETWORK
	};
	
	Demand_File optional_demand [] = {
		RIDERSHIP, VEHICLE_TYPE, END_DEMAND
	};

	char *keys [] = {
		SELECTION_DESCRIPTION,
		SUMMARY_TIME_PERIODS,
		SUMMARY_TIME_INCREMENT,
		TIME_PERIOD_METHOD,
		SELECT_TRANSIT_MODES,
		SELECT_TRANSIT_ROUTES,
		SELECT_SUBAREA_POLYGON,
		SELECTION_DESCRIPTION_x,
		SUMMARY_TIME_PERIODS_x,
		SUMMARY_TIME_INCREMENT_x,
		TIME_PERIOD_METHOD_x,
		SELECT_TRANSIT_MODES_x,
		SELECT_TRANSIT_ROUTES_x,
		SELECT_SUBAREA_POLYGON_x,
		STOP_EQUIVALENCE_FILE,
		LINE_EQUIVALENCE_FILE,
		LINK_EQUIVALENCE_FILE,
		NEW_RUN_SCHEDULE_FILE,
		NEW_RUN_CAPACITY_FILE,
		NULL
	};
	char *reports [] = {
		"LINE_RIDERSHIP_REPORT",
		"LINE_SUMMARY_REPORT",
		"TRANSIT_PERFORMANCE_REPORT",
		"CAPACITY_CONSTRAINED_RUNS",
		"LINE_RIDERSHIP_REPORT_*",
		"LINE_SUMMARY_REPORT_*",
		"TRANSIT_PERFORMANCE_REPORT_*",
		"CAPACITY_CONSTRAINED_RUNS_*",
		"TRANSIT_STOP_GROUP_SUMMARY",
		"TRANSIT_STOP_GROUP_DETAILS",
		"TRANSIT_STOP_GROUP_BY_STOP",
		"PRINT_STOP_EQUIVALENCIES",
		"LINE_GROUP_SUMMARY_REPORT",
		"LINE_GROUP_PERFORMANCE_REPORT",
		"PRINT_LINE_EQUIVALENCIES",
		"TRANSIT_LINK_GROUP_SUMMARY",
		"PRINT_LINK_EQUIVALENCIES",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);
	Notes_Name_Flag (true);

	offset_flag = distance_flag = run_flag = cap_flag = false;

	group_num = 0;
	set_ptr = NULL;

	line_array = (Line_Array *) new Rider_Array ();
}

//---------------------------------------------------------
//	LineSum destructor
//---------------------------------------------------------

LineSum::~LineSum (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	LineSum *exe = new LineSum ();

	return (exe->Start_Execution (commands, control));
}

