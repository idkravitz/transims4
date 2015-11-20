//*********************************************************
//	Signal_Process.cpp - additional signal processing
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Signal_Processing
//---------------------------------------------------------

bool IntControl::Signal_Processing (Db_File *fh)
{
	if (!Network_Service::Signal_Processing (fh)) return (false);
	
	if (delete_flag || !update_flag) {
		int index = signal_data.Num_Records () + 1;

		signal_db.Copy_Fields ((Db_Base *) fh);
		signal_db.Put_Field (signal_index, index);

		if (!signal_db.Add_Record ()) {
			Error ("Writing Signal Database");
		}
	}
	return (true);
}
