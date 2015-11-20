//*********************************************************
//	Copy_Shape.cpp - copy shape to output file
//*********************************************************

#include "ArcAddZ.hpp"

#include "Point_Array.hpp"
#include "Db_Field.hpp"

//---------------------------------------------------------
//	Copy_Shape
//---------------------------------------------------------

void ArcAddZ::Copy_Shape (File_Group *arcview)
{
	int i, nfield, z_field;
	bool binary;

	XYZ_Point pt;
	Db_Field *fld;

	z_field = arcview->input.Optional_Field ("Z_COORD", "ELEVATION", "Z");

	//----- create the fields ----

	nfield = arcview->input.Num_Fields ();
	binary = (arcview->input.Record_Format () == Db_Code::BINARY);

	for (i=1; i <= nfield; i++) {
		fld = arcview->input.Field (i);

		arcview->output.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary);
	}
	arcview->output.Write_Header ();

	//---- coppy the fields ----

	if (arcview->group_num > 0) {
		Show_Message ("Coping ArcView Shape File #%d -- Record", arcview->group_num);
	} else {
		Show_Message ("Coping ArcView Shape File -- Record");
	}
	Set_Progress ();

	//---- read the input shape file ----

	while (arcview->input.Read_Record ()) {
		Show_Progress ();

		arcview->output.parts.Reset ();
		arcview->output.points.Reset ();

		arcview->output.Copy_Fields (&arcview->input);

		if (z_field > 0) {
			arcview->output.Put_Field (z_field, z_coord);
		}

		for (i=1; i <= arcview->input.Num_Parts (); i++) {
			if (!arcview->output.parts.Add (arcview->input.parts [i])) goto point_error;
		}
		for (i=1; i <= arcview->input.Num_Points (); i++) {
			pt = *(arcview->input.points [i]);
			pt.z = z_coord;

			if (!arcview->output.points.Add (&pt)) goto point_error;
		}

		//---- save the diff record ----

		if (!arcview->output.Write_Record ()) {
			Error ("Writing %s", arcview->output.File_Type ());
		}
	}
	End_Progress ();

	arcview->input.Close ();
	arcview->output.Close ();
	return;

point_error:
	Error ("Insufficient Memory for Output Shape Points");
	return;
}
