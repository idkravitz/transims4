//*********************************************************
//	ProblemSelect.cpp - Select Households for Re-Routing
//*********************************************************

#include "ProblemSelect.hpp"

char * ProblemSelect::PROBLEM_FILE				= "PROBLEM_FILE";
char * ProblemSelect::PROBLEM_FORMAT			= "PROBLEM_FORMAT";
char * ProblemSelect::HOUSEHOLD_LIST			= "HOUSEHOLD_LIST";
char * ProblemSelect::NEW_HOUSEHOLD_LIST		= "NEW_HOUSEHOLD_LIST";
char * ProblemSelect::SELECT_LINKS				= "SELECT_LINKS";
char * ProblemSelect::SELECT_TIME_PERIODS		= "SELECT_TIME_PERIODS";
char * ProblemSelect::SELECT_PROBLEM_TYPES		= "SELECT_PROBLEM_TYPES";
char * ProblemSelect::SELECT_SUBAREA_POLYGON	= "SELECT_SUBAREA_POLYGON";
char * ProblemSelect::SELECTION_PERCENTAGE		= "SELECTION_PERCENTAGE";
char * ProblemSelect::MAXIMUM_PERCENT_SELECTED	= "MAXIMUM_PERCENT_SELECTED";
char * ProblemSelect::RANDOM_NUMBER_SEED		= "RANDOM_NUMBER_SEED";
char * ProblemSelect::TIME_OF_DAY_FORMAT		= "TIME_OF_DAY_FORMAT";

//---------------------------------------------------------
//	ProblemSelect constructor
//---------------------------------------------------------

ProblemSelect::ProblemSelect (void) : Demand_Service (), Problem_Service ()
{
	Program ("ProblemSelect");
	Version ("4.0.4");
	Title ("Summarize Problem Files");

	Network_File optional_network [] = {
		DIRECTORY, NODE, LINK, ACTIVITY_LOCATION, END_NETWORK
	};

	char *keys [] = {
		PROBLEM_FILE,
		PROBLEM_FORMAT,
		HOUSEHOLD_LIST,
		NEW_HOUSEHOLD_LIST,
		SELECT_LINKS,
		SELECT_TIME_PERIODS,
		SELECT_PROBLEM_TYPES,
		SELECT_SUBAREA_POLYGON,
		SELECTION_PERCENTAGE,
		MAXIMUM_PERCENT_SELECTED,
		RANDOM_NUMBER_SEED,
		TIME_OF_DAY_FORMAT,
		NULL
	};
	Optional_Network_Files (optional_network);

	Key_List (keys);
	Enable_Partitions (true);

	select_flag = link_flag = time_flag = type_flag = subarea_flag = hh_flag = false;

	nproblem = num_select = nhhold = nselect = 0;
	percent = max_percent = 100.0;
}

//---------------------------------------------------------
//	ProblemSelect destructor
//---------------------------------------------------------

ProblemSelect::~ProblemSelect (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ProblemSelect *exe = new ProblemSelect ();

	return (exe->Start_Execution (commands, control));
}

