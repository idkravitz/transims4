//*********************************************************
//	Stop_Process.cpp - additional transit stop processing
//*********************************************************

#include "ArcNet.hpp"

#include "Stop_File.hpp"

//---------------------------------------------------------
//	Stop_Processing
//---------------------------------------------------------

bool ArcNet::Stop_Processing (Db_File *fh)
{
	int link, dir;
	double offset;

	XYZ_Point *pt_ptr;
	Offset_Data offset_rec;
	Link_Data *link_ptr;
	Stop_Data *stop_ptr;

	Stop_File *file = (Stop_File *) fh;

	if (First_Record ()) {
		arcview_stop.Num_Points (1);

		if (!stop_offset.Max_Records (file->Num_Records ())) {
			Error ("Insufficient Memory for Transit Stop Points");
		}
	}

	if (Network_Service::Stop_Processing (file)) {
		arcview_stop.Copy_Fields (file);

		stop_ptr = stop_data.New_Record ();

		link = stop_ptr->Link ();
		dir = stop_ptr->Dir ();
		offset = UnRound (stop_ptr->Offset ());		

		link_ptr = link_data.Get (link);

		Link_Shape (link_ptr, dir, &points, offset, 0.0, stop_side);

		pt_ptr = points [1];

		arcview_stop.points.Set (1, pt_ptr);
	
		if (!arcview_stop.Write_Record ()) {
			Error ("Writing ArcView Transit Stop File");
		}

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
