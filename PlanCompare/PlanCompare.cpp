//*********************************************************
//	PlanCompare.cpp - Compare Two Plan Files
//*********************************************************

#include "PlanCompare.hpp"

char * PlanCompare::INPUT_PLAN_FILE					= "INPUT_PLAN_FILE";
char * PlanCompare::INPUT_PLAN_FORMAT				= "INPUT_PLAN_FORMAT";
char * PlanCompare::INPUT_TRIP_FILE					= "INPUT_TRIP_FILE";
char * PlanCompare::COMPARE_PLAN_FILE				= "COMPARE_PLAN_FILE";
char * PlanCompare::COMPARE_PLAN_FORMAT				= "COMPARE_PLAN_FORMAT";
char * PlanCompare::COMPARE_LOCATION_FILE			= "COMPARE_LOCATION_FILE";
char * PlanCompare::COMPARE_TRIP_FILE				= "COMPARE_TRIP_FILE";
char * PlanCompare::OUTPUT_PLAN_FILE				= "OUTPUT_PLAN_FILE";
char * PlanCompare::OUTPUT_PLAN_FORMAT				= "OUTPUT_PLAN_FORMAT";
char * PlanCompare::NODE_LIST_PATHS					= "NODE_LIST_PATHS";
char * PlanCompare::HOUSEHOLD_LIST					= "HOUSEHOLD_LIST";
char * PlanCompare::SELECT_TRAVELERS				= "SELECT_TRAVELERS";
char * PlanCompare::SELECT_TRIP_MODES				= "SELECT_TRIP_MODES";
char * PlanCompare::IGNORE_LOCATION_DIFFERENCES		= "IGNORE_LOCATION_DIFFERENCES";
char * PlanCompare::INDEX_BY_COMPARE_LOCATION		= "INDEX_BY_COMPARE_LOCATION";
char * PlanCompare::ONLY_COMPARE_DRIVE_LEGS			= "ONLY_COMPARE_DRIVE_LEGS";
char * PlanCompare::COMPARE_GENERALIZED_COSTS		= "COMPARE_GENERALIZED_COSTS";
char * PlanCompare::PERCENT_TIME_DIFFERENCE			= "PERCENT_TIME_DIFFERENCE";
char * PlanCompare::MINIMUM_TIME_DIFFERENCE			= "MINIMUM_TIME_DIFFERENCE";
char * PlanCompare::MAXIMUM_TIME_DIFFERENCE			= "MAXIMUM_TIME_DIFFERENCE";
char * PlanCompare::PERCENT_PATH_DIFFERENCE			= "PERCENT_PATH_DIFFERENCE";
char * PlanCompare::MINIMUM_PATH_DIFFERENCE			= "MINIMUM_PATH_DIFFERENCE";
char * PlanCompare::MAXIMUM_PATH_DIFFERENCE			= "MAXIMUM_PATH_DIFFERENCE";
char * PlanCompare::SELECTION_PERCENTAGE			= "SELECTION_PERCENTAGE";
char * PlanCompare::MAXIMUM_PERCENT_SELECTED		= "MAXIMUM_PERCENT_SELECTED";
char * PlanCompare::SELECT_BY_PERCENT_DIFFERENCE	= "SELECT_BY_PERCENT_DIFFERENCE";
char * PlanCompare::SELECT_BY_RELATIVE_GAP			= "SELECT_BY_RELATIVE_GAP";
char * PlanCompare::SELECT_TIME_PERIODS				= "SELECT_TIME_PERIODS";
char * PlanCompare::RANDOM_NUMBER_SEED				= "RANDOM_NUMBER_SEED";
char * PlanCompare::TIME_OF_DAY_FORMAT				= "TIME_OF_DAY_FORMAT";
char * PlanCompare::SUMMARY_TIME_PERIODS			= "SUMMARY_TIME_PERIODS";
char * PlanCompare::SUMMARY_TIME_INCREMENT			= "SUMMARY_TIME_INCREMENT";
char * PlanCompare::PERIOD_CONTROL_POINT			= "PERIOD_CONTROL_POINT";
char * PlanCompare::HIGHEST_MATCH_DIFFERENCE		= "HIGHEST_MATCH_DIFFERENCE";
char * PlanCompare::LOWEST_MATCH_DIFFERENCE			= "LOWEST_MATCH_DIFFERENCE";
char * PlanCompare::NEW_DISTRIBUTION_FILE			= "NEW_DISTRIBUTION_FILE";
char * PlanCompare::NEW_COST_DISTRIBUTION_FILE		= "NEW_COST_DISTRIBUTION_FILE";
char * PlanCompare::NEW_MODE_DISTRIBUTION_FILE		= "NEW_MODE_DISTRIBUTION_FILE";
char * PlanCompare::NEW_MODE_DISTRIBUTION_DATA_FILE = "NEW_MODE_DISTRIBUTION_DATA_FILE";
char * PlanCompare::NEW_DIFFERENCE_FILE				= "NEW_DIFFERENCE_FILE";
char * PlanCompare::NEW_START_DIFFERENCE_FILE		= "NEW_START_DIFFERENCE_FILE";
char * PlanCompare::NEW_END_DIFFERENCE_FILE			= "NEW_END_DIFFERENCE_FILE";
char * PlanCompare::NEW_TRIP_TIME_FILE				= "NEW_TRIP_TIME_FILE";
char * PlanCompare::NEW_TRAVELER_MATCH_FILE			= "NEW_TRAVELER_MATCH_FILE";
char * PlanCompare::NEW_INCOMPLETE_TRIP_FILE		= "NEW_INCOMPLETE_TRIP_FILE";
char * PlanCompare::NEW_PERIOD_DIFFERENCE_FILE		= "NEW_PERIOD_DIFFERENCE_FILE";

int PlanCompare::percent_break [NUM_PERCENTILES] = {50, 65, 70, 75, 80, 85, 90, 95, 99};

//---------------------------------------------------------
//	PlanCompare constructor
//---------------------------------------------------------

PlanCompare::PlanCompare (void) : Demand_Service ()
{
	Program ("PlanCompare");
	Version ("4.0.50");
	Title ("Compare Two Plan Files");

	Network_File optional_network [] = {
		DIRECTORY, ACTIVITY_LOCATION, END_NETWORK
	};
	char *keys [] = {
		INPUT_PLAN_FILE,
		INPUT_PLAN_FORMAT,
		INPUT_TRIP_FILE,
		COMPARE_PLAN_FILE,
		COMPARE_PLAN_FORMAT,
		COMPARE_LOCATION_FILE,
		COMPARE_TRIP_FILE,
		OUTPUT_PLAN_FILE,
		OUTPUT_PLAN_FORMAT,
		NODE_LIST_PATHS,
		HOUSEHOLD_LIST,
		SELECT_TRAVELERS,
		SELECT_TRIP_MODES,
		IGNORE_LOCATION_DIFFERENCES,
		INDEX_BY_COMPARE_LOCATION,
		ONLY_COMPARE_DRIVE_LEGS,
		COMPARE_GENERALIZED_COSTS,
		PERCENT_TIME_DIFFERENCE,
		MINIMUM_TIME_DIFFERENCE,
		MAXIMUM_TIME_DIFFERENCE,
		PERCENT_PATH_DIFFERENCE,
		MINIMUM_PATH_DIFFERENCE,
		MAXIMUM_PATH_DIFFERENCE,
		SELECTION_PERCENTAGE,
		MAXIMUM_PERCENT_SELECTED,
		SELECT_BY_PERCENT_DIFFERENCE,
		SELECT_BY_RELATIVE_GAP,
		SELECT_TIME_PERIODS,
		RANDOM_NUMBER_SEED,
		TIME_OF_DAY_FORMAT,
		SUMMARY_TIME_PERIODS,
		SUMMARY_TIME_INCREMENT,
		PERIOD_CONTROL_POINT,
		HIGHEST_MATCH_DIFFERENCE,
		LOWEST_MATCH_DIFFERENCE,
		NEW_DISTRIBUTION_FILE,
		NEW_COST_DISTRIBUTION_FILE,
		NEW_MODE_DISTRIBUTION_FILE,
		NEW_MODE_DISTRIBUTION_DATA_FILE,
		NEW_DIFFERENCE_FILE,
		NEW_START_DIFFERENCE_FILE,
		NEW_END_DIFFERENCE_FILE,
		NEW_TRIP_TIME_FILE,
		NEW_TRAVELER_MATCH_FILE,
		NEW_INCOMPLETE_TRIP_FILE,
		NEW_PERIOD_DIFFERENCE_FILE,
		NULL
	};

	char *reports [] = {
		"PERCENT_PATH_CHANGE",
		"PERCENT_TRAVEL_TIME_CHANGE",
		"PERCENT_GENERALIZED_COST_CHANGE",
		"DUMP_PATH_CHANGES_GREATER_THAN_*",
		"DUMP_TRAVEL_TIME_CHANGES_GREATER_THAN_*",
		"DUMP_COST_CHANGES_GREATER_THAN_*", 
		"TOTAL_DIFFERENCE_DISTRIBUTION",
		"TIME_DIFFERENCE_DISTRIBUTIONS",
		"TIME_PERIOD_SUMMARY",
		"GENERALIZED_COST_DISTRIBUTION",
		"COST_DIFFERENCE_DISTRIBUTIONS",
		"COST_PERIOD_SUMMARY",
		"TRIP_GAP_REPORT",
		NULL
	};
	
	Optional_Network_Files (optional_network);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	num_matched = num_complete = num_drive = num_compared = num_equal = in_only = compare_only = num_bad = period_point = 0;

	output_flag = select_flag = diff_flag = hhlist_flag = distrib_flag = time_flag = false;
	path_diff_flag = diff_file_flag = period_flag = trip_flag = ignore_flag = drive_flag = false;
	cost_flag = cost_data_flag = cost_distrib_flag = gap_flag = traveler_flag = gap_data_flag = false;
	start_flag = end_flag = loc_flag = match_out_flag = incomplete_flag = compare_loc_flag = false;
	mode_flag = mode_file_flag = drive_access_flag = mode_data_flag = period_diff_flag = false;

	select_trips.Initialize (sizeof (Select_Trips), 2);
	travel_diff.Initialize (sizeof (Travel_Diff), 3);
	start_diff.Initialize (sizeof (Location_Diff), 2);
	end_diff.Initialize (sizeof (Location_Diff), 2);
	period_diff.Initialize (sizeof (Period_Diff), 10);

	nsel = nout = 0;
	tot_select = 0.0;
	tot_diff = 0.0;
	abs_diff = 0.0;
	tot_base = 0.0;
	sig_diff = 0.0;
	sig_tot = 0.0;
	min_diff = 0;
	max_diff = 0;

	max_time = 60;
	min_time = 1;

	min_path = 0;
	max_path = 100000;

	path_diff = 0.0;
	percent = 100.0;
	max_percent = 100.0;
	percent_diff = 10.0;
	high_diff = MAX_INTEGER;
	low_diff = -MAX_INTEGER;

	time_diff.Report_Numbers (TOTAL_DISTRIB, TTIME_DISTRIB, PERIOD_SUM);
	cost_diff.Report_Numbers (TCOST_DISTRIB, COST_DISTRIB, COST_SUM);
	cost_diff.Generalized_Cost (true);

	change_text [PATH_CHANGE] = "Path";
	change_text [TIME_CHANGE] = "Travel Time";
	change_text [COST_CHANGE] = "Generalized Cost";
}

//---------------------------------------------------------
//	PlanCompare destructor
//---------------------------------------------------------

PlanCompare::~PlanCompare (void)
{
	time_diff.Clear ();
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	PlanCompare *exe = new PlanCompare ();

	return (exe->Start_Execution (commands, control));
}

