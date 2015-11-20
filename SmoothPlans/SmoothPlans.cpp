//*********************************************************
//	SmoothPlans.cpp - adjust plans based on counts
//*********************************************************

#include "SmoothPlans.hpp"

char * SmoothPlans::PLAN_FILE				= "PLAN_FILE";
char * SmoothPlans::PLAN_FORMAT				= "PLAN_FORMAT";
char * SmoothPlans::NODE_LIST_PATHS			= "NODE_LIST_PATHS";
char * SmoothPlans::LINK_LIST				= "LINK_LIST";
char * SmoothPlans::RANDOM_NUMBER_SEED		= "RANDOM_NUMBER_SEED";
char * SmoothPlans::TIME_OF_DAY_FORMAT		= "TIME_OF_DAY_FORMAT";
char * SmoothPlans::TIME_INCREMENT			= "TIME_INCREMENT";
char * SmoothPlans::MAXIMUM_TIME_SHIFT		= "MAXIMUM_TIME_SHIFT";
char * SmoothPlans::SMOOTH_GROUP_SIZE		= "SMOOTH_GROUP_SIZE";
char * SmoothPlans::PERCENT_MOVED_FORWARD	= "PERCENT_MOVED_FORWARD";
char * SmoothPlans::PERCENT_MOVED_BACKWARD	= "PERCENT_MOVED_BACKWARD";
char * SmoothPlans::NUMBER_OF_ITERATIONS	= "NUMBER_OF_ITERATIONS";
char * SmoothPlans::NEW_PLAN_FILE			= "NEW_PLAN_FILE";
char * SmoothPlans::NEW_PLAN_FORMAT			= "NEW_PLAN_FORMAT";
char * SmoothPlans::NEW_LINK_SUMMARY_FILE	= "NEW_LINK_SUMMARY_FILE";

//---------------------------------------------------------
//	SmoothPlans constructor
//---------------------------------------------------------

SmoothPlans::SmoothPlans (void) : Demand_Service ()
{
	Program ("SmoothPlans");
	Version ("4.0.5");
	Title ("Smooth Plans by Time of Day");

	Network_File required_network [] = {
		END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, NODE, LINK, PARKING, LANE_CONNECTIVITY, END_NETWORK
	};

	Demand_File required_demand [] = {
		END_DEMAND
	};

	Demand_File optional_demand [] = {
		LINK_DELAY, END_DEMAND
	};

	char *keys [] = {
		PLAN_FILE,
		PLAN_FORMAT,
		NODE_LIST_PATHS,
		LINK_LIST,
		RANDOM_NUMBER_SEED,
		TIME_OF_DAY_FORMAT,
		TIME_INCREMENT,
		MAXIMUM_TIME_SHIFT,
		SMOOTH_GROUP_SIZE,
		PERCENT_MOVED_FORWARD,
		PERCENT_MOVED_BACKWARD,
		NUMBER_OF_ITERATIONS,
		NEW_PLAN_FILE,
		NEW_PLAN_FORMAT,
		NEW_LINK_SUMMARY_FILE,
		NULL
	};

	char *reports [] = {
		"TIME_CHANGE_SUMMARY",
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

	AB_Key_Flag  (true);

	type_flag = true;
	delay_flag = output_flag = summary_flag = turn_flag = change_flag = link_flag = false;

	new_time = 0;
	increment = 15;
	max_change = 45;
	niter = 3;
	naverage = 3;
	forward = 20.0;
	backward = 20.0;
	distribution = NULL;
}

//---------------------------------------------------------
//	SmoothPlans destructor
//---------------------------------------------------------

SmoothPlans::~SmoothPlans (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	SmoothPlans *exe = new SmoothPlans ();

	return (exe->Start_Execution (commands, control));
}


