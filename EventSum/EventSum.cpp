//*********************************************************
//	EventSum.cpp - Summarize Event File Data
//*********************************************************

#include "EventSum.hpp"

char * EventSum::EVENT_FILE					= "EVENT_FILE";
char * EventSum::EVENT_FORMAT				= "EVENT_FORMAT";
char * EventSum::HOUSEHOLD_LIST				= "HOUSEHOLD_LIST";
char * EventSum::TRAVEL_TIME_FILE			= "TRAVEL_TIME_FILE";
char * EventSum::TRAVEL_TIME_FORMAT			= "TRAVEL_TIME_FORMAT";
char * EventSum::TIME_OF_DAY_FORMAT			= "TIME_OF_DAY_FORMAT";
char * EventSum::SUMMARY_TIME_PERIODS		= "SUMMARY_TIME_PERIODS";
char * EventSum::SUMMARY_TIME_INCREMENT		= "SUMMARY_TIME_INCREMENT";
char * EventSum::SELECT_HOUSEHOLD_RANGE		= "SELECT_HOUSEHOLD_RANGE";
char * EventSum::SELECT_TRIP_MODES			= "SELECT_TRIP_MODES";
char * EventSum::SELECT_PLAN_MODES			= "SELECT_PLAN_MODES";
char * EventSum::COMBINE_EVENT_LEGS			= "COMBINE_EVENT_LEGS";
char * EventSum::NEW_HOUSEHOLD_LIST			= "NEW_HOUSEHOLD_LIST";
char * EventSum::NEW_TRAVEL_TIME_FILE		= "NEW_TRAVEL_TIME_FILE";
char * EventSum::NEW_TRAVEL_TIME_FORMAT		= "NEW_TRAVEL_TIME_FORMAT";
char * EventSum::NEW_TIME_SUMMARY_FILE		= "NEW_TIME_SUMMARY_FILE";
char * EventSum::NEW_TIME_SUMMARY_FORMAT	= "NEW_TIME_SUMMARY_FORMAT";
char * EventSum::MINIMUM_SCHEDULE_VARIANCE	= "MINIMUM_SCHEDULE_VARIANCE";
char * EventSum::NEW_DISTRIBUTION_FILE		= "NEW_DISTRIBUTION_FILE";
char * EventSum::NEW_DIFFERENCE_FILE		= "NEW_DIFFERENCE_FILE";
char * EventSum::NEW_LINK_EVENT_FILE		= "NEW_LINK_EVENT_FILE";
char * EventSum::NEW_LINK_EVENT_FORMAT		= "NEW_LINK_EVENT_FORMAT";
char * EventSum::LINK_EQUIVALENCE_FILE		= "LINK_EQUIVALENCE_FILE";
char * EventSum::PERCENT_TIME_DIFFERENCE	= "PERCENT_TIME_DIFFERENCE";
char * EventSum::MINIMUM_TIME_DIFFERENCE	= "MINIMUM_TIME_DIFFERENCE";
char * EventSum::MAXIMUM_TIME_DIFFERENCE	= "MAXIMUM_TIME_DIFFERENCE";
char * EventSum::SELECTION_PERCENTAGE		= "SELECTION_PERCENTAGE";
char * EventSum::MAXIMUM_PERCENT_SELECTED	= "MAXIMUM_PERCENT_SELECTED";
char * EventSum::PLAN_FILE					= "PLAN_FILE";
char * EventSum::PLAN_FORMAT				= "PLAN_FORMAT";
char * EventSum::NEW_PLAN_FILE				= "NEW_PLAN_FILE";
char * EventSum::NEW_PLAN_FORMAT			= "NEW_PLAN_FORMAT";

//---------------------------------------------------------
//	EventSum constructor
//---------------------------------------------------------

EventSum::EventSum (void) : Demand_Service ()
{
	Program ("EventSum");
	Version ("4.0.16");
	Title ("Summarize Event File Data");

	Network_File optional_network [] = {
		DIRECTORY, LINK, ACTIVITY_LOCATION, END_NETWORK
	};
	Demand_File optional_demand [] = {
		TRIP, ACTIVITY, END_DEMAND
	};

	char *keys [] = {
		EVENT_FILE,
		EVENT_FORMAT,
		HOUSEHOLD_LIST,
		TRAVEL_TIME_FILE,
		TRAVEL_TIME_FORMAT,
		TIME_OF_DAY_FORMAT,
		SUMMARY_TIME_PERIODS,
		SUMMARY_TIME_INCREMENT,
		SELECT_HOUSEHOLD_RANGE,
		SELECT_TRIP_MODES,
		SELECT_PLAN_MODES,
		COMBINE_EVENT_LEGS,
		NEW_HOUSEHOLD_LIST,
		NEW_TRAVEL_TIME_FILE,
		NEW_TRAVEL_TIME_FORMAT,
		NEW_TIME_SUMMARY_FILE,
		NEW_TIME_SUMMARY_FORMAT,
		MINIMUM_SCHEDULE_VARIANCE,
		NEW_DISTRIBUTION_FILE,
		NEW_DIFFERENCE_FILE,
		//NEW_LINK_EVENT_FILE,
		//NEW_LINK_EVENT_FORMAT,
		//LINK_EQUIVALENCE_FILE,
		PERCENT_TIME_DIFFERENCE,
		MINIMUM_TIME_DIFFERENCE,
		MAXIMUM_TIME_DIFFERENCE,
		SELECTION_PERCENTAGE,
		MAXIMUM_PERCENT_SELECTED,
		PLAN_FILE,
		PLAN_FORMAT,
		NEW_PLAN_FILE,
		NEW_PLAN_FORMAT,
		NULL
	};
	char *reports [] = {
		"TOP_100_TRAVEL_TIME_DIFFERENCES",
		"TOTAL_DIFFERENCE_DISTRIBUTION",
		"TIME_DIFFERENCE_DISTRIBUTIONS",
		"TIME_PERIOD_SUMMARY",
		//"TOP_100_PERIOD_EVENTS",
		//"LINK_EVENTS_GREATER_THAN_*",
		//"GROUP_EVENTS_GREATER_THAN_*",
		//"PRINT_LINK_EQUIVALENCIES",
		//"TRAVELER_SUMMARY_REPORT",
		//"EVENT_SUMMARY_REPORT",
		NULL
	};

	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	link_flag = trip_flag = travel_flag = group_flag = time_in_flag = false;
	hhlist_flag = time_file_flag = time_sum_flag = compare_flag = false;
	trip_file_flag = activity_flag = distrib_flag = time_flag = diff_file_flag = false;
	select_flag = new_hhold_flag = update_flag = combine_flag = range_flag = false;

	min_variance = 0;

	increment = 900;
	num_inc = 96;
	max_hhold = MAX_INTEGER / 100;

	max_time = 60;
	min_time = 1;

	percent_diff = 0.0;
	percent = 100.0;
	max_percent = 100.0;

	time_diff.Report_Numbers (TOTAL_DISTRIB, TTIME_DISTRIB, PERIOD_SUM);
}

//---------------------------------------------------------
//	EventSum destructor
//---------------------------------------------------------

EventSum::~EventSum (void)
{
	time_diff.Clear ();
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	EventSum *exe = new EventSum ();

	return (exe->Start_Execution (commands, control));
}
