//*********************************************************
//	Parking_Process.cpp - save parking offsets
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Parking_Processing
//---------------------------------------------------------

bool ArcPlan::Parking_Processing (Db_File *fh)
{
	int link, dir;
	double offset;

	XYZ_Point *pt_ptr;
	Offset_Data offset_rec;
	Link_Data *link_ptr;
	Parking_Data *parking_ptr;

	if (First_Record ()) {
		if (!parking_offset.Max_Records (fh->Num_Records ())) {
			Error ("Insufficient Memory for Parking Offsets");
		}
	}
	if (Network_Service::Parking_Processing (fh)) {
		parking_ptr = parking_data.New_Record ();

		link = parking_ptr->Link ();
		dir = parking_ptr->Dir ();
		offset = UnRound (parking_ptr->Offset ());

		link_ptr = link_data.Get (link);

		Link_Shape (link_ptr, dir, &points, offset, 0.0, parking_side);

		pt_ptr = points [1];

		//---- save the parking offset data ----

		offset_rec.ID (parking_ptr->ID ());
		offset_rec.Link (link);
		offset_rec.Dir (dir);
		offset_rec.Offset (offset);
		offset_rec.X (pt_ptr->x);
		offset_rec.Y (pt_ptr->y);
		offset_rec.Z (pt_ptr->z);

		if (!parking_offset.Add (&offset_rec)) {
			Error ("Saving Parking Offsets");
		}
	}
	return (false);
}
