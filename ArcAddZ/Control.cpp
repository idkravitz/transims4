//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ArcAddZ.hpp"

#include "Db_Code.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ArcAddZ::Program_Control (void)
{
	int i, num_group;
	char *str_ptr;

	File_Group *arcview;

	str_ptr = Get_Control_String (ADD_Z_AS_MEASURE);
	if (str_ptr != NULL) {
		measure_flag = Get_Control_Flag (ADD_Z_AS_MEASURE);
	}

	//---- get the number of groups ----

	num_group = Highest_Control_Group (ARCVIEW_SHAPE_FILE_x, 0);

	if (!file_group.Max_Records (num_group + 1)) goto mem_error;

	//---- process each group ----

	for (i=0; i <= num_group; i++) {

		if (i == 0) {
			str_ptr = Get_Control_String (ARCVIEW_SHAPE_FILE);
			if (str_ptr == NULL && num_group == 0) goto control_error;

		} else {
			str_ptr = Get_Control_String (ARCVIEW_SHAPE_FILE_x, i);
		}
		if (str_ptr == NULL) continue;

		if (!Break_Check (3)) {
			Print (1);
		}
		arcview = new File_Group;
		if (arcview == NULL) goto mem_error;

		arcview->group_num = i;

		if (i > 0) {
			Print (1, "File Group Number = %d", i);
		}

		//---- open arcview shape file ----

		arcview->input.File_Type ("ArcView Shape File");

		if (!arcview->input.Open (Project_Filename (str_ptr))) {
			File_Error (arcview->input.File_Type (), arcview->input.Filename ());
		}

		//---- get the new arcview shape file ----

		if (i == 0) {
			str_ptr = Get_Control_String (NEW_ARCVIEW_SHAPE_FILE);
		} else {
			str_ptr = Get_Control_String (NEW_ARCVIEW_SHAPE_FILE_x, i);
		}
		if (str_ptr == NULL) goto control_error;

		arcview->output.File_Type ("ArcView Shape File");
		arcview->output.File_Access (Db_Code::CREATE);
		arcview->output.Shape_Type (arcview->input.Shape_Type ());
		arcview->output.Z_Flag (true);
		arcview->output.M_Flag (measure_flag);

		if (!arcview->output.Open (Project_Filename (str_ptr))) {
			File_Error (arcview->output.File_Type (), arcview->output.Shape_Filename ());
		}

		//---- add the files to the group ----

		if (!file_group.Add (arcview)) goto mem_error;
	}

	//---- get the output format flag ----

	str_ptr = Get_Control_String (ADD_Z_AS_MEASURE);

	if (str_ptr != NULL) {
		Print (2, "Add Z Coordinates as a Measure = %s", str_ptr);
	} else {
		Print (2, "Z Coordinates are Added as a Shape Measure");
	}

	//---- get the default Z coordinates ----

	str_ptr = Get_Control_String (DEFAULT_Z_VALUE);

	if (str_ptr != NULL) {
		Print (2, "Default Z Value = %s meters", str_ptr);
		z_coord = atof (str_ptr);
	}
	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());

mem_error:
	Error ("Insufficient Memory for Arcview Files");
}
