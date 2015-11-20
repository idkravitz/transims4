//*********************************************************
//	Write_Turn.cpp - Write the Turn Prohibition File
//*********************************************************

#include "TransimsNet.hpp"

#include "Turn_File.hpp"

//---------------------------------------------------------
//	Write_Turn
//---------------------------------------------------------

void TransimsNet::Write_Turn (void)
{
	int node;

	Turn_Data *turn_ptr;
	Link_Data *link_ptr;

	Time_Step time_step (1, "24_HOUR_CLOCK");

	Turn_File *turn_file = (Turn_File *) Network_Db_Base (NEW_TURN_PROHIBITION);
	
	Show_Message ("Writing %s - Record", turn_file->File_Type ());
	Set_Progress ();

	for (turn_ptr = turn_data.First (); turn_ptr; turn_ptr = turn_data.Next ()) {
		Show_Progress ();

		//---- get the approach link ----

		link_ptr = link_data.Get (turn_ptr->In_Link ());

		if (link_ptr == NULL) {
			Write (1, "link=%d was not found (out=%d)", turn_ptr->In_Link (), turn_ptr->Out_Link ());
			continue;
		}

		node = (turn_ptr->In_Dir () == 1) ? link_ptr->Anode () : link_ptr->Bnode ();

		//---- write the turn record ----

		turn_file->Node (node);
		turn_file->In_Link (turn_ptr->In_Link ());
		turn_file->Out_Link (turn_ptr->Out_Link ());
		turn_file->Start (time_step.Format_Step (Resolve (turn_ptr->Start ())));
		turn_file->End (time_step.Format_Step (Resolve (turn_ptr->End ())));
		turn_file->Use (Use_Code (turn_ptr->Use ()));
		turn_file->Penalty (turn_ptr->Penalty ());

		turn_file->Notes ("Turn Prohibition");

		if (!turn_file->Write ()) {
			Error ("Writing Turn Prohibition");
		}
		nturn++;	
	}
	End_Progress ();

	turn_file->Close ();
}
