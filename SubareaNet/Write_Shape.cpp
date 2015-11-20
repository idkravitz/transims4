//*********************************************************
//	Write_Shape.cpp - write the subarea shape file
//*********************************************************

#include "SubareaNet.hpp"

#include "Shape_File.hpp"

//---------------------------------------------------------
//	Write_Shape
//---------------------------------------------------------

void SubareaNet::Write_Shape (void)
{
	Shape_Data *shape_ptr;
	XYZ *point;
	Link_Data *link_ptr;

	Shape_File *shape_file = (Shape_File *) Network_Db_Base (NEW_SHAPE);

	char *ext_dir [] = {
		"Subarea", "External"
	};

	//---- process each subarea shape ----
	
	Show_Message ("Writing Subarea Shape Data -- Record");
	Set_Progress (1000);

	for (shape_ptr = shape_data.First (); shape_ptr; shape_ptr = shape_data.Next ()) {
		Show_Progress ();

		link_ptr = link_data.Get (shape_ptr->Link ());

		if (link_ptr == NULL || link_ptr->Type () == 0) continue;

		//---- save the shape ----

		shape_file->Link (shape_ptr->Link ());
		shape_file->Points (shape_ptr->Points ());

		if (!shape_file->Write (false)) {
			Error ("Writing %s", shape_file->File_Type ());
		}
		nshape++;

		for (int i=1; i <= shape_ptr->Points (); i++) {
			point = shape_ptr->Point (i);

			shape_file->X (UnRound (point->x));
			shape_file->Y (UnRound (point->y));
			shape_file->Z (UnRound (point->z));

			if (!shape_file->Write (true)) {
				Error ("Writing %s", shape_file->File_Type ());
			}
		}
	}
	End_Progress ();
}
