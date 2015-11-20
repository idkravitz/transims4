//*********************************************************
//	Read_Polygons.cpp - read the boundary polygon files
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Read_Polygons
//---------------------------------------------------------

void LocationData::Read_Polygons (void)
{
	int i, index, num_pts;

	Arcview_File *poly_file;
	Boundary poly_rec;
	Index_Array *polygon;
	Db_Index_Array *data;

	//---- read the arcview boundary file ----

	for (i=0; i < num_polygons; i++) {
		poly_file = polygon_files [i];
		if (poly_file == NULL) continue;

		Show_Message ("Reading %s -- Record", poly_file->File_Type ());
		Set_Progress (1000);

		//---- read each record ----

		index = 0;
		polygon = polygons [i];
		data = polygon_db [i];

		while (poly_file->Read_Record ()) {
			Show_Progress ();

			poly_rec.zone = ++index;
			num_pts = poly_file->Num_Points ();

			poly_rec.points = new Point_Data ();

			if (poly_rec.points == NULL || !poly_rec.points->Num_Points (num_pts)) {
				Error ("Insufficient Memory for Boundary Polygons");
			}
			num_pts *= sizeof (XY_Point);

			memcpy (poly_rec.points->Get_XY_Data (1), poly_file->Get_XY_Data (1), num_pts);

			if (!polygon->Add (&poly_rec)) {
				Error ("Adding Boundary Polygon");
			}

			//----- add to the database ----

			data->Copy_Fields (poly_file);
			data->Put_Field (1, index);

			if (!data->Add_Record ()) {
				Error ("Writing Boundary Polygon Database");
			}
		}
		End_Progress ();

		Print (2, "Number of %s Records = %d", poly_file->File_Type (), Progress_Count ());
		
		poly_file->Close ();
	}
}
