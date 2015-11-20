//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "DiurnalMatrix.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Write_Diurnal
//---------------------------------------------------------
//
// This function is called to write the currently accumulated share
// data into a new diurnal distribution file. Parameters are:
//
//	file_cntr:			serial number of the file to write
//	zone_cntr:			current size of the zone array
//	zone[]:				array of zone ids
//	num_intv:			number of time intervals in the diurnal data
//	start,end:			arrays for start and end times for each interval
//	share[][]:			two-dimensional array for shares by interval and zone
//
void DiurnalMatrix::Write_Diurnal (int file_cntr, int zone_cntr, int zone[], int num_intv,
								   double *start, double *end, double share[MAX_COL][MAX_INTV])
{
	int i, j, k;
	char actual_file_name[FILE_NAME_SIZE];

	// create the distribution file name and open the file
	str_fmt (actual_file_name, sizeof(actual_file_name), "%s_%d.dist", output_file_name, file_cntr+1);
	Print (1, "Output Diurnal File %d = %s", file_cntr, actual_file_name);

	// define the header for the new distribution file
	output_file = new Db_Header(actual_file_name,Db_Code::CREATE);
	output_file->Add_Field(diurnal_start_field,Db_Code::DOUBLE,16,6);
	output_file->Add_Field(diurnal_end_field,Db_Code::DOUBLE,16,6);
	for ( i=0 ; i<=zone_cntr ; i++ ) {
		char svalue[FIELD_BUFFER];
		str_fmt (svalue, sizeof(svalue), "SHARE_%d",zone[i]);
		output_file->Add_Field(svalue,Db_Code::DOUBLE,16,6);
	}
	output_file->Write_Header();

	// loop over all zone data to find flat sum zero distributions
	// and fix them, issuing a warning
	for ( i=0 ; i<=zone_cntr ; i++ ) {
		double sum = 0.0;
		for ( j=0 ; j<num_intv ; j++ ) sum += share[i][j];
		if ( sum==0.0 ) {
			Write (1,"Warning: flat diurnal distribution for zone %d", zone[i]);
			for ( j=0 ; j<num_intv ; j++ ) share[i][j] = 1.0;
		}
	}

	// loop over the time intervals, adjust for rollovers in case the first time
	// interval doesn't start at midnight, and create the share fields in the inner loop
	for ( k=0 ; k<num_intv ; k++ ) {
		j = k + rollover;
		if ( j>=num_intv ) j = j - num_intv;
		output_file->Put_Field(1,start[j]);
		output_file->Put_Field(2,end[j]);
		for ( i=0 ; i<=zone_cntr ; i++ ) {
			output_file->Put_Field(i+3,share[i][j]);
		}
		output_file->Write();
	}
	output_file->Close();

	// create a new file name and file for the user program script
	str_fmt (actual_file_name, sizeof(actual_file_name), "%s_%d.script", output_file_name, file_cntr+1);
	Print (1, "Output Script File %d = %s", file_cntr, actual_file_name);
	output_file = new Db_Header(actual_file_name,Db_Code::CREATE);

	// loop over all zones and create the necessary if blocks, based on the
	// filed names provided by the control files or defaults
	char svalue[FIELD_BUFFER];
	for ( i=0 ; i<=zone_cntr ; i++ ) {
		str_fmt (svalue, sizeof(svalue), "IF ( Matrix.%s == %d ) THEN", matrix_match_field, zone[i]);
		output_file->Write(svalue);
		str_fmt (svalue, sizeof(svalue), "\tRETURN ( Diurnal.SHARE_%d )", zone[i]);
		output_file->Write(svalue);
		output_file->Write("ENDIF");
	}
	output_file->Close();

	return;
}

//---------------------------------------------------------
//	Write_Diurnal
//---------------------------------------------------------
//
// This function is called to copy data from the complete matrix file
// to a matrix file for just the selected zones. The assumption is that
// the complete matrix file is already sorted by the column used as the match
// criteria for the distribution. The input file is already open and
// is ready to read the next record. Parameters are:
//
//	file_cntr:			serial number of the file to write
//	last_zone:			the last zone to be copied to this file
//
void DiurnalMatrix::Copy_Matrix (int file_cntr, int last_zone)
{
	static bool more;
	static int record;
	static int prev_zone;

	// create the file name and open the new matrix file
	char actual_file_name[FILE_NAME_SIZE];
	str_fmt (actual_file_name, sizeof(actual_file_name), "%s_%d.matrix", output_file_name, file_cntr+1);
	Print (1, "Output Matrix File %d = %s", file_cntr, actual_file_name);

	// create the header for this file based on original field names
	output_file = new Db_Header(actual_file_name,Db_Code::CREATE);
	output_file->Add_Field(matrix_org_field,Db_Code::INTEGER,12);
	output_file->Add_Field(matrix_des_field,Db_Code::INTEGER,12);
	output_file->Add_Field(matrix_trips_field,Db_Code::INTEGER,12);
	output_file->Write_Header();

	// for the first invocation of this procedure, read the record and
	// initialize a few static variables to keep track of the status between
	// calls to this procedure
	if ( file_cntr==0 ) {
		input_matrix->Read();
		more = true;
		record = 1;
		prev_zone = -1;
	}

	// loop to read records and interprete/copy them
	while (true) {
		int test_zone;

		// if a record has been already read from the file (flag "more"),
		// get the "match" field from the input file. If there sin't any more,
		// set the test_field variable to something higher that the last zone
		// to be witten to force proper termination of the loop.
		if ( more ) {
			input_matrix->Get_Field(matrix_match_field,&test_zone);
		} else {
			test_zone = last_zone + 1;
		}

		// break out of the input loop and return to the calling program
		// after closing the current output file.
		if ( test_zone>last_zone ) {
			break;
		}

		// check for proper sequence of input records, they need to be sorted
		// by the "match" field. Exception is for the first record where there
		// obviously nothing to compare to.
		// If the test succeeds, copy input to output fields and write the record.
		if ( record == 1 || test_zone>=prev_zone ) {
			int ivalue;
			input_matrix->Get_Field(matrix_org_field,&ivalue);
			output_file->Put_Field(matrix_org_field,ivalue);
			input_matrix->Get_Field(matrix_des_field,&ivalue);
			output_file->Put_Field(matrix_des_field,ivalue);
			input_matrix->Get_Field(matrix_trips_field,&ivalue);
			output_file->Put_Field(matrix_trips_field,ivalue);
			output_file->Write();
			prev_zone = test_zone;
		} else {
			Error ("%s in Matrix File Out of Sequence on Line %d", matrix_match_field, record);
		}

		// delayed read: we read at the end of the loop to make it easier to
		// check for the next "match" field.
		more = input_matrix->Read();
		record++;
	}

	// close the file and return
	output_file->Close();
	return;
}

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void DiurnalMatrix::Execute (void)
{
	// array to hold the shares by share column and time interval
	double share[MAX_COL][MAX_INTV];
	// array to store the zone id of each zone for the current output file
	int zone[MAX_COL];
	// array of start times
	double start[MAX_INTV];
	// array of end times
	double end[MAX_INTV];
	// variables to be used when reading from a file for comparison
	double test_start;
	double test_end;

	Show_Message ("Reading Input Diurnal File -- Record");
	Set_Progress ();

	// initialize all counters and indicators
	int curr_zone;
	int intv_cntr = -1;
	int prev_zone = -1;
	int zone_cntr = -1;
	int file_cntr = -1;
	int record = -1;
	int num_intv = 0;

	// loop to read the entire file until there are no more records
	for (;input_file->Read();) {
		Show_Progress ();
		record++;

		// read the next zone id from the complex distribution file
		// and check for the proper sequence (it must be sorted by the
		// zone field and the intervals).
		// Once the maximum number of zones per output distribution file has
		// been read in, call the output routine and the copy
		// matrix routine to dump the information to the next
		// set of output files.
		input_file->Get_Field(diurnal_zone_field,&curr_zone);
		if ( curr_zone<prev_zone ) {
			Error ("%s in Diurnal File Out of Sequence on Line %d", diurnal_zone_field, record);
		} else if ( curr_zone!=prev_zone ) {
			if ( zone_cntr>=max_col-1 ) {
				file_cntr++;
				Write_Diurnal (file_cntr, zone_cntr, zone, num_intv,
							   start, end, share);
				Copy_Matrix (file_cntr, prev_zone);
				zone_cntr = -1;
			}
			prev_zone = curr_zone;
			zone_cntr++;
			intv_cntr = -1;
		}
		intv_cntr++;

		// accumulate the input data in the appropriate arrays. Fix
		// time stamp problems. The first time around, remember the
		// start and end fields. For all subsequent zones, the start
		// and end fields are checked against the first set, and an error
		// is created if the order or values are different.
		input_file->Get_Field(diurnal_zone_field,&zone[zone_cntr]);
		input_file->Get_Field(diurnal_start_field,&test_start);
		input_file->Get_Field(diurnal_end_field,&test_end);
		if ( test_end == 0.0 ) test_end=24.0;
		input_file->Get_Field(diurnal_share_field,&share[zone_cntr][intv_cntr]);
		if ( file_cntr==-1 && zone_cntr==0 ) {
			start[intv_cntr] = test_start;
			end[intv_cntr] = test_end;
			num_intv++;
		} else {
			if ( start[intv_cntr]!=test_start || end[intv_cntr] != test_end ) {
				Error ("Time Interval Out of Sequence on Line %d", record);
			}
		}
	}

	// dump remaining share data into a last file if there is anything to dump
	file_cntr++;
	if ( zone_cntr>0 ) {
		Write_Diurnal (file_cntr, zone_cntr, zone, num_intv,
					start, end, share);
		Copy_Matrix (file_cntr, prev_zone);
	}
	End_Progress ();

	//---- write a message to the screen and the printout file ----

	Write (1, "Number of Records = %d", Progress_Count ());

	//---- terminate successful execution ----

	Exit_Stat (DONE);
}
