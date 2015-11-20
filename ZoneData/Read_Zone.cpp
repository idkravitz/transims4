//*********************************************************
//	Read_Zone.cpp - Read the Zone File
//*********************************************************

#include "ZoneData.hpp"

#include "In_Polygon.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Location
//---------------------------------------------------------

void ZoneData::Read_Zone (void)
{
	int i, field, id, count, num_in, num_out;
	double dx, dy;

	Db_Index_Array *data;
	Index_Array *polygon;
	Boundary *ptr;

	num_in = num_out = 0;

	//---- read the zone file to count records ----

	Show_Message ("Reading %s -- Record", input_file->File_Type ());
	Set_Progress (1000);

	while (input_file->Read ()) {
		Show_Progress ();

		id = input_file->Zone ();
		if (id == 0) continue;

		//---- count the records for the data file -----

		for (i=0; i < num_data_files; i++) {
			data = data_db [i];
			if (data == NULL) continue;

			field = zone_field [i];

			input_file->Get_Field (field, &id);

			if (id <= 0) continue;

			if (data->Get_Record (&id)) {
				data->Get_Field (2, &count);
				data->Put_Field (2, ++count);
				data->Write_Record ();
			}
		}
	}
	input_file->Rewind ();

	//---- copy the zones to the output file ----

	Set_Progress (1000);

	while (input_file->Read ()) {
		Show_Progress ();

		id = input_file->Zone ();
		if (id == 0) continue;

		num_in++;

		output_file->Copy_Fields (input_file);

		//---- copy standard field types -----

		output_file->Zone (id);
		output_file->X (input_file->X ());
		output_file->Y (input_file->Y ());
		output_file->Area_Type (input_file->Area_Type ());

		//---- set the data field id ----

		if (script_flag) {

			for (i=0; i < num_data_files; i++) {
				data = data_db [i];
				if (data == NULL) continue;

				field = zone_field [i];

				input_file->Get_Field (field, &id);

				if (!data->Get_Record (&id)) {
					data->Reset_Record ();
				}
			}

			//---- set the polygon record index ----

			if (polygon_flag) {
				dx = input_file->X ();
				dy = input_file->Y ();
			}

			for (i=0; i < num_polygons; i++) {
				data = polygon_db [i];
				if (data == NULL) continue;

				polygon = polygons [i];

				//---- find the polygon index ----

				id = 0;

				for (ptr = (Boundary *) polygon->First (); ptr; ptr = (Boundary *) polygon->Next ()) {
					if (In_Polygon (dx, dy, ptr->points)) {
						id = ptr->zone;
						break;
					}
				}
				if (!id || !data->Get_Record (&id)) {
					data->Reset_Record ();
				}
			}

			//---- execute the conversion script ----

			if (program.Execute (num_in) == 0) continue;
		}

		//---- save the output fields ----

		if (!output_file->Write ()) {
			Error ("Writing %s Record %d", output_file->File_Type (), id);
		}
		num_out++;
	}
	End_Progress ();

	input_file->Close ();
	output_file->Close ();

	Write (2, "Number of Zone Records Read = %d", num_in);
	Write (1, "Number of Zone Records Written = %d", num_out);
}
