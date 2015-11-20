//*********************************************************
//	ProblemSum.cpp - summarize problem files
//*********************************************************

#include "ProblemSum.hpp"

char * ProblemSum::PROBLEM_FILE				= "PROBLEM_FILE";
char * ProblemSum::PROBLEM_FORMAT			= "PROBLEM_FORMAT";
char * ProblemSum::TIME_OF_DAY_FORMAT		= "TIME_OF_DAY_FORMAT";
char * ProblemSum::SUMMARY_TIME_PERIODS		= "SUMMARY_TIME_PERIODS";
char * ProblemSum::SUMMARY_TIME_INCREMENT	= "SUMMARY_TIME_INCREMENT";
char * ProblemSum::SELECT_LINKS				= "SELECT_LINKS";
char * ProblemSum::SELECT_PROBLEM_TYPES		= "SELECT_PROBLEM_TYPES";
char * ProblemSum::SELECT_SUBAREA_POLYGON	= "SELECT_SUBAREA_POLYGON";
char * ProblemSum::NEW_PROBLEM_TIME_FILE	= "NEW_PROBLEM_TIME_FILE";
char * ProblemSum::NEW_PROBLEM_TIME_FORMAT	= "NEW_PROBLEM_TIME_FORMAT";
char * ProblemSum::NEW_PROBLEM_LINK_FILE	= "NEW_PROBLEM_LINK_FILE";
char * ProblemSum::NEW_PROBLEM_LINK_FORMAT	= "NEW_PROBLEM_LINK_FORMAT";

//---------------------------------------------------------
//	ProblemSum constructor
//---------------------------------------------------------

ProblemSum::ProblemSum (void) : Demand_Service (), Problem_Service ()
{
	Program ("ProblemSum");
	Version ("4.0.5");
	Title ("Summarize Problem Files");

	Network_File optional_network [] = {
		DIRECTORY, NODE, LINK, ACTIVITY_LOCATION, END_NETWORK
	};

	char *keys [] = {
		PROBLEM_FILE,
		PROBLEM_FORMAT,
		TIME_OF_DAY_FORMAT,
		SUMMARY_TIME_PERIODS,
		SUMMARY_TIME_INCREMENT,
		SELECT_LINKS,
		SELECT_PROBLEM_TYPES,
		SELECT_SUBAREA_POLYGON,
		NEW_PROBLEM_TIME_FILE,
		NEW_PROBLEM_TIME_FORMAT,
		NEW_PROBLEM_LINK_FILE,
		NEW_PROBLEM_LINK_FORMAT,
		NULL
	};
	char *reports [] = {
		"PROBLEM_TYPE_SUMMARY",
		"PROBLEM_TIME_SUMMARY",
		"PROBLEM_DENSITY_GREATER_THAN_*",
		NULL
	};
	Optional_Network_Files (optional_network);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	link_flag = time_flag = type_flag = subarea_flag = location_flag = net_flag = false;
	problem_time_flag = problem_link_flag = false;

	num_types = header_code = 0;
	memset (num_problems, '\0', sizeof (num_problems));
}

//---------------------------------------------------------
//	ProblemSum destructor
//---------------------------------------------------------

ProblemSum::~ProblemSum (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ProblemSum *exe = new ProblemSum ();

	return (exe->Start_Execution (commands, control));
}

