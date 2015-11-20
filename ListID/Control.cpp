//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ListID.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ListID::Program_Control (void)
{
	char *str_ptr;

	//---- open the input data file ----

	str_ptr = Get_Control_String (INPUT_DATA_FILE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	input_file.File_Type ("Input Data File");
	input_file.Open (Project_Filename (str_ptr));

	//---- open the output id file ----

	str_ptr = Get_Control_String (OUTPUT_ID_FILE);

	if (str_ptr == NULL) goto control_error;
	
	Print (1);
	output_file.File_Type ("Output ID File");
	output_file.File_Access (Db_File::CREATE);
	output_file.Open (Project_Filename (str_ptr));

	//---- read the number of header records ----

	str_ptr = Get_Control_String (NUM_HEADER_RECORDS);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &num_head);

		if (num_head < 0 || num_head > 10) {
			Error ("Number of Header Records %d is Out of Range (0-10)", num_head);
		}
	}
	Print (2, "Number of Header Records = %d", num_head);
	return;
	
control_error:
	Error ("Missing Control Key = %s", Current_Key ());
}
