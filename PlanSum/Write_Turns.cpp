//*********************************************************
//	Write_Turns.cpp - Create Output Turn Movement File
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Write_Turns
//---------------------------------------------------------

void PlanSum::Write_Turns (void)
{
	int i;
	char *time1_ptr, *time2_ptr;

	Movement_Data *move_ptr;
	Movement_Array *array_ptr;
	Dir_Data *dir_ptr;

	Show_Message ("Writing %s -- Record", turn_file.File_Type ());
	Set_Progress (10000);

	//---- write each turn movement ----

	for (i=1; i <= num_inc; i++) {
		array_ptr = movement_data [i];

		time1_ptr = time_periods.Range_Format (i);

		time2_ptr = strstr (time1_ptr, "..");
		*time2_ptr = '\0';
		time2_ptr += 2;

		for (move_ptr = array_ptr->First_Key (); move_ptr; move_ptr = array_ptr->Next_Key ()) {
			Show_Progress ();

			turn_file.Node (move_ptr->node);

			dir_ptr = dir_data [move_ptr->in_link];
			turn_file.In_Link (dir_ptr->Link ());

			dir_ptr = dir_data [move_ptr->out_link];
			turn_file.Out_Link (dir_ptr->Link ());

			turn_file.Start (time1_ptr);
			turn_file.End (time2_ptr);
			turn_file.Volume (move_ptr->volume);
	
			//---- save the record ----

			if (!turn_file.Write ()) {
				Error ("Writing Turn Movement File");
			}
		}
	}
	End_Progress ();

	Print (2, "Number of %s Records = %d", turn_file.File_Type (), Progress_Count ());

	turn_file.Close ();
}
