//*********************************************************
//	Location_Process.cpp - additional location processing
//*********************************************************

#include "ConvertTours.hpp"

//---------------------------------------------------------
//	Location_Processing
//---------------------------------------------------------

bool ConvertTours::Location_Processing (Db_File *file)
{
	if (Network_Service::Location_Processing (file)) {
		double dvalue;

		Convert_Location *loc_rec = (Convert_Location *) location_data.New_Record ();

		if (loc_rec->Zone () == 0) return (false);
		if (loc_rec->Zone () > num_zone) num_zone = loc_rec->Zone ();

		if (!loc_rec->Initialize ()) {
			Error ("Insufficient Memory for Location Weights");
		}
		for (int field = field_map.First (); field; field = field_map.Next ()) {
			((Db_Base *) file)->Get_Field (field, &dvalue);

			loc_rec->Weight (field_map.Record_Index (), dvalue);
		}
		return (true);
	}
	return (false);
}
