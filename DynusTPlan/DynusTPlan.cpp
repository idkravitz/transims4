//*********************************************************
//	DynusTPlan.cpp - Convert Plans to DynusT Format
//*********************************************************

#include "DynusTPlan.hpp"

char * DynusTPlan::PLAN_FILE				= "PLAN_FILE";
char * DynusTPlan::PLAN_FORMAT				= "PLAN_FORMAT";
char * DynusTPlan::NODE_LIST_PATHS			= "NODE_LIST_PATHS";
char * DynusTPlan::SELECT_TIME_PERIOD		= "SELECT_TIME_PERIOD";
char * DynusTPlan::OUTPUT_TIME_INCREMENT	= "OUTPUT_TIME_INCREMENT";
char * DynusTPlan::SELECTION_PERCENTAGE     = "SELECTION_PERCENTAGE";
char * DynusTPlan::RANDOM_NUMBER_SEED		= "RANDOM_NUMBER_SEED";
char * DynusTPlan::DYNUST_PLAN_DIRECTORY	= "DYNUST_PLAN_DIRECTORY";
char * DynusTPlan::DYNUST_USER_CLASS		= "DYNUST_USER_CLASS";

//---------------------------------------------------------
//	DynusTPlan constructor
//---------------------------------------------------------

DynusTPlan::DynusTPlan (void) : Demand_Service ()
{
	Program ("DynusTPlan");
	Version ("4.0.4");
	Title ("Convert Plans to DynusT Format");

	Network_File required_network [] = {
		NODE, LINK, ZONE, PARKING, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, END_NETWORK
	};

	Demand_File required_demand [] = {
		END_DEMAND
	};

	Demand_File optional_demand [] = {
		VEHICLE, VEHICLE_TYPE, END_DEMAND
	};

	char *keys [] = {
		PLAN_FILE,
		PLAN_FORMAT,
		NODE_LIST_PATHS,
		SELECT_TIME_PERIOD,
		OUTPUT_TIME_INCREMENT,
		SELECTION_PERCENTAGE,
		RANDOM_NUMBER_SEED,
		DYNUST_PLAN_DIRECTORY,
		DYNUST_USER_CLASS,
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
	user_class = 1;

	percent = 100.0;
}

//---------------------------------------------------------
//	DynusTPlan destructor
//---------------------------------------------------------

DynusTPlan::~DynusTPlan (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	DynusTPlan *exe = new DynusTPlan ();

	return (exe->Start_Execution (commands, control));
}


