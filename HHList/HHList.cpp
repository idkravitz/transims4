//*********************************************************
//	HHList.cpp - create a partitioned household list
//*********************************************************

#include "HHList.hpp"

char * HHList::TRIP_FILE			= "TRIP_FILE";
char * HHList::TRIP_FORMAT			= "TRIP_FORMAT";
char * HHList::ACTIVITY_FILE		= "ACTIVITY_FILE";
char * HHList::ACTIVITY_FORMAT		= "ACTIVITY_FORMAT";
char * HHList::HOUSEHOLD_FILE       = "HOUSEHOLD_FILE";
char * HHList::HOUSEHOLD_FORMAT		= "HOUSEHOLD_FORMAT";
char * HHList::HOUSEHOLD_LIST		= "HOUSEHOLD_LIST";
char * HHList::NEW_HOUSEHOLD_LIST	= "NEW_HOUSEHOLD_LIST";
char * HHList::NUM_SPLIT_FILES		= "NUM_SPLIT_FILES";
char * HHList::RANDOM_NUMBER_SEED	= "RANDOM_NUMBER_SEED";

//---------------------------------------------------------
//	HHList constructor
//---------------------------------------------------------

HHList::HHList (void) : Execution_Service ()
{
	Program ("HHList");
	Version ("4.0.1");
	Title ("Create a Partitioned Household List");

	char *keys [] = {
		TRIP_FILE,
		TRIP_FORMAT,
		ACTIVITY_FILE,
		ACTIVITY_FORMAT,
		HOUSEHOLD_FILE,
		HOUSEHOLD_FORMAT,
		HOUSEHOLD_LIST,
		NEW_HOUSEHOLD_LIST,
		NUM_SPLIT_FILES,
		RANDOM_NUMBER_SEED,
		NULL
	};

	Key_List (keys);

	num_files = 0;
	list_flag = two_flag = false;
	output_file = NULL;
}

//---------------------------------------------------------
//	HHList destructor
//---------------------------------------------------------

HHList::~HHList (void)
{
	if (num_files > 0 && output_file != NULL) {
		for (int i=0; i < num_files; i++) {
			if (output_file [i] != NULL) {
				output_file [i]->Close ();
			}
		}
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	HHList *exe = new HHList ();

	return (exe->Start_Execution (commands, control));
}
