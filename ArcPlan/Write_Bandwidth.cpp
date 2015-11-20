//*********************************************************
//	Write_Bandwidth - draw the link bandwidth polygons
//*********************************************************

#include "ArcPlan.hpp"

#include "Smooth_Shape.hpp"

//---------------------------------------------------------
//	Write_Bandwidth
//---------------------------------------------------------

void ArcPlan::Write_Bandwidth (void)
{
	int *volume, dir, link_field, dir_field, volume_field;
	double width, offset, length;

	XYZ_Point point, *ptr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;

	link_field = arcview_width.Field_Number ("LINK");
	dir_field = arcview_width.Field_Number ("DIR");
	volume_field = arcview_width.Field_Number ("VOLUME");

	Show_Message ("Writing %s -- Record", arcview_width.File_Type ());
	Set_Progress (10000);
	
	//---- process each link direction ----

	volume = width_data;
	
	for (dir_ptr = dir_data.First (); dir_ptr; dir_ptr = dir_data.Next ()) {
		Show_Progress ();

		if (*(++volume) == 0) continue;
		if (*volume < min_value) continue;

		//---- write the data fields ----

		arcview_width.Put_Field (link_field, dir_ptr->Link ());
		arcview_width.Put_Field (dir_field, dir_ptr->Dir ());
		arcview_width.Put_Field (volume_field, *volume);

		dir = dir_ptr->Dir ();

		width = *volume / width_factor;

		if (width < min_width) {
			width = min_width;
		} else if (width > max_width) {
			width = max_width;
		}
		width = -width;

		//---- retrieve the link information ----

		link_ptr = link_data.Get (dir_ptr->Link ());

		length = UnRound (link_ptr->Length ());

		if (dir == 0) {
			offset = 0.0;
		} else {
			offset = length;
		}

		//---- get the centerline points ----

		arcview_width.points.Reset ();

		Link_Shape (link_ptr, dir, &points, offset, length, 0.0);

		ptr = points.First ();
		point = *ptr;

		for (; ptr; ptr = points.Next ()) {
			if (!arcview_width.points.Add (ptr)) goto point_error;
		}

		//---- get the outside points of the band ----

		offset = length - offset;
		dir = 1 - dir;

		Link_Shape (link_ptr, dir, &points, offset, length, width);

		if (max_angle > 0 && min_length > 0) {
			Smooth_Shape (&points, max_angle, min_length);
		}

		for (ptr = points.First (); ptr; ptr = points.Next ()) {
			if (!arcview_width.points.Add (ptr)) goto point_error;
		}

		//---- close the polygon ----

		if (!arcview_width.points.Add (&point)) goto point_error;

		//---- write the shape record ----

		if (!arcview_width.Write_Record ()) {
			Error ("Writing ArcView Bandwidth File");
		}
		num_width++;
	}
	End_Progress ();

	arcview_width.Close ();
	return;

point_error:
	Error ("Insufficient Memory for %d Points", arcview_width.points.Num_Points ());
}
