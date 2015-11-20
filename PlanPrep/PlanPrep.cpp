//*********************************************************
//	PlanPrep.cpp - Manipulate Plan Files
//*********************************************************

#include "PlanPrep.hpp"

char * PlanPrep::INPUT_PLAN_FILE			= "INPUT_PLAN_FILE";
char * PlanPrep::INPUT_PLAN_FORMAT			= "INPUT_PLAN_FORMAT";
char * PlanPrep::INPUT_PLAN_SORT			= "INPUT_PLAN_SORT";
char * PlanPrep::MERGE_PLAN_FILE			= "MERGE_PLAN_FILE";
char * PlanPrep::MERGE_PLAN_FORMAT			= "MERGE_PLAN_FORMAT";
char * PlanPrep::SUBAREA_PLAN_FILE			= "SUBAREA_PLAN_FILE";
char * PlanPrep::SUBAREA_PLAN_FORMAT		= "SUBAREA_PLAN_FORMAT";
char * PlanPrep::OUTPUT_PLAN_FILE			= "OUTPUT_PLAN_FILE";
char * PlanPrep::OUTPUT_PLAN_FORMAT			= "OUTPUT_PLAN_FORMAT";
char * PlanPrep::NODE_LIST_PATHS			= "NODE_LIST_PATHS";
char * PlanPrep::PLAN_SORT_OPTION			= "PLAN_SORT_OPTION";
char * PlanPrep::PLAN_COMBINE_OPTION		= "PLAN_COMBINE_OPTION";
char * PlanPrep::MAX_OUTPUT_PLAN_SIZE		= "MAX_OUTPUT_PLAN_SIZE";
char * PlanPrep::SELECT_TRAVELER_RANGE		= "SELECT_TRAVELER_RANGE";
char * PlanPrep::SELECT_TIME_PERIODS		= "SELECT_TIME_PERIODS";
char * PlanPrep::SELECTION_PERCENTAGE		= "SELECTION_PERCENTAGE";
char * PlanPrep::RANDOM_NUMBER_SEED			= "RANDOM_NUMBER_SEED";
char * PlanPrep::CHECK_PLAN_PARTITIONS		= "CHECK_PLAN_PARTITIONS";
char * PlanPrep::UPDATE_PLAN_PARTITIONS		= "UPDATE_PLAN_PARTITIONS";
char * PlanPrep::HOUSEHOLD_LIST				= "HOUSEHOLD_LIST";
char * PlanPrep::DELETE_HOUSEHOLD_LIST		= "DELETE_HOUSEHOLD_LIST";
char * PlanPrep::DELETE_ONE_LEG_PLANS		= "DELETE_ONE_LEG_PLANS";
char * PlanPrep::FIX_PLAN_FILE				= "FIX_PLAN_FILE";
char * PlanPrep::MERGE_BY_TRIP				= "MERGE_BY_TRIP";

int PlanPrep::percent_break [NUM_PERCENTILES] = {50, 65, 70, 75, 80, 85, 90, 95, 99};

//---------------------------------------------------------
//	PlanPrep constructor
//---------------------------------------------------------

PlanPrep::PlanPrep (void) : Demand_Service ()
{
	Program ("PlanPrep");
	Version ("4.0.16");
	Title ("Manipulate Plan Files");

	char *keys [] = {
		INPUT_PLAN_FILE,
		INPUT_PLAN_FORMAT,
		INPUT_PLAN_SORT,
		MERGE_PLAN_FILE,
		MERGE_PLAN_FORMAT,
		SUBAREA_PLAN_FILE,
		SUBAREA_PLAN_FORMAT,
		OUTPUT_PLAN_FILE,
		OUTPUT_PLAN_FORMAT,
		NODE_LIST_PATHS,
		PLAN_SORT_OPTION,
		PLAN_COMBINE_OPTION,
		MAX_OUTPUT_PLAN_SIZE,
		SELECT_TRAVELER_RANGE,
		SELECT_TIME_PERIODS,
		SELECTION_PERCENTAGE,
		RANDOM_NUMBER_SEED,
		CHECK_PLAN_PARTITIONS,
		UPDATE_PLAN_PARTITIONS,
		HOUSEHOLD_LIST,
		DELETE_HOUSEHOLD_LIST,
		DELETE_ONE_LEG_PLANS,
		FIX_PLAN_FILE,
		MERGE_BY_TRIP,
		NULL
	};

	char *reports [] = {
		"PERCENT_PATH_CHANGE",
		"PERCENT_TRAVEL_TIME_CHANGE",
		"DUMP_PATH_CHANGES_GREATER_THAN_*",
		"DUMP_TRAVEL_TIME_CHANGES_GREATER_THAN_*",
		"FIX_PLAN_LISTS",
		NULL
	};

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	plan_list.Initialize (sizeof (Plan_Data *), 2, true);
	fix_list.Initialize (sizeof (Fix_List), 10);

	merge_flag = output_flag = compare_flag = path_flag = sort_flag = combine_flag = trip_flag = false;
	input_sort_flag = select_flag = time_flag = check_flag = partition_flag = fix_flag = false;
	max_size_flag = size_flag = first_out = parts_flag = binary_flag = hhlist_flag = false;
	subarea_flag = subpart_flag = hhold_flag = delete_flag = one_leg_flag = false;
	type_flag = true;

	max_size = 0;
	nfile = nout = replaced = updated = num_one_leg = 0;
}

//---------------------------------------------------------
//	PlanPrep destructor
//---------------------------------------------------------

PlanPrep::~PlanPrep (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	PlanPrep *exe = new PlanPrep ();

	return (exe->Start_Execution (commands, control));
}
