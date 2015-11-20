//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "DiurnalMatrix.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void DiurnalMatrix::Program_Control (void)
{
	char *str_ptr;
	int ivalue;
	char svalue[FIELD_BUFFER];
	bool success;

	//---- input matrix file ----

	str_ptr = Get_Control_String (IN_MATRIX_FILE);

	if (str_ptr == NULL) goto control_error;

	Get_Token (str_ptr, input_matrix_name, FILE_NAME_SIZE);

	if (input_matrix_name == NULL) {
		Error ("IN_MATRIX_FILE Control Key is Required");
	}
	Print (2, "Input Matrix File = %s", input_matrix_name);
	input_matrix = new Db_Header(input_matrix_name,Db_Code::READ);

	str_ptr = Get_Control_String (IN_MATRIX_ORG_FIELD);
	if (str_ptr != NULL) {
		Get_Token (str_ptr, matrix_org_field);
		success = input_matrix->Get_Field(matrix_org_field,svalue);
		if (!success) Error ("Field \"%s\" Missing in File \"%s\"", matrix_org_field, input_matrix_name);
	}
	Print (1, "Origin Field = \"%s\"", matrix_org_field);

	str_ptr = Get_Control_String (IN_MATRIX_DES_FIELD);
	if (str_ptr != NULL) {
		Get_Token (str_ptr, matrix_des_field);
		success = input_matrix->Get_Field(matrix_des_field,svalue);
		if (!success) Error ("Field \"%s\" Missing in File \"%s\"", matrix_des_field, input_matrix_name);
	}
	Print (1, "Destination Field = \"%s\"", matrix_des_field);

	str_ptr = Get_Control_String (IN_MATRIX_TRIPS_FIELD);
	if (str_ptr != NULL) {
		Get_Token (str_ptr, matrix_trips_field);
		success = input_matrix->Get_Field(matrix_trips_field,svalue);
		if (!success) Error ("Field \"%s\" Missing in File \"%s\"", matrix_trips_field, input_matrix_name);
	}
	Print (1, "Trips Field = \"%s\"", matrix_trips_field);

	str_ptr = Get_Control_String (IN_MATRIX_MATCH_FIELD);
	if (str_ptr != NULL) {
		Get_Token (str_ptr, matrix_match_field);
		success = input_matrix->Get_Field(matrix_match_field,svalue);
		if (!success) Error ("Field \"%s\" Missing in File \"%s\"", matrix_match_field, input_matrix_name);
	}
	Print (1, "Match Field = \"%s\"", matrix_match_field);

	//---- input file ----

	str_ptr = Get_Control_String (IN_DIURNAL_FILE);

	if (str_ptr == NULL) goto control_error;

	Get_Token (str_ptr, input_diurnal_name, FILE_NAME_SIZE);

	if (input_diurnal_name == NULL) {
		Error ("IN_DIURNAL_FILE Control Key is Required");
	}
	Print (2, "Input Diurnal File = %s", input_diurnal_name);
	input_file = new Db_Header(input_diurnal_name,Db_Code::READ);

	str_ptr = Get_Control_String (IN_DIURNAL_ZONE_FIELD);
	if (str_ptr != NULL) {
		Get_Token (str_ptr, diurnal_zone_field);
		success = input_file->Get_Field(diurnal_zone_field,svalue);
		if (!success) Error ("Field \"%s\" Missing in File \"%s\"", diurnal_zone_field, input_diurnal_name);
	}
	Print (1, "Zone Field = \"%s\"", diurnal_zone_field);

	str_ptr = Get_Control_String (IN_DIURNAL_START_FIELD);
	if (str_ptr != NULL) {
		Get_Token (str_ptr, diurnal_start_field);
		success = input_file->Get_Field(diurnal_start_field,svalue);
		if (!success) Error ("Field \"%s\" Missing in File \"%s\"", diurnal_start_field, input_diurnal_name);
	}
	Print (1, "Start Time Field = \"%s\"", diurnal_start_field);

	str_ptr = Get_Control_String (IN_DIURNAL_END_FIELD);
	if (str_ptr != NULL) {
		Get_Token (str_ptr, diurnal_end_field);
		success = input_file->Get_Field(diurnal_end_field,svalue);
		if (!success) Error ("Field \"%s\" Missing in File \"%s\"", diurnal_end_field, input_diurnal_name);
	}
	Print (1, "End Time Field = \"%s\"", diurnal_end_field);

	str_ptr = Get_Control_String (IN_DIURNAL_SHARE_FIELD);
	if (str_ptr != NULL) {
		Get_Token (str_ptr, diurnal_share_field);
		success = input_file->Get_Field(diurnal_share_field,svalue);
		if (!success) Error ("Field \"%s\" Missing in File \"%s\"", diurnal_share_field, input_diurnal_name);
	}
	Print (1, "Share Field = \"%s\"", diurnal_share_field);
	
	//---- output file ----

	str_ptr = Get_Control_String (OUT_FILE_BASE_NAME);

	if (str_ptr == NULL) goto control_error;

	Get_Token (str_ptr, output_file_name, FILE_NAME_SIZE);

	if (output_file_name == NULL) {
		Error ("OUT_FILE_BASE_NAME Control Key is Required");
	}

	//---- maximum number of share columns in each output file ----

	str_ptr = Get_Control_String (MAX_SHARE_COLS);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &ivalue);
		if ( ivalue<1 || ivalue>MAX_COL ) {
			Error ("Maximum Number of Share Columns %d is Out of Range (<%d)", ivalue, MAX_COL);
		}
		max_col = ivalue;
	}
	Print (2, "Maximum Number of Share Columns in Each Output File = %d", max_col);
	
	//---- time interval rollover offset ----

	str_ptr = Get_Control_String (INTERVAL_ROLLOVER);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &ivalue);
		if ( ivalue<0 || ivalue>=MAX_INTV ) {
			Error ("Time Interval Rollover Offset %d is Out of Range (<%d)", ivalue, MAX_INTV);
		}
		rollover = ivalue;
	}
	Print (2, "Time Interval Rollover Offset = %d", rollover);
	
	return;
	
control_error:
	Error ("Missing Control Key = %s", Current_Key ());
}
