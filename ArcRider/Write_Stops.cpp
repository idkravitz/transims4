//*********************************************************
//	Write_Stops - draw transit boardings and alightings
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	Write_Stops
//---------------------------------------------------------

void ArcRider::Write_Stops (void)
{
	int index, board, alight, stop_field, board_field, alight_field, total_field;
	bool stop_flag;

	Offset_Data *stop_ptr;
	XYZ_Point point;

	stop_field = arcview_stop.Field_Number ("STOP");
	board_field = arcview_stop.Field_Number ("BOARD");
	alight_field = arcview_stop.Field_Number ("ALIGHT");
	total_field = arcview_stop.Field_Number ("TOTAL");
	
	stop_flag = (transit_stops.Num_Ranges () > 0);

	arcview_stop.Num_Points (1);

	Show_Message ("Writing %s -- Record", arcview_stop.File_Type ());
	Set_Progress (1000);

	//---- process each stop ----

	for (stop_ptr = stop_offset.First (); stop_ptr; stop_ptr = stop_offset.Next ()) {
		Show_Progress ();

		if (stop_flag && !transit_stops.In_Range (stop_ptr->ID ())) continue;

		index = stop_offset.Record_Index ();

		board = board_data [index];
		alight = alight_data [index];

		if (board <= 0 && alight <= 0) continue;

		//---- write the stop record ----

		point.x = stop_ptr->X ();
		point.y = stop_ptr->Y ();
		point.z = stop_ptr->Z ();

		arcview_stop.Put_Field (stop_field, stop_ptr->ID ());
		arcview_stop.Put_Field (board_field, board);
		arcview_stop.Put_Field (alight_field, alight);
		arcview_stop.Put_Field (total_field, (board + alight));

		arcview_stop.points.Set (1, &point);

		if (!arcview_stop.Write_Record ()) {
			Error ("Writing ArcView Stop Demand File");
		}
		num_stop++;
	}
	End_Progress ();

	arcview_stop.Close ();
}
