//*********************************************************
//	Timing_Process.cpp - additional timing plan processing
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Timing_Processing
//---------------------------------------------------------

bool IntControl::Timing_Processing (Db_File *fh)
{
	if (Network_Service::Timing_Processing (fh)) {
		Timing_Data *timing_ptr = timing_data.New_Record ();

		if (timing_ptr->Timing () > max_timing) max_timing = timing_ptr->Timing ();

		timing_db.Copy_Fields ((Db_Base *) fh);

		if (!timing_db.Add_Record ()) {
			Error ("Writing Timing Database");
		}
		return (true);
	}
	return (false);
}
