//*********************************************************
//	Read_Shape.cpp - read the shape point file
//*********************************************************

#include "Network_Service.hpp"

#include "Shape_File.hpp"

//---------------------------------------------------------
//	Read_Shape
//---------------------------------------------------------

void Network_Service::Read_Shape (void)
{
	Shape_File *file = (Shape_File *) Network_Db_File (SHAPE);

	//---- store the shape point data ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress ();

	shape_data.Z_Flag (file->Z_Flag ());
	
	//file->Num_Records ();
	First_Record (true);
	int i, num;

	while (file->Read (false)) {
		Show_Progress ();

		if (Shape_Processing (file)) {
			if (!shape_data.Add ()) goto add_error;
		}
		First_Record (false);

		num = file->Points ();

		for (i=1; i <= num; i++) {
			if (!file->Read (true)) {
				Error ("Number of Nested Records for Link %d", file->Link ());
			}
			Show_Progress ();

			if (Shape_Processing (file)) {
				if (!shape_data.Add ()) goto add_error;
			}			
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, "Number of %s Records = %d", file->File_Type (), Progress_Count ());

	num = shape_data.Num_Records ();

	if (num && num != Progress_Count ()) {
		Print (1, "Number of Shape Data Records = %d", num);
	}
	shape_data.Optimize ();

	if (num > 0) Network_Data_True (SHAPE);
	return;

add_error:
	Error ("Adding Record to the Shape Data List");
}

//---------------------------------------------------------
//	Shape_Processing
//---------------------------------------------------------

bool Network_Service::Shape_Processing (Db_File *fh)
{
	static int link, npoints, point_num; 
	static bool new_shape;
	
	Shape_Data *shape_ptr;

	Shape_File *file = (Shape_File *) fh;

	//---- initialize the data processing ----

	if (First_Record ()) {
		new_shape = true;
		link = npoints = 0;
		point_num = 1;
	}

	//---- get the data record ----

	shape_ptr = shape_data.New_Record (new_shape);

	if (shape_ptr == NULL) {
		Error ("Insufficient Memory for Shape Point Data");
		return (false);
	}

	//---- process a header line ----

	if (new_shape) {	
		link = file->Link ();

		if (link == 0) return (false);

		if (link < 1) {
			return (Check_Message ("Link Number %d is Out of Range", link));
		}
		new_shape = false;
		point_num = 1;

		shape_ptr->Link (link);

		npoints = file->Points ();

		if (npoints < 1) {
			return (Check_Message ("Number of Points %d on Link %d is Out of Range", npoints, link));
		}
		if (!shape_ptr->Points (npoints)) {
			Error ("Insufficient Memory for %d Points on Link %d", npoints, link);
		}

	} else {

		//---- process a point record ----

		double x, y;
		XYZ point;

		x = file->X ();
		y = file->Y ();

		if (Projection_Flag ()) {
			if (!(Get_Projection ()->Convert (&x, &y))) {
				return (Check_Message ("Converting Coordinate %lf, %lf", x, y));
			}
		}
		point.x = Round (x);
		point.y = Round (y);
		point.z = Round (file->Z ());

		shape_ptr->Point (point_num, &point);

		if (point_num == npoints) {
			new_shape = true;
			return (true);
		} else {
			point_num++;
			return (false);
		}
	}
	return (false);
}
