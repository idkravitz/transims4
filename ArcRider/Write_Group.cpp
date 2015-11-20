//*********************************************************
//	Write_Group - draw stop group boardings and alightings
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	Write_Group
//---------------------------------------------------------

void ArcRider::Write_Group (void)
{
	int i, stop, num, index, board, alight;
	int group_field, board_field, alight_field, total_field, name_field;
	double x, y, z;

	Offset_Data *stop_ptr;
	XYZ_Point point;
	Integer_List *group;

	group_field = arcview_group.Field_Number ("GROUP");
	name_field = arcview_group.Field_Number ("NAME");
	board_field = arcview_group.Field_Number ("BOARD");
	alight_field = arcview_group.Field_Number ("ALIGHT");
	total_field = arcview_group.Field_Number ("TOTAL");

	arcview_group.Num_Points (1);

	Show_Message ("Writing %s -- Record", arcview_group.File_Type ());
	Set_Progress (10);

	//---- process each stop group ----

	for (i=1; i <= stop_equiv.Num_Groups (); i++) {

		group = stop_equiv.Group_List (i);
		if (group == NULL) continue;

		Show_Progress ();

		board = alight = num = 0;
		x = y = z = 0.0;

		//---- sum the stops in the group ----

		for (stop = group->First (); stop != 0; stop = group->Next ()) {

			stop_ptr = stop_offset.Get (stop);
			if (stop_ptr == NULL) continue;

			x += stop_ptr->X ();
			y += stop_ptr->Y ();
			z += stop_ptr->Z ();

			index = stop_offset.Record_Index ();

			board += board_data [index];
			alight += alight_data [index];
			num++;
		}
		if (board <= 0 && alight <= 0) continue;

		//---- write the stop record ----

		point.x = x / num;
		point.y = y / num;
		point.z = z / num;

		arcview_group.Put_Field (group_field, i);
		arcview_group.Put_Field (name_field, stop_equiv.Group_Label (i));
		arcview_group.Put_Field (board_field, board);
		arcview_group.Put_Field (alight_field, alight);
		arcview_group.Put_Field (total_field, (board + alight));

		arcview_group.points.Set (1, &point);

		if (!arcview_group.Write_Record ()) {
			Error ("Writing ArcView Stop Group File");
		}
		num_group++;
	}
	End_Progress ();

	arcview_group.Close ();
}
