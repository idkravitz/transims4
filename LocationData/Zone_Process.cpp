//*********************************************************
//	Zone_Process.cpp - Read the Zone File
//*********************************************************

#include "LocationData.hpp"

#include "Zone_File.hpp"

//---------------------------------------------------------
//	Zone_Processing
//---------------------------------------------------------

bool LocationData::Zone_Processing (Db_File *fh)
{
	int i, zone;
	double dvalue;

	Zone_File *file;
	Zone_Field_Data zone_rec;
	Subzone *sub_ptr;

	file = (Zone_File *) fh;

	//---- get the link number ----

	zone = file->Zone ();
	if (zone == 0) return (false);

	zone_rec.zone = zone;

	for (i=0; i < num_subzone; i++) {
		sub_ptr = subzone [i];
		if (sub_ptr == NULL) continue;

		if (sub_ptr->zone_field == 0) continue;

		file->Get_Field (sub_ptr->zone_field, &dvalue);

		zone_rec.data = dvalue;

		if (!sub_ptr->zone_field_data.Add (&zone_rec)) {
			Error ("Adding Zone Data to Subzone Group");
		}
	}

	//---- don't save the location data ----

	return (false);
}
