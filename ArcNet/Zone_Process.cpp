//*********************************************************
//	Zone_Process.cpp - additional zone processing
//*********************************************************

#include "ArcNet.hpp"

#include "Zone_File.hpp"

//---------------------------------------------------------
//	Zone_Processing
//---------------------------------------------------------

bool ArcNet::Zone_Processing (Db_File *fh)
{
	static int z_field;

	Zone_File *file = (Zone_File *) fh;
	XYZ_Point point;

	if (First_Record ()) {
		arcview_zone.Num_Points (1);
		z_field = file->Optional_Field ("Z_COORD", "ELEVATION", "Z");
	}

	if (Network_Service::Zone_Processing (file)) {
		arcview_zone.Copy_Fields (file);

		point.x = file->X ();
		point.y = file->Y ();
		file->Get_Field (z_field, &(point.z));

		arcview_zone.points.Set (1, &point);

		if (!arcview_zone.Write_Record ()) {
			Error ("Writing ArcView Zone File");
		}
	}
	return (false);
}
