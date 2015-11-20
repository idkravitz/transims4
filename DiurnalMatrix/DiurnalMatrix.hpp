//*********************************************************
//	DiurnalMatrix.hpp - sample program template
//*********************************************************

#ifndef FMTDIURNAL_HPP
#define FMTDIURNAL_HPP

#define MAX_COL 500
#define MAX_INTV 100

#include "Execution_Service.hpp"
#include "Diurnal_File.hpp"

//---------------------------------------------------------
//	DiurnalMatrix - execution class definition
//
//	Execution_Service provides standard methods for
//	program execution, control keys, printout files,
//	input/output messages and error handling
//
//---------------------------------------------------------

class DiurnalMatrix : public Execution_Service
{
public:
	DiurnalMatrix (void);
	virtual ~DiurnalMatrix (void);

	//---- override default execution method ----

	virtual void Execute (void);

protected:

	//---- program specific control keys ----

	static char * IN_DIURNAL_FILE;
	static char * IN_DIURNAL_ZONE_FIELD;
	static char * IN_DIURNAL_START_FIELD;
	static char * IN_DIURNAL_END_FIELD;
	static char * IN_DIURNAL_SHARE_FIELD;
	static char * IN_MATRIX_FILE;
	static char * IN_MATRIX_ORG_FIELD;
	static char * IN_MATRIX_DES_FIELD;
	static char * IN_MATRIX_TRIPS_FIELD;
	static char * IN_MATRIX_MATCH_FIELD;
	static char * INTERVAL_ROLLOVER;
	static char * OUT_FILE_BASE_NAME;
	static char * MAX_SHARE_COLS;

	//---- override default program control method ----

	virtual void Program_Control (void);

private:

	// internally used functions to write diurnal files and matrix files
	void Write_Diurnal (int file_cntr, int zone_cntr, int zone[], int num_intv,
								 double *start, double *end, double share[MAX_COL][MAX_INTV]);
	void Copy_Matrix (int file_cntr, int prev_zone);
	//---- program specific global variables -----

	// file names
	char input_matrix_name[FILE_NAME_SIZE];
	char input_diurnal_name[FILE_NAME_SIZE];
	char output_file_name[FILE_NAME_SIZE];
	// maximum number of share columns in the resulting diurnal distribution file(s)
	int max_col;
	// start time distribution with this interval
	int rollover;

	// field names
	char diurnal_zone_field[FIELD_BUFFER];
	char diurnal_start_field[FIELD_BUFFER];
	char diurnal_end_field[FIELD_BUFFER];
	char diurnal_share_field[FIELD_BUFFER];

	char matrix_org_field[FIELD_BUFFER];
	char matrix_des_field[FIELD_BUFFER];
	char matrix_trips_field[FIELD_BUFFER];
	char matrix_match_field[FIELD_BUFFER];

	// files
	Db_Header *input_matrix;
	Db_Header *input_file;
	Db_Header *output_file;
};
#endif
