//*********************************************************
//	Signal_Process.cpp - read the signal file
//*********************************************************

#include "Progression.hpp"

#include "Signal_File.hpp"

//---------------------------------------------------------
//	Signal_Processing
//---------------------------------------------------------

bool Progression::Signal_Processing (Db_File *fh)
{
	Signal_Offset *offset_ptr;
	Signal_Data *signal_ptr;

	Signal_File *file = (Signal_File *) fh;	

	//---- reserve memory ----

	if (First_Record ()) {
		if (!signal_db.Max_Records (file->Num_Records ())) {
			Error ("Insufficient Memory for Signal Database");
		}
	}

	if (Network_Service::Signal_Processing (fh)) {

		//---- save the database record ----

		signal_db.Copy_Fields (file);

		if (!signal_db.Add_Record ()) {
			Error ("Writing Signal Database");
		}

		//---- create the fixed signal data ----

		signal_ptr = signal_data.New_Record ();

		if (signal_ptr->Type () == TIMED) {
			offset_ptr = new Signal_Offset (signal_data.Num_Records () + 1);
	
			if (offset_ptr == NULL) {
				Error ("Creating Signal Offset Record");
			}
			offset_ptr->Input (signal_ptr->Offset ());

			if (!clear_flag) {
				offset_ptr->Preset (signal_ptr->Offset ());
			}
			if (period_flag) {
				offset_ptr->Period (progress_time.Period (signal_ptr->Start ()));
				if (offset_ptr->Period () == 0) {
					offset_ptr->Preset (signal_ptr->Offset ());
				}
			} else {
				offset_ptr->Period (1);
			}
			if (!signal_offset.Add (offset_ptr)) {
				Error ("Adding Signal Offset Record");
			}
		}
		return (true);
	}
	return (false);
}
