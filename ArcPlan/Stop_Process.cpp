//*********************************************************
//	Stop_Process.cpp - save stop offsets
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Stop_Processing
//---------------------------------------------------------

bool ArcPlan::Stop_Processing (Db_File *fh)
{
	int link, dir;
	double offset;

	XYZ_Point *pt_ptr;
	Offset_Data offset_rec;
	Link_Data *link_ptr;
	Stop_Data *stop_ptr;

	if (First_Record ()) {
		if (!stop_offset.Max_Records (fh->Num_Records ())) {
			Error ("Insufficient Memory for Transti Stop Offsets");
		}
	}
	if (Network_Service::Stop_Processing (fh)) {
		stop_ptr = stop_data.New_Record ();

		link = stop_ptr->Link ();
		dir = stop_ptr->Dir ();
		offset = UnRound (stop_ptr->Offset ());

		link_ptr = link_data.Get (link);

		Link_Shape (link_ptr, dir, &points, offset, 0.0, stop_side);

		pt_ptr = points [1];

		//---- save the stop offset data ----

		offset_rec.ID (stop_ptr->ID ());
		offset_rec.Link (link);
		offset_rec.Dir (dir);
		offset_rec.Offset (offset);
		offset_rec.X (pt_ptr->x);
		offset_rec.Y (pt_ptr->y);
		offset_rec.Z (pt_ptr->z);

		if (!stop_offset.Add (&offset_rec)) {
			Error ("Saving Transit Stop Offsets");
		}
	}
	return (false);
}
