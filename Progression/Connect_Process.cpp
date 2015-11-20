//*********************************************************
//	Connectivity_Process.cpp - lane connectivity processing
//*********************************************************

#include "Progression.hpp"

//---------------------------------------------------------
//	Connectivity_Processing
//---------------------------------------------------------

bool Progression::Connectivity_Processing (Db_File *fh)
{
	Thru_Connect thru_rec;
	Connect_Data *connect_ptr;

	if (Network_Service::Connectivity_Processing (fh)) {

		if (!equiv_flag) {
			connect_ptr = connect_data.New_Record ();

			if (connect_ptr->Type () == THRU) {
				thru_rec.Link_Dir (connect_ptr->Out_Link_Dir ());
				thru_rec.Thru_Link_Dir (connect_ptr->In_Link_Dir ());

				if (!thru_connect.Add (&thru_rec)) {
					Error ("Adding Thru Connection Record");
				}
			} else if (!delay_flag) {
				return (false);
			}
		}
		return (true);
	}
	return (false);
}
