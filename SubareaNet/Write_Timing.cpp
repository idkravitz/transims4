//*********************************************************
//	Write_Timing - write a timing plan file
//*********************************************************

#include "SubareaNet.hpp"

#include "Timing_File.hpp"

//---------------------------------------------------------
//	Write_Timing
//---------------------------------------------------------

void SubareaNet::Write_Timing (void)
{
	int timing, timing_field;

	Signal_Data *signal_ptr;
	
	Timing_File *timing_file = (Timing_File *) Network_Db_Base (NEW_TIMING_PLAN);

	Show_Message ("Writing Subarea Timing Plan -- Record");
	Set_Progress (1000);

	timing_field = timing_db.Required_Field ("TIMING", "PLAN");

	timing_db.Rewind ();

	while (timing_db.Read_Record ()) {
		Show_Progress ();

		timing_db.Get_Field (timing_field, &timing);

		for (signal_ptr = signal_data.First (); signal_ptr; signal_ptr = signal_data.Next ()) {
			if (signal_ptr->Timing () == timing) {
				timing_file->Copy_Fields (&timing_db);

				if (!timing_file->Write ()) {
					Error ("Writing %s", timing_file->File_Type ());
				}
				ntiming++;
				break;
			}
		}
	}
	End_Progress ();
}
