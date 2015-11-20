//*********************************************************
//	Write_Activity.cpp - write the activity file
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Write_Activity
//---------------------------------------------------------

void PlanTrips::Write_Activity (void)
{
	Activity_Data *act_ptr;

	char buffer [FIELD_BUFFER];
	
	Show_Message ("Writing %s", act_file->File_Type ());
	Set_Progress (10000);

	//---- process each activity ----

	for (act_ptr = activity_data.First_Key (); act_ptr; act_ptr = activity_data.Next_Key ()) {
		Show_Progress ();

		act_file->Household (act_ptr->Household ());
		act_file->Person (act_ptr->Person ());
		act_file->Activity (act_ptr->Activity ());
		act_file->Purpose (act_ptr->Purpose ());
		act_file->Priority (act_ptr->Priority ());

		trip_time.Format_Step (act_ptr->Start_Time (), buffer);
		act_file->Start_Min (buffer);
		act_file->Start_Max (buffer);
		
		trip_time.Format_Step (act_ptr->End_Time (), buffer);
		act_file->End_Min (buffer);
		act_file->End_Max (buffer);
		
		trip_time.Format_Duration (act_ptr->Duration (), buffer);
		act_file->Time_Min (buffer);
		act_file->Time_Max (buffer);

		act_file->Mode (act_ptr->Mode ());
		act_file->Vehicle (act_ptr->Vehicle ());
		act_file->Location (act_ptr->Location ());
		act_file->Passengers (act_ptr->Passengers ());
		act_file->Constraint (act_ptr->Constraint ());

		if (!act_file->Write ()) {
			Error ("Writing %s", act_file->File_Type ());
		}
		num_act++;
	}
	End_Progress ();

	act_file->Close ();
}
