//*********************************************************
//	Progression.cpp - Signal Progression Utility
//*********************************************************

#include "Progression.hpp"

char * Progression::LINK_EQUIVALENCE_FILE	 = "LINK_EQUIVALENCE_FILE";
char * Progression::CLEAR_EXISTING_OFFSETS	 = "CLEAR_EXISTING_OFFSETS";
char * Progression::EVALUATE_EXISTING_OFFSETS= "EVALUATE_EXISTING_OFFSETS";
char * Progression::PROGRESSION_TIME_PERIODS = "PROGRESSION_TIME_PERIODS";
char * Progression::PROGRESSION_PERIOD_SPEED = "PROGRESSION_PERIOD_SPEED";
char * Progression::OPTIMIZATION_METHOD		 = "OPTIMIZATION_METHOD";
char * Progression::GROUP_PERIOD_WEIGHT_FILE = "GROUP_PERIOD_WEIGHT_FILE";
char * Progression::KEEP_LINK_GROUP_ORDER	 = "KEEP_LINK_GROUP_ORDER";
char * Progression::ARCVIEW_PROGRESSION_FILE = "ARCVIEW_PROGRESSION_FILE";
char * Progression::LINK_DIRECTION_OFFSET	 = "LINK_DIRECTION_OFFSET";

//---------------------------------------------------------
//	Progression constructor
//---------------------------------------------------------

Progression::Progression (void) : Demand_Service ()
{
	Program ("Progression");
	Version ("4.0.7");
	Title ("Signal Progression Offset");

	Network_File required_network [] = {
		NODE, LINK, LANE_CONNECTIVITY, SIGNALIZED_NODE, 
		TIMING_PLAN, PHASING_PLAN, NEW_SIGNALIZED_NODE, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, SHAPE, UNSIGNALIZED_NODE, NEW_DIRECTORY, END_NETWORK
	};
	Demand_File optional_demand [] = {
		LINK_DELAY, END_DEMAND
	};

	char *keys [] = {
		LINK_EQUIVALENCE_FILE,
		CLEAR_EXISTING_OFFSETS,
		EVALUATE_EXISTING_OFFSETS,
		PROGRESSION_TIME_PERIODS,
		PROGRESSION_PERIOD_SPEED,
		OPTIMIZATION_METHOD,
		GROUP_PERIOD_WEIGHT_FILE,
		KEEP_LINK_GROUP_ORDER,
		ARCVIEW_PROGRESSION_FILE,
		LINK_DIRECTION_OFFSET,
		NULL
	};
	char *reports [] = {
		"PRINT_LINK_EQUIVALENCIES",
		"GROUP_PERIOD_WEIGHTS",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Report_List (reports);
	
	projection.Add_Keys ();

	fixed = updated = progression_time = period = mid_period = method = max_period = 0;
	progression_speed = 0.0;
	link_offset = 5.0;
	equiv_flag = clear_flag = speed_flag = period_flag = arcview_flag = delay_flag = false;
	weight_flag = order_flag = eval_flag = false;
}

//---------------------------------------------------------
//	Progression destructor
//---------------------------------------------------------

Progression::~Progression (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	Progression *exe = new Progression ();

	return (exe->Start_Execution (commands, control));
}

