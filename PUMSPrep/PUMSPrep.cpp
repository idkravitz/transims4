//*********************************************************
//	PUMSPrep.cpp - PUMS data extraction
//*********************************************************

#include "PUMSPrep.hpp"

char * PUMSPrep::PUMS_DATA_FILE              = "PUMS_DATA_FILE";
char * PUMSPrep::PUMS_DATA_FILE_x            = "PUMS_DATA_FILE_*";
char * PUMSPrep::PUMS_HOUSEHOLD_FILE         = "PUMS_HOUSEHOLD_FILE";
char * PUMSPrep::PUMS_HOUSEHOLD_FORMAT       = "PUMS_HOUSEHOLD_FORMAT";
char * PUMSPrep::PUMS_POPULATION_FILE        = "PUMS_POPULATION_FILE";
char * PUMSPrep::PUMS_POPULATION_FORMAT      = "PUMS_POPULATION_FORMAT";
char * PUMSPrep::NEW_PUMS_HOUSEHOLD_FILE     = "NEW_PUMS_HOUSEHOLD_FILE";
char * PUMSPrep::NEW_PUMS_HOUSEHOLD_FORMAT   = "NEW_PUMS_HOUSEHOLD_FORMAT";
char * PUMSPrep::NEW_PUMS_POPULATION_FILE    = "NEW_PUMS_POPULATION_FILE";
char * PUMSPrep::NEW_PUMS_POPULATION_FORMAT  = "NEW_PUMS_POPULATION_FORMAT";
char * PUMSPrep::HOUSEHOLD_DATA_FIELD_x      = "HOUSEHOLD_DATA_FIELD_*";
char * PUMSPrep::POPULATION_DATA_FIELD_x     = "POPULATION_DATA_FIELD_*";
char * PUMSPrep::STATE_PUMA_LIST             = "STATE_PUMA_LIST";
char * PUMSPrep::SELECT_HOUSEHOLD_TYPES      = "SELECT_HOUSEHOLD_TYPES";
char * PUMSPrep::CONVERSION_SCRIPT           = "CONVERSION_SCRIPT";

//---------------------------------------------------------
//	PUMSPrep constructor
//---------------------------------------------------------

PUMSPrep::PUMSPrep (void) : Execution_Service (), Db_Code ()
{
	Program ("PUMSPrep");
	Version ("4.0.1");
	Title ("PUMS Data Extraction");

	char *keys [] = {
		PUMS_DATA_FILE, 
		PUMS_DATA_FILE_x,
		PUMS_HOUSEHOLD_FILE, 
		PUMS_HOUSEHOLD_FORMAT,
		PUMS_POPULATION_FILE,
		PUMS_POPULATION_FORMAT,
		NEW_PUMS_HOUSEHOLD_FILE, 
		NEW_PUMS_HOUSEHOLD_FORMAT,
		NEW_PUMS_POPULATION_FILE,
		NEW_PUMS_POPULATION_FORMAT,
		HOUSEHOLD_DATA_FIELD_x,
		POPULATION_DATA_FIELD_x,
		STATE_PUMA_LIST,
		SELECT_HOUSEHOLD_TYPES,
		CONVERSION_SCRIPT,
		NULL
	};
	char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		NULL
	};

	Key_List (keys);
	Report_List (reports);

	input_flag = select_flag = type_flag = script_flag = false;
	hhold_field = state_field = puma_field = persons_field = type_field = person_field = 0;
	num_pums = nrec = pums_hh = pums_pop = hh_in = pop_in = hh_out = pop_out = 0;

	pums_file = NULL;

	hh_fields.Initialize (sizeof (PUMS_FIELD), 10);
	pop_fields.Initialize (sizeof (PUMS_FIELD), 10);
	state_puma.Initialize (sizeof (STATE_PUMA), 10);
}

//---------------------------------------------------------
//	PUMSPrep destructor
//---------------------------------------------------------

PUMSPrep::~PUMSPrep (void)
{
	if (num_pums > 0 && pums_file != NULL) {
		for (int i=0; i < num_pums; i++) {
			if (pums_file [i] != NULL) {
				delete pums_file [i];
			}
		}
		delete [] pums_file;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	PUMSPrep *exe = new PUMSPrep ();

	return (exe->Start_Execution (commands, control));
}
