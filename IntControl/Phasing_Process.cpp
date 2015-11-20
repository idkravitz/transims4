//*********************************************************
//	Phasing_Process.cpp - additional phasing plan processing
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Phasing_Processing
//---------------------------------------------------------

bool IntControl::Phasing_Processing (Db_File *fh)
{
	if (!Network_Service::Phasing_Processing (fh)) return (false);
	
	if (delete_flag || !update_flag) {
		int index = phasing_data.Num_Records () + 1;

		phasing_db.Copy_Fields ((Db_Base *) fh);
		phasing_db.Put_Field (phasing_index, index);

		if (!phasing_db.Add_Record ()) {
			Error ("Writing Phasing Database");
		}
	}
	return (true);
}
