//*********************************************************
//	Location_Process.cpp - Read the Activity Location File
//*********************************************************

#include "LinkSum.hpp"

#include "Location_File.hpp"

//---------------------------------------------------------
//	Location_Processing
//---------------------------------------------------------

bool LinkSum::Location_Processing (Db_File *fh)
{
	int i, field, link, lvalue1, lvalue2, zone;
	double dvalue1, dvalue2;
	char svalue [FIELD_BUFFER];

	Link_Data *link_ptr;
	Location_File *file;
	Db_Field *fld;
	Link_Location loc_rec;

	file = (Location_File *) fh;

	//---- allocate memory ----

	if (First_Record ()) {

		//---- initialize the link activity data ----

		if (activity_flag) {

			//---- initialize the fields ----

			for (i=2; i <= field_list.Num_Records (); i++) {
				link_db.Put_Field (i, 0);
			}

			//---- set the record key in sorted order ----

			for (link_ptr = link_data.First_Key (); link_ptr; link_ptr = link_data.Next_Key ()) {
				link_db.Put_Field (1, link_ptr->Link ());

				if (!link_db.Add_Record ()) {
					Error ("Adding Link Database");
				}
			}
		}

		//---- initialize the zone location data ---

		if (zone_flag || group_flag) {
			if (!link_location.Max_Records (file->Num_Records ())) {
				Error ("Insufficient Memory for Activity Location Data");
			}
		}
	}

	//---- get the link number ----

	link = file->Link ();
	if (link == 0) return (false);

	//---- sum the activities on links ----

	if (activity_flag) {
		if (!link_db.Get_Record (link)) {
			Error ("Reading Link Database Record %d", link);
		}

		//---- sum the data fields ----

		for (i=2, field = field_list.First (); field > 0; i++, field = field_list.Next ()) {
			fld = file->Field (field);

			if (fld->Type () == INTEGER) {
				if (file->Get_Field (field, &lvalue1)) {
					if (link_db.Get_Field (i, &lvalue2)) {
						lvalue2 += lvalue1;
						link_db.Put_Field (i, lvalue2);
					}
				}
			} else if (fld->Type () == DOUBLE) {
				if (file->Get_Field (field, &dvalue1)) {
					if (link_db.Get_Field (i, &dvalue2)) {
						dvalue2 += dvalue1;
						link_db.Put_Field (i, dvalue2);
					}
				}
			} else if (fld->Type () == STRING) {
				if (file->Get_Field (field, svalue)) {
					link_db.Put_Field (i, svalue);
				}
			}
		}

		//---- save the changes ----

		if (!link_db.Write_Record ()) {
			Error ("Writing Link Database Record %d", link);
		}
	}

	//---- add a link location record ----

	if (zone_flag || group_flag) {
		zone = file->Zone ();

		if (zone > 0) {
			loc_rec.link = link;
			loc_rec.location = file->Location ();
			loc_rec.zone = zone;
			loc_rec.count = 0;

			if (!link_location.Add (&loc_rec)) {
				Error ("Adding Link Location Record");
			}
		}
	}

	//---- don't save the location data ----

	return (false);
}
