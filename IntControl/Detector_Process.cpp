//*********************************************************
//	Detector_Process.cpp - additional detector processing
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Detector_Processing
//---------------------------------------------------------

bool IntControl::Detector_Processing (Db_File *fh)
{
	if (Network_Service::Detector_Processing (fh)) {
		Detector_Data *detector_ptr = detector_data.New_Record ();

		if (detector_ptr->ID () > max_detector) max_detector = detector_ptr->ID ();

		if (detector_flag) {
			detector_db.Copy_Fields ((Db_Base *) fh);

			if (!detector_db.Add_Record ()) {
				Error ("Writing Detector Database");
			}
		}
		return (true);
	}
	return (false);
}
