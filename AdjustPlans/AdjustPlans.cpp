//*********************************************************
//	AdjustPlans.cpp - adjust plans based on counts
//*********************************************************

#include "AdjustPlans.hpp"

char * AdjustPlans::PLAN_FILE				= "PLAN_FILE";
char * AdjustPlans::PLAN_FORMAT				= "PLAN_FORMAT";
char * AdjustPlans::NODE_LIST_PATHS			= "NODE_LIST_PATHS";
char * AdjustPlans::TRAFFIC_COUNT_FILE		= "TRAFFIC_COUNT_FILE";
char * AdjustPlans::TRAFFIC_COUNT_FORMAT	= "TRAFFIC_COUNT_FORMAT";
char * AdjustPlans::LINK_EQUIVALENCE_FILE	= "LINK_EQUIVALENCE_FILE";
char * AdjustPlans::COUNT_SCALING_FACTOR	= "COUNT_SCALING_FACTOR";
char * AdjustPlans::RANDOM_NUMBER_SEED		= "RANDOM_NUMBER_SEED";
char * AdjustPlans::MAXIMUM_PERCENT_ERROR	= "MAXIMUM_PERCENT_ERROR";
char * AdjustPlans::MAXIMUM_ITERATIONS		= "MAXIMUM_ITERATIONS";
char * AdjustPlans::MAXIMUM_REPLICATION		= "MAXIMUM_REPLICATION";
char * AdjustPlans::PROHIBIT_DELETIONS		= "PROHIBIT_DELETIONS";
char * AdjustPlans::NEW_PLAN_FILE			= "NEW_PLAN_FILE";
char * AdjustPlans::NEW_PLAN_FORMAT			= "NEW_PLAN_FORMAT";
char * AdjustPlans::NEW_HOUSEHOLD_LIST		= "NEW_HOUSEHOLD_LIST";
char * AdjustPlans::CONVERGENCE_FILE		= "CONVERGENCE_FILE";
char * AdjustPlans::CONVERGENCE_FORMAT		= "CONVERGENCE_FORMAT";

//---------------------------------------------------------
//	AdjustPlans constructor
//---------------------------------------------------------

AdjustPlans::AdjustPlans (void) : Demand_Service ()
{
	Program ("AdjustPlans");
	Version ("4.0.9");
	Title ("Adjust Plans based on Counts");

	Network_File required_network [] = {
		NODE, LINK, PARKING, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, LANE_CONNECTIVITY, END_NETWORK
	};

	Demand_File required_demand [] = {
		END_DEMAND
	};

	Demand_File optional_demand [] = {
		LINK_DELAY, VEHICLE, NEW_VEHICLE, END_DEMAND
	};

	char *keys [] = {
		PLAN_FILE,
		PLAN_FORMAT,
		NODE_LIST_PATHS,
		TRAFFIC_COUNT_FILE,
		TRAFFIC_COUNT_FORMAT,
		LINK_EQUIVALENCE_FILE,
		COUNT_SCALING_FACTOR,
		RANDOM_NUMBER_SEED,
		MAXIMUM_PERCENT_ERROR,
		MAXIMUM_ITERATIONS,
		MAXIMUM_REPLICATION,
		PROHIBIT_DELETIONS,
		NEW_PLAN_FILE,
		NEW_PLAN_FORMAT,
		NEW_HOUSEHOLD_LIST,
		CONVERGENCE_FILE,
		CONVERGENCE_FORMAT,
		NULL
	};
	
	char *reports [] = {
		"PRINT_LINK_EQUIVALENCIES",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	Check_Data (true);
	Renumber (true);
	Sort_Vehicles (true);

	AB_Key_Flag  (true);

	type_flag = true;
	turn_flag = false;
	delay_flag = false;
	output_flag = false;
	converge_flag = false;
	hhold_flag = false;
	equiv_flag = false;
	delete_flag = false;
	
	max_error = 5.0;		//---- 5% ----
	factor = 1.0;
	max_iter = 25;
	max_copy = 25;
	num_deleted = num_replicated = num_added = max_traveler = max_vehicle = max_household = 0;
}

//---------------------------------------------------------
//	AdjustPlans destructor
//---------------------------------------------------------

AdjustPlans::~AdjustPlans (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	AdjustPlans *exe = new AdjustPlans ();

	return (exe->Start_Execution (commands, control));
}


