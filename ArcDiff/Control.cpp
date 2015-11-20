//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ArcDiff.hpp"

#include "Db_Code.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ArcDiff::Program_Control (void)
{
	int i, num_group, num, type;
	char *str_ptr, buffer [STRING_BUFFER];

	Delay_Group *arcview;

	//---- get the number of groups ----

	num_group = Highest_Control_Group (ARCVIEW_DELAY_FILE_1_x, 0);

	num = Highest_Control_Group (ARCVIEW_LINK_DIR_FILE_1_x, 0);
	if (num > num_group) num_group = num;

	num = Highest_Control_Group (ARCVIEW_RIDERSHIP_FILE_1_x, 0);
	if (num > num_group) num_group = num;

	if (!delay_group.Max_Records (num_group + 1)) goto mem_error;

	//---- process each group ----

	for (i=0; i <= num_group; i++) {

		if (i == 0) {
			str_ptr = Get_Control_String (ARCVIEW_DELAY_FILE_1);

			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (ARCVIEW_LINK_DIR_FILE_1);

				if (str_ptr == NULL) {
					str_ptr = Get_Control_String (ARCVIEW_RIDERSHIP_FILE_1);
					if (str_ptr != NULL) type = RIDER_TYPE;
				} else {
					type = LINK_DIR_TYPE;
					if (Get_Control_String (ARCVIEW_RIDERSHIP_FILE_1) != NULL) goto file_error;
				}
			} else {
				type = DELAY_TYPE;
				if (Get_Control_String (ARCVIEW_LINK_DIR_FILE_1) != NULL ||
					Get_Control_String (ARCVIEW_RIDERSHIP_FILE_1) != NULL) goto file_error;
			}
			if (str_ptr == NULL && num_group == 0) goto control_error;

		} else {
			str_ptr = Get_Control_String (ARCVIEW_DELAY_FILE_1_x, i);

			if (str_ptr == NULL) {
				str_ptr = Get_Control_String (ARCVIEW_LINK_DIR_FILE_1_x, i);

				if (str_ptr == NULL) {
					str_ptr = Get_Control_String (ARCVIEW_RIDERSHIP_FILE_1_x, i);
					if (str_ptr != NULL) type = RIDER_TYPE;
				} else {
					type = LINK_DIR_TYPE;
					if (Get_Control_String (ARCVIEW_RIDERSHIP_FILE_1_x, i) != NULL) goto file_error;
				}
			} else {
				type = DELAY_TYPE;
				if (Get_Control_String (ARCVIEW_LINK_DIR_FILE_1_x, i) != NULL ||
					Get_Control_String (ARCVIEW_RIDERSHIP_FILE_1_x, i) != NULL) goto file_error;
			}
		}
		if (str_ptr == NULL) continue;

		if (!Break_Check (4)) {
			Print (1);
		}
		arcview = new Delay_Group;
		if (arcview == NULL) goto mem_error;

		arcview->group_num = i;

		if (i > 0) {
			Print (1, "File Group Number = %d", i);
		}

		//---- open arcview delay file #1 ----

		str_fmt (buffer, sizeof (buffer), "ArcView %s File #1", 
			((type == DELAY_TYPE) ? "Delay" : ((type == LINK_DIR_TYPE) ? "Link_Dir" : "Ridership")));

		arcview->delay1.File_Type (buffer);
		arcview->delay1.Shape_Type (Db_Code::VECTOR);

		if (!arcview->delay1.Open (Project_Filename (str_ptr))) {
			File_Error (arcview->delay1.File_Type (), arcview->delay1.Filename ());
		}

		//---- get the arcview delay file #2 ----

		if (i == 0) {
			if (type == DELAY_TYPE) {
				str_ptr = Get_Control_String (ARCVIEW_DELAY_FILE_2);
			} else if (type == LINK_DIR_TYPE) {
				str_ptr = Get_Control_String (ARCVIEW_LINK_DIR_FILE_2);
			} else {
				str_ptr = Get_Control_String (ARCVIEW_RIDERSHIP_FILE_2);
			}
		} else {
			if (type == DELAY_TYPE) {
				str_ptr = Get_Control_String (ARCVIEW_DELAY_FILE_2_x, i);
			} else if (type == LINK_DIR_TYPE) {
				str_ptr = Get_Control_String (ARCVIEW_LINK_DIR_FILE_2_x, i);
			} else {
				str_ptr = Get_Control_String (ARCVIEW_RIDERSHIP_FILE_2_x, i);
			}
		}
		if (str_ptr == NULL) goto control_error;

		buffer [strlen (buffer) - 1] = '2';
		arcview->delay2.File_Type (buffer);
		arcview->delay2.Shape_Type (Db_Code::VECTOR);

		if (!arcview->delay2.Open (Project_Filename (str_ptr))) {
			File_Error (arcview->delay2.File_Type (), arcview->delay2.Filename ());
		}

		//---- get the new arcview diff file ----

		if (i == 0) {
			str_ptr = Get_Control_String (NEW_ARCVIEW_DIFF_FILE);
		} else {
			str_ptr = Get_Control_String (NEW_ARCVIEW_DIFF_FILE_x, i);
		}
		if (str_ptr == NULL) goto control_error;

		arcview->diff.File_Type ("ArcView Difference File");
		arcview->diff.File_Access (Db_Code::CREATE);
		arcview->diff.Shape_Type (arcview->delay1.Shape_Type ());
		arcview->diff.Z_Flag (arcview->delay1.Z_Flag ());
		arcview->diff.M_Flag (arcview->delay1.M_Flag ());

		if (!arcview->diff.Open (Project_Filename (str_ptr))) {
			File_Error (arcview->diff.File_Type (), arcview->diff.Shape_Filename ());
		}

		//---- add the files to the group ----

		if (!delay_group.Add (arcview)) goto mem_error;
	}

	//---- percent difference flag ----

	str_ptr = Get_Control_String (PERCENT_DIFFERENCE_FLAG);

	if (str_ptr != NULL) {
		Print (2, "Percent Difference Flag = %s", str_ptr);

		percent_flag = Get_Control_Flag (PERCENT_DIFFERENCE_FLAG);
	}
	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());

mem_error:
	Error ("Insufficient Memory for Arcview Files");

file_error:
	Error ("Delay, Link Direction, and Ridership files must use different groups");
}
