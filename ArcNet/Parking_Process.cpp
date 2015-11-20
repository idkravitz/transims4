//*********************************************************
//	Parking_Process.cpp - additional parking processing
//*********************************************************

#include "ArcNet.hpp"

#include "Parking_File.hpp"

//---------------------------------------------------------
//	Parking_Processing
//---------------------------------------------------------

bool ArcNet::Parking_Processing (Db_File *fh)
{
	int link, dir;
	double offset;

	XYZ_Point *pt_ptr;
	Offset_Data offset_rec;
	Link_Data *link_ptr;
	Parking_Data *parking_ptr;

	Parking_File *file = (Parking_File *) fh;

	if (First_Record ()) {
		arcview_parking.Num_Points (1);

		if (!parking_offset.Max_Records (file->Num_Records ())) {
			Error ("Insufficient Memory for Parking Points");
		}
	}

	if (Network_Service::Parking_Processing (file)) {
		arcview_parking.Copy_Fields (file);

		parking_ptr = parking_data.New_Record ();

		if (select_time) {
			if (!time_range.Span_Range (parking_ptr->Start (), parking_ptr->End ())) return (false);
		}
		link = parking_ptr->Link ();
		dir = parking_ptr->Dir ();
		offset = UnRound (parking_ptr->Offset ());		

		link_ptr = link_data.Get (link);

		Link_Shape (link_ptr, dir, &points, offset, 0.0, parking_side);

		pt_ptr = points [1];

		arcview_parking.points.Set (1, pt_ptr);

		if (!arcview_parking.Write_Record ()) {
			Error ("Writing ArcView Parking File");
		}

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
