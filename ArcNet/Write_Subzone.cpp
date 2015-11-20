//*********************************************************
//	Write_Subzone.cpp - converts subzone data file
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Write_Subzone
//---------------------------------------------------------

void ArcNet::Write_Subzone (void)
{
	XYZ_Point point;

	int z_field = subzone_file.Optional_Field ("Z_COORD", "ELEVATION", "Z");

	arcview_subzone.Num_Points (1);

	Show_Message ("Reading %s -- Record", subzone_file.File_Type ());
	Set_Progress (1000);

	while (subzone_file.Read ()) {
		Show_Progress ();

		arcview_subzone.Copy_Fields (&subzone_file);

		point.x = subzone_file.X ();
		point.y = subzone_file.Y ();
		subzone_file.Get_Field (z_field, &(point.z));

		arcview_subzone.points.Set (1, &point);

		if (!arcview_subzone.Write_Record ()) {
			Error ("Writing ArcView Subzone File");
		}
	}
	End_Progress ();
	
	subzone_file.Close ();
	arcview_subzone.Close ();

	Print (2, "Number of %s Records = %d", subzone_file.File_Type (), Progress_Count ());
}
