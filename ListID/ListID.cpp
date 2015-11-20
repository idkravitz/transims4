//*********************************************************
//	ListID.cpp - extract record IDs from a file
//*********************************************************

#include "ListID.hpp"

char * ListID::INPUT_DATA_FILE      = "INPUT_DATA_FILE";
char * ListID::OUTPUT_ID_FILE       = "OUTPUT_ID_FILE";
char * ListID::NUM_HEADER_RECORDS   = "NUM_HEADER_RECORDS";

//---------------------------------------------------------
//	ListID constructor
//---------------------------------------------------------

ListID::ListID (void) : Execution_Service ()
{
	Program ("ListID");
	Version ("4.0.2");
	Title ("Extract Record ID Values from a File");

	char *keys [] = {
		INPUT_DATA_FILE, 
		OUTPUT_ID_FILE, 
		NUM_HEADER_RECORDS, 
		NULL
	};

	Key_List (keys);

	num_head = 1;
}

//---------------------------------------------------------
//	ListID destructor
//---------------------------------------------------------

ListID::~ListID (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	ListID *exe = new ListID ();

	return (exe->Start_Execution (commands, control));
}
