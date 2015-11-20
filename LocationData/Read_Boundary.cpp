//*********************************************************
//	Read_Boundary.cpp - read the zone boundary file 
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Read_Boundary
//---------------------------------------------------------

void LocationData::Read_Boundary (void)
{
	int zone, num_pts;

	Boundary zone_rec;

	//---- read the arcview boundary file ----

	Show_Message ("Reading %s -- Record", boundary_file.File_Type ());
	Set_Progress (1000);

	while (boundary_file.Read_Record ()) {
		Show_Progress ();

		//---- get the zone field ----

		boundary_file.Get_Field (zone_field, &zone);
		if (zone == 0) continue;

		zone_rec.zone = zone;
		num_pts = boundary_file.Num_Points ();

		zone_rec.points = new Point_Data ();

		if (zone_rec.points == NULL || !zone_rec.points->Num_Points (num_pts)) {
			Error ("Insufficient Memory for Zone Polygons");
		}
		num_pts *= sizeof (XY_Point);

		memcpy (zone_rec.points->Get_XY_Data (1), boundary_file.Get_XY_Data (1), num_pts);

		if (!zone_boundary.Add (&zone_rec)) {
			Error ("Adding Zone Boundary");
		}
	}
	End_Progress ();

	Print (2, "Number of %s Records = %d", boundary_file.File_Type (), Progress_Count ());
	
	boundary_file.Close ();
}
