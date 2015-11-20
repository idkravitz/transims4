//*********************************************************
//	SF3Dictionary.cpp - SF3 data dictionary table index
//*********************************************************

#include "SF3Dictionary.hpp"

char * SF3Dictionary::SF3_DATA_DICTIONARY    = "SF3_DATA_DICTIONARY";
char * SF3Dictionary::SF3_TABLE_INDEX        = "SF3_TABLE_INDEX";
char * SF3Dictionary::NUM_INDEX_FIELDS       = "NUM_INDEX_FIELDS";

//---------------------------------------------------------
//	SF3Dictionary constructor
//---------------------------------------------------------

SF3Dictionary::SF3Dictionary (void) : Execution_Service (), Db_Code ()
{
	Program ("SF3Dictionary");
	Version ("4.0.0");
	Title ("Extract SF3 Data Dictionary Records");

	char *keys [] = {
		SF3_DATA_DICTIONARY, 
		SF3_TABLE_INDEX, 
		NUM_INDEX_FIELDS, 
		NULL
	};

	Key_List (keys);

	num_fields = 5;
}

//---------------------------------------------------------
//	SF3Dictionary destructor
//---------------------------------------------------------

SF3Dictionary::~SF3Dictionary (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	SF3Dictionary *exe = new SF3Dictionary ();

	return (exe->Start_Execution (commands, control));
}
