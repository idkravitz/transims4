//*********************************************************
//	Location_Process.cpp - additional location processing
//*********************************************************

#include "ArcNet.hpp"

#include "Location_File.hpp"

//---------------------------------------------------------
//	Location_Processing
//---------------------------------------------------------

bool ArcNet::Location_Processing (Db_File *fh)
{
	int link, dir;
	double offset;

	XYZ_Point *pt_ptr;
	Offset_Data offset_rec;
	Link_Data *link_ptr;
	Location_Data *location_ptr;

	Location_File *file = (Location_File *) fh;

	if (First_Record ()) {
		arcview_location.Num_Points (1);

		if (!location_offset.Max_Records (file->Num_Records ())) {
			Error ("Insufficient Memory for Location Points");
		}
	}

	if (Network_Service::Location_Processing (file)) {
		arcview_location.Copy_Fields (file);

		location_ptr = location_data.New_Record ();

		link = location_ptr->Link ();
		dir = location_ptr->Dir ();
		offset = UnRound (location_ptr->Offset ());		

		link_ptr = link_data.Get (link);

		Link_Shape (link_ptr, dir, &points, offset, 0.0, location_side);

		pt_ptr = points [1];

		arcview_location.points.Set (1, pt_ptr);

		if (!arcview_location.Write_Record ()) {
			Error ("Writing ArcView Activity Location File");
		}

		//---- save the location offset data ----

		offset_rec.ID (location_ptr->ID ());
		offset_rec.Link (link);
		offset_rec.Dir (dir);
		offset_rec.Offset (offset);
		offset_rec.X (pt_ptr->x);
		offset_rec.Y (pt_ptr->y);
		offset_rec.Z (pt_ptr->z);

		if (!location_offset.Add (&offset_rec)) {
			Error ("Saving Activity Location Offsets");
		}
	}
	return (false);
}
