//*********************************************************
//	Read_SF3.cpp - main execution procedure
//*********************************************************

#include "SF3Prep.hpp"

//---------------------------------------------------------
//	Read_SF3
//---------------------------------------------------------

void SF3Prep::Read_SF3 (SF3_GROUP *group)
{
	int i, rec_id, county, level, state_offset, record, rec_num;
	char *rec_ptr, buffer [FIELD_BUFFER], *str_ptr;
	double x_coord, y_coord;
	bool keep;

	Db_File *geo_file;
	SEGMENT *segment;
	STATE_COUNTY *sel_ptr;
	FIELD_RANGE *fld_ptr;

	segment = (SEGMENT *) group->segment->First ();
	geo_file = segment->file;

	Show_Message ("Reading %s -- Record", geo_file->File_Type ());
	Set_Progress ();

	if (type_flag) {
		state_offset = 29;
	} else {
		state_offset = 6;
	}
	rec_id = 0;
	keep = false;

	while (geo_file->Read ()) {
		Show_Progress ();

		rec_ptr = geo_file->Record ();
		nrec++;

		//---- check the summary level ----

		memcpy (buffer, rec_ptr + 8, 3);
		buffer [3] = '\0';

		level = atoi (buffer);
		if (level != sum_level) continue;

		//---- check the selection range ----

		if (select_flag) {
			keep = false;
			memcpy (buffer, rec_ptr + 31, 3);
			buffer [3] = '\0';

			county = atoi (buffer);

			memcpy (buffer, rec_ptr + state_offset, 2);
			buffer [2] = '\0';

			for (sel_ptr = (STATE_COUNTY *) state_county.First (); sel_ptr; sel_ptr = (STATE_COUNTY *) state_county.Next ()) {
				if (sel_ptr->county == county) {
					if (str_cmp (sel_ptr->state, buffer) == 0) {
						keep = true;
						break;
					}
				}
			}
			if (!keep) continue;
		}

		//---- get the record id ----

		memcpy (buffer, rec_ptr + 29, 2);
		buffer [2] = '\0';

		rec_id = atoi (buffer) * 1000000;

		memcpy (buffer, rec_ptr + 18, 7);
		buffer [7] = '\0';

		record = atoi (buffer);

		rec_id += record;

		//---- get the coordinates ----

		memcpy (buffer, rec_ptr + 310, 9);
		buffer [9] = '\0';

		y_coord = atof (buffer);

		memcpy (buffer, rec_ptr + 319, 10);
		buffer [10] = '\0';

		x_coord = atof (buffer);

		projection.Convert (&x_coord, &y_coord);

		//---- process the fields from each segment file ----

		for (segment = (SEGMENT *) group->segment->First (); segment; segment = (SEGMENT *) group->segment->Next ()) {

			//---- locate the record number ----

			if (segment->number > 0) {
				while (segment->file->Read ()) {
					segment->file->Get_Field_Number (5, buffer, sizeof (buffer));
					rec_num = atoi (buffer);
					if (rec_num >= record) break;
				}
				if (rec_num > record) continue;
			}

			//---- process each field ----

			for (fld_ptr = (FIELD_RANGE *) field_range.First (); fld_ptr; fld_ptr = (FIELD_RANGE *) field_range.Next ()) {
				if (fld_ptr->segment != segment->number) continue;

				if (segment->number > 0) {
					str_ptr = segment->file->Record ();

					for (i=0; i < fld_ptr->offset && str_ptr != NULL; i++) {
						str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
					}
				} else {
					memcpy (buffer, rec_ptr + fld_ptr->offset, fld_ptr->count);
					buffer [fld_ptr->count] = '\0';	
				}

				new_zone_file.Put_Field (fld_ptr->field, buffer);

				if (segment->number > 0 && fld_ptr->count > 1) {
					for (i=1; i < fld_ptr->count; i++) {
						str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
						new_zone_file.Put_Field (fld_ptr->field + i, buffer);
					}
				}
			}
		}
		new_zone_file.Put_Field (1, rec_id);
		new_zone_file.Put_Field (2, x_coord);
		new_zone_file.Put_Field (3, y_coord);

		if (!new_zone_file.Write ()) {
			Error ("Writing %s", new_zone_file.File_Type ());
		}
		zone_out++;
	}
	End_Progress ();

	//---- close the files ----

	for (segment = (SEGMENT *) group->segment->First (); segment; segment = (SEGMENT *) group->segment->Next ()) {
		segment->file->Close ();
	}
}
