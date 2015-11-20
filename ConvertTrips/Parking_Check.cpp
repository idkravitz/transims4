//*********************************************************
//	Parking_Check.cpp - check for missing parking lots
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Parking_Check
//---------------------------------------------------------

void ConvertTrips::Parking_Check (void)
{
	bool link_flag;
	int count;

	Convert_Location *loc_ptr;
	Link_Data *link_ptr;

	link_flag = Network_File_Flag (LINK);

	count = 0;

	for (loc_ptr = (Convert_Location *) location_data.First (); loc_ptr != NULL; 
			loc_ptr = (Convert_Location *) location_data.Next ()) {

		if (loc_ptr->Zone () == 0) continue;
		if (loc_ptr->Org_Parking () != 0 && loc_ptr->Des_Parking () != 0) continue;

		if (link_flag) {
			link_ptr = link_data.Get (loc_ptr->Link ());
			if (link_ptr == NULL) {
				Error ("link %d was Not Found", loc_ptr->Link ());
			}
			if (!Use_Permission (link_ptr->Use (), CAR)) continue;
		}
		if (count == 0) {
			Print (2, "No Parking Lot at Activity Locations...");
		}
		if (!(count % 10)) {
			Print (1, "\t");
		}
		count++;
		Print (0, "%d ", loc_ptr->ID ());
	}
	if (count) {
		Warning ("%d Activity Locations have No Parking Lot", count);
	}
}
