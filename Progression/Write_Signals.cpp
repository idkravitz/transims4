//*********************************************************
//	Write_Signals.cpp - write the new signalized node file
//*********************************************************

#include "Progression.hpp"

#include "Signal_File.hpp"

//---------------------------------------------------------
//	Write_Signals
//---------------------------------------------------------

void Progression::Write_Signals (void)
{
	int node, start;
	int node_field, offset_field, start_field;
	char buffer [20];

	Time_Step time_step;
	Signal_Data *signal_ptr;
	Signal_Offset *offset_ptr;
	
	Signal_File *signal_file = (Signal_File *) Network_Db_Base (NEW_SIGNALIZED_NODE);

	Show_Message ("Writing %s -- Record", signal_file->File_Type ());
	Set_Progress (1000);

	node_field = signal_db.Required_Field ("NODE");
	offset_field = signal_db.Required_Field ("OFFSET");
	start_field = signal_db.Optional_Field ("START", "STARTTIME", "START_TIME");

	signal_db.Rewind ();

	while (signal_db.Read_Record ()) {
		Show_Progress ();

		signal_file->Copy_Fields (&signal_db);

		signal_db.Get_Field (node_field, &node);
		
		//---- start time ----

		if (start_field > 0) {
			signal_db.Get_Field (start_field, buffer);

			if (buffer [0] != '\0') {
				start = time_step.Step (buffer);
			} else {
				start = 0;
			}
		}

		//---- retrieve the signal data record ----

		signal_ptr = signal_data.Get (node, start);

		if (signal_ptr != NULL) {
			if (signal_ptr->Type () == TIMED) {
				offset_ptr = signal_offset.Get (signal_data.Record_Index ());

				if (offset_ptr != NULL && offset_ptr->Preset () == 0 && offset_ptr->Offset () >= 0) {
					signal_file->Put_Field (offset_field, offset_ptr->Offset ());
					updated++;
				}
				fixed++;
			}
		}
		if (!signal_file->Write ()) {
			Error ("Writing %s", signal_file->File_Type ());
		}
	}
	End_Progress ();
}
