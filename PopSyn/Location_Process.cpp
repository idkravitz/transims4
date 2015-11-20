//*********************************************************
//	Location_Process.cpp - Read the Activity Location File
//*********************************************************

#include "PopSyn.hpp"

#include "Location_File.hpp"

//---------------------------------------------------------
//	Location_Processing
//---------------------------------------------------------

bool PopSyn::Location_Processing (Db_File *fh)
{
	int zone, location;
	double weight;

	Location_File *file;
	Household_Model *model_ptr;
	Zone_Location *zone_ptr;
	Location_List *list_ptr;

	file = (Location_File *) fh;

	if (First_Record ()) {
		int num = file->Num_Records ();

		if (zone_loc_flag) {
			num += zone_loc_file.Num_Records ();
		}
		if (!location_list.Max_Records (num)) goto mem_error;
	}

	//---- initialize the location list ----

	location = file->Location ();
	if (location == 0) return (false);

	list_ptr = location_list.New_List (location);
	if (list_ptr == NULL) goto mem_error;

	//---- get the zone record -----

	file->Get_Field (zone_field, &zone);

	zone_ptr = zone_location.Get (zone);

	if (zone_ptr == NULL) {
		Zone_Location zone_rec;

		zone_rec.Zone (zone);
		zone_rec.Location (0);

		if (!zone_location.Add (&zone_rec)) {
			Error ("Adding Zone Location Data");
		}
		zone_ptr = zone_location.Record ();
	}

	//---- add the location to the zone list ----

	list_ptr->List (zone_ptr->Location ());
	zone_ptr->Location (location_list.Num_Records ());

	//---- get the distribution weights ----

	for (model_ptr = household_model.First (); model_ptr; model_ptr = household_model.Next ()) {
		if (model_ptr->Location_Field () > 0) {
			file->Get_Field (model_ptr->Location_Field (), &weight);
		} else {
			weight = 1.0;
		}
		list_ptr->Weight (household_model.Record_Index (), weight);
	}
	return (false);

mem_error:
	Error ("Insufficient Memory for the Location List");
	return (false);
}
