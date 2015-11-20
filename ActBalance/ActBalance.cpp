//*********************************************************
//	ActBalance.cpp - Activity Generator
//*********************************************************

#include "ActBalance.hpp"

char * ActBalance::ACTIVITY_PURPOSE_RANGE_x		= "ACTIVITY_PURPOSE_RANGE_*";
char * ActBalance::BALANCING_TARGET_FIELD_x		= "BALANCING_TARGET_FIELD_*";
char * ActBalance::BALANCING_FACTOR_FIELD_x		= "BALANCING_FACTOR_FIELD_*";
char * ActBalance::BALANCING_TARGET_FILE		= "BALANCING_TARGET_FILE";
char * ActBalance::BALANCING_TARGET_FORMAT		= "BALANCING_TARGET_FORMAT";
char * ActBalance::NEW_BALANCING_FACTOR_FILE	= "NEW_BALANCING_FACTOR_FILE";
char * ActBalance::NEW_BALANCING_FACTOR_FORMAT	= "NEW_BALANCING_FACTOR_FORMAT";
char * ActBalance::ZONE_EQUIVALENCE_FILE		= "ZONE_EQUIVALENCE_FILE";

//---------------------------------------------------------
//	ActBalance constructor
//---------------------------------------------------------

ActBalance::ActBalance (void) : Demand_Service ()
{
	Program ("ActBalance");
	Version ("4.0.0");
	Title ("Activity Balancing");

	Network_File required_network [] = {
		ACTIVITY_LOCATION, END_NETWORK
	};

	Network_File optional_network [] = {
		DIRECTORY, END_NETWORK
	};

	Demand_File required_demand [] = {
		ACTIVITY, END_DEMAND
	};

	char *keys [] = {
		ACTIVITY_PURPOSE_RANGE_x,
		BALANCING_TARGET_FIELD_x,
		BALANCING_FACTOR_FIELD_x,
		BALANCING_TARGET_FILE,
		BALANCING_TARGET_FORMAT,
		NEW_BALANCING_FACTOR_FILE,
		NEW_BALANCING_FACTOR_FORMAT,
		ZONE_EQUIVALENCE_FILE,
		NULL
	};

	char *reports [] = {
		"ZONE_FACTOR_SUMMARY",
		"DISTRICT_FACTOR_SUMMARY",
		"PRINT_ZONE_EQUIVALENCIES",
		NULL
	};

	Required_Network_Files (required_network);
	Optional_Network_Files (optional_network);
	Required_Demand_Files (required_demand);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	max_purpose = max_zone = total_act = total_used = 0;

	factor_group.Initialize (sizeof (Factor_Group), 10);
}

//---------------------------------------------------------
//	ActBalance destructor
//---------------------------------------------------------

ActBalance::~ActBalance (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ActBalance *exe = new ActBalance ();

	return (exe->Start_Execution (commands, control));
}

