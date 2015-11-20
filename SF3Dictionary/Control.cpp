//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "SF3Dictionary.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void SF3Dictionary::Program_Control (void)
{
	char *str_ptr;

	//---- open the sf3 data dictionary ----

	str_ptr = Get_Control_String (SF3_DATA_DICTIONARY);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	input_file.File_Type ("SF3 Data Dictionary");
	input_file.Open (Project_Filename (str_ptr));

	//---- open the sf3 table index ----

	str_ptr = Get_Control_String (SF3_TABLE_INDEX);

	if (str_ptr == NULL) goto control_error;
	
	Print (1);
	output_file.File_Type ("SF3 Table Index");
	output_file.Dbase_Format (COMMA_DELIMITED);
	output_file.Create (Project_Filename (str_ptr));

	output_file.Add_Field ("TABLE", STRING, 12);
	output_file.Add_Field ("SEGMENT", INTEGER, 4);
	output_file.Add_Field ("OFFSET", INTEGER, 6);
	output_file.Add_Field ("FIELDS", INTEGER, 4);

	output_file.Write_Header ();

	//---- read the number of index fields ----

	str_ptr = Get_Control_String (NUM_INDEX_FIELDS);

	if (str_ptr != NULL) {
		Get_Integer (str_ptr, &num_fields);

		if (num_fields < 0 || num_fields > 10) {
			Error ("Number of Index Fields %d is Out of Range (0-10)", num_fields);
		}
	}
	Print (2, "Number of SF3 Index Fields = %d", num_fields);
	return;
	
control_error:
	Error ("Missing Control Key = %s", Current_Key ());
}
