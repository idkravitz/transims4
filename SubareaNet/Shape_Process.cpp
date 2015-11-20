//*********************************************************
//	Shape_Process.cpp - read the shape point file
//*********************************************************

#include "SubareaNet.hpp"

#include "Shape_File.hpp"

//---------------------------------------------------------
//	Shape_Processing
//---------------------------------------------------------

bool SubareaNet::Shape_Processing (Db_File *fh)
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
			Error ("Link Number %d is Out of Range", link);
		}
		new_shape = false;
		point_num = 1;

		shape_ptr->Link (link);

		npoints = file->Points ();

		if (npoints < 1) {
			Error ("Number of Points %d on Link %d is Out of Range", npoints, link);
		}
		if (!shape_ptr->Points (npoints)) {
			Error ("Insufficient Memory for %d Points on Link %d", npoints, link);
		}

	} else {

		//---- process a point record ----

		XYZ point;

		point.x = Round (file->X ());
		point.y = Round (file->Y ());
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
