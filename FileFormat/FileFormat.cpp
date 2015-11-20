//*********************************************************
//	FileFormat.cpp - convert file format
//*********************************************************

#include "FileFormat.hpp"

char * FileFormat::INPUT_DIRECTORY		= "INPUT_DIRECTORY";
char * FileFormat::INPUT_FILE			= "INPUT_FILE";
char * FileFormat::INPUT_FORMAT			= "INPUT_FORMAT";
char * FileFormat::INPUT_HEADER_LINES	= "INPUT_HEADER_LINES";
char * FileFormat::NESTED_COUNT_FIELD	= "NESTED_COUNT_FIELD";
char * FileFormat::INPUT_FILE_x			= "INPUT_FILE_*";
char * FileFormat::INPUT_FORMAT_x		= "INPUT_FORMAT_*";
char * FileFormat::INPUT_HEADER_LINES_x	= "INPUT_HEADER_LINES_*";
char * FileFormat::NESTED_COUNT_FIELD_x	= "NESTED_COUNT_FIELD_*";
char * FileFormat::OUTPUT_DIRECTORY		= "OUTPUT_DIRECTORY";
char * FileFormat::OUTPUT_FILE			= "OUTPUT_FILE";
char * FileFormat::OUTPUT_FORMAT		= "OUTPUT_FORMAT";
char * FileFormat::OUTPUT_FILE_x		= "OUTPUT_FILE_*";
char * FileFormat::OUTPUT_FORMAT_x		= "OUTPUT_FORMAT_*";

//---------------------------------------------------------
//	FileFormat constructor
//---------------------------------------------------------

FileFormat::FileFormat (void) : Execution_Service ()
{
	Program ("FileFormat");
	Version ("4.0.2");
	Title ("Convert File Format");

	char *keys [] = {
		INPUT_DIRECTORY,
		INPUT_FILE,
		INPUT_FORMAT,
		INPUT_HEADER_LINES,
		NESTED_COUNT_FIELD,
		INPUT_FILE_x,
		INPUT_FORMAT_x,
		INPUT_HEADER_LINES_x,
		NESTED_COUNT_FIELD_x,
		OUTPUT_DIRECTORY,
		OUTPUT_FILE, 
		OUTPUT_FORMAT,
		OUTPUT_FILE_x,
		OUTPUT_FORMAT_x,
		NULL
	};
	Key_List (keys);

	nfiles = 0;
	input_file = NULL;
	output_file = NULL;
}

//---------------------------------------------------------
//	FileFormat destructor
//---------------------------------------------------------

FileFormat::~FileFormat (void)
{
	for (int i=0; i < nfiles; i++) {
		if (input_file [i] != NULL) {
			delete input_file [i];
		}
		if (output_file [i] != NULL) {
			delete output_file [i];
		}
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	FileFormat *exe = new FileFormat ();

	return (exe->Start_Execution (commands, control));
}
