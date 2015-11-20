//*********************************************************
//	DiurnalMatrix.cpp - automate diurnal file reformatting
//*********************************************************

#include "DiurnalMatrix.hpp"
#include "Utility.hpp"

//---- define program specific control keys ----

char * DiurnalMatrix::IN_DIURNAL_FILE			= "IN_DIURNAL_FILE";
char * DiurnalMatrix::IN_DIURNAL_ZONE_FIELD		= "IN_DIURNAL_ZONE_FIELD";
char * DiurnalMatrix::IN_DIURNAL_START_FIELD	= "IN_DIURNAL_START_FIELD";
char * DiurnalMatrix::IN_DIURNAL_END_FIELD		= "IN_DIURNAL_END_FIELD";
char * DiurnalMatrix::IN_DIURNAL_SHARE_FIELD	= "IN_DIURNAL_SHARE_FIELD";
char * DiurnalMatrix::IN_MATRIX_FILE			= "IN_MATRIX_FILE";
char * DiurnalMatrix::IN_MATRIX_ORG_FIELD		= "IN_MATRIX_ORG_FIELD";
char * DiurnalMatrix::IN_MATRIX_DES_FIELD		= "IN_MATRIX_DES_FIELD";
char * DiurnalMatrix::IN_MATRIX_TRIPS_FIELD		= "IN_MATRIX_TRIPS_FIELD";
char * DiurnalMatrix::IN_MATRIX_MATCH_FIELD		= "IN_MATRIX_MATCH_FIELD";
char * DiurnalMatrix::INTERVAL_ROLLOVER			= "INTERVAL_ROLLOVER";
char * DiurnalMatrix::OUT_FILE_BASE_NAME		= "OUT_FILE_BASE_NAME";
char * DiurnalMatrix::MAX_SHARE_COLS			= "MAX_SHARE_COLS";

//---------------------------------------------------------
//	DiurnalMatrix constructor
//---------------------------------------------------------

DiurnalMatrix::DiurnalMatrix (void) : Execution_Service ()
{
	//---- set the program name, version numbers, and default title ----

	Program ("DiurnalMatrix");
	Version ("4.0.0");
	Title ("Reformat Diurnal and Matrix File");

	//---- add program specific keys to a null terminated array of strings ----

	char *keys [] = {
		IN_DIURNAL_FILE,
		IN_DIURNAL_ZONE_FIELD,
		IN_DIURNAL_START_FIELD,
		IN_DIURNAL_END_FIELD,
		IN_DIURNAL_SHARE_FIELD,
		IN_MATRIX_FILE,
		IN_MATRIX_ORG_FIELD,
		IN_MATRIX_DES_FIELD,
		IN_MATRIX_TRIPS_FIELD,
		IN_MATRIX_MATCH_FIELD,
		INTERVAL_ROLLOVER,
		OUT_FILE_BASE_NAME,
		MAX_SHARE_COLS,
		NULL
	};

	//---- pass the list of keys to the key processing methods ----

	Key_List (keys);

	//---- initialize global variables ----

	// file names
	input_matrix_name[0] = 0x00;
	input_diurnal_name[0] = 0x00;
	output_file_name[0] = 0x00;

	// maximum number of share columns in the resulting diurnal distribution file(s)
	max_col = 5;

	// start time distribution with this interval
	rollover = 0;

	// default values for field names in diurnal and matrix files
	str_fmt (diurnal_zone_field, sizeof(diurnal_zone_field), "ZONE");
	str_fmt (diurnal_start_field, sizeof(diurnal_zone_field), "START");
	str_fmt (diurnal_end_field, sizeof(diurnal_zone_field), "END");
	str_fmt (diurnal_share_field, sizeof(diurnal_zone_field), "SHARE");
	str_fmt (matrix_org_field, sizeof(diurnal_zone_field), "ORG");
	str_fmt (matrix_des_field, sizeof(diurnal_zone_field), "DES");
	str_fmt (matrix_trips_field, sizeof(diurnal_zone_field), "TRIPS");
	str_fmt (matrix_match_field, sizeof(diurnal_zone_field), "ORG");
}

//---------------------------------------------------------
//	DiurnalMatrix destructor
//---------------------------------------------------------

DiurnalMatrix::~DiurnalMatrix (void)
{
	//---- delete allocated pointers ----
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	//---- create an instance of the program class -----

	DiurnalMatrix *exe = new DiurnalMatrix ();

	//-------------------------------------------
	//	start program execution 
	//-------------------------------------------
	//	1.	read the command line
	//	2.	show the program banner
	//	3.	read the control file
	//	4.	open the report file
	//	5.	process the standard keys
	//	6.	call Process_Control method
	//	7.	call Execute method
	//-------------------------------------------

	return (exe->Start_Execution (commands, control));
}
