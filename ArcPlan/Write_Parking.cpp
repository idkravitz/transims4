//*********************************************************
//	Write_Parking - draw parking lots demands
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Write_Parking
//---------------------------------------------------------

void ArcPlan::Write_Parking (void)
{
	int index, in, out, park_field, out_field, in_field, tot_field;

	Offset_Data *parking_ptr;
	XYZ_Point point;

	park_field = arcview_parking.Field_Number ("PARKING");
	out_field = arcview_parking.Field_Number ("DEPART");
	in_field = arcview_parking.Field_Number ("ARRIVE");
	tot_field = arcview_parking.Field_Number ("TOTAL");

	arcview_parking.Num_Points (1);

	Show_Message ("Writing %s -- Record", arcview_parking.File_Type ());
	Set_Progress (1000);

	//---- process each parking lot ----

	for (parking_ptr = parking_offset.First (); parking_ptr; parking_ptr = parking_offset.Next ()) {
		Show_Progress ();

		index = parking_offset.Record_Index ();

		out = parking_out [index];
		in = parking_in [index];

		if (in <= 0 && out <= 0) continue;

		//---- write the parking record ----

		point.x = parking_ptr->X ();
		point.y = parking_ptr->Y ();

		arcview_parking.Put_Field (park_field, parking_ptr->ID ());
		arcview_parking.Put_Field (out_field, out);
		arcview_parking.Put_Field (in_field, in);
		arcview_parking.Put_Field (tot_field, (out + in));

		arcview_parking.points.Set (1, &point);

		if (!arcview_parking.Write_Record ()) {
			Error ("Writing ArcView Parking Demand File");
		}
		num_parking++;
	}
	End_Progress ();

	arcview_parking.Close ();
}
