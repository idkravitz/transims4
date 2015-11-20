//*********************************************************
//	VISSIMPlan.cpp - Convert Plans to VISSIM XML Format
//*********************************************************

#include "VISSIMPlan.hpp"

char * VISSIMPlan::PLAN_FILE				= "PLAN_FILE";
char * VISSIMPlan::PLAN_FORMAT				= "PLAN_FORMAT";
char * VISSIMPlan::NODE_LIST_PATHS			= "NODE_LIST_PATHS";
char * VISSIMPlan::SELECT_TIME_PERIOD		= "SELECT_TIME_PERIOD";
char * VISSIMPlan::OUTPUT_TIME_INCREMENT	= "OUTPUT_TIME_INCREMENT";
char * VISSIMPlan::SELECTION_PERCENTAGE     = "SELECTION_PERCENTAGE";
char * VISSIMPlan::RANDOM_NUMBER_SEED		= "RANDOM_NUMBER_SEED";
char * VISSIMPlan::NEW_VISSIM_XML_FILE		= "NEW_VISSIM_XML_FILE";

//---------------------------------------------------------
//	VISSIMPlan constructor
//---------------------------------------------------------

VISSIMPlan::VISSIMPlan (void) : Demand_Service ()
{
	Program ("VISSIMPlan");
	Version ("4.0.2");
	Title ("Convert Plans to VISSIM XML Format");

	Network_File required_network [] = {
		END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, NODE, LINK, PARKING, END_NETWORK
	};

	Demand_File required_demand [] = {
		END_DEMAND
	};

	Demand_File optional_demand [] = {
		END_DEMAND
	};

	char *keys [] = {
		PLAN_FILE,
		PLAN_FORMAT,
		NODE_LIST_PATHS,
		SELECT_TIME_PERIOD,
		OUTPUT_TIME_INCREMENT,
		SELECTION_PERCENTAGE,
		RANDOM_NUMBER_SEED,
		NEW_VISSIM_XML_FILE,
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	Enable_Partitions (true);

	AB_Key_Flag  (true);

	type_flag = true;
	select_flag = false;
	num_out = 0;

	percent = 100.0;
}

//---------------------------------------------------------
//	VISSIMPlan destructor
//---------------------------------------------------------

VISSIMPlan::~VISSIMPlan (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	VISSIMPlan *exe = new VISSIMPlan ();

	return (exe->Start_Execution (commands, control));
}


