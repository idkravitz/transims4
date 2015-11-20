//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "SF3Prep.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void SF3Prep::Program_Control (void)
{
	int i, j, num, files, fld, start, end;
	char *str_ptr, *ptr, *format_ptr, buffer [FIELD_BUFFER], name [FIELD_BUFFER], file_key [10];

	SF3_GROUP group, *group_ptr;
	SF3_FIELD *fld_ptr;
	SEGMENT segment, *seg_ptr;
	FIELD_RANGE fld_rec;
	STATE_COUNTY select_rec;

	//---- get the number of sf3 files ----

	num = Highest_Control_Group (SF3_GEOGRAPHY_FILE_x, 0);

	if (num == 0) {
		if (Get_Control_String (SF3_GEOGRAPHY_FILE) == NULL) goto control_error;
	}
	num++;

	//---- process each sf3 group ----

	for (i=0; i < num; i++) {
		if (i == 0) {
			str_ptr = Get_Control_String (SF3_GEOGRAPHY_FILE);
		} else {
			str_ptr = Get_Control_String (SF3_GEOGRAPHY_FILE_x, i);
		}
		if (str_ptr == NULL) continue;

		group.group = 0;
		group.segment = new Data_Array (sizeof (SEGMENT), 10);
		if (group.segment == NULL) goto mem_error;

		segment.number = 0;
		segment.file = new Db_File ();

		Print (1);
		if (i == 0) {
			str_cpy (buffer, sizeof (buffer), "SF3 Geography File");
		} else {
			str_fmt (buffer, sizeof (buffer), "SF3 Geography File #%d", i);
		}
		segment.file->File_Type (buffer);
		segment.file->Open (Project_Filename (str_ptr));

		//---- check the file type ----

		if (!segment.file->Read ()) goto eof_error;
		str_ptr = segment.file->Record ();
		if (str_cmp (str_ptr, "uSF3", 4)) goto sf3_error;
		if (str_ptr [4] == ',') goto geo_error;
		str_fmt (file_key, sizeof (file_key), "uSF3,%2.2s", str_ptr+6);
		segment.file->Rewind ();

		if (!group.segment->Add (&segment)) goto mem_error;

		//---- retrieve segment files ----

		files = Highest_Control_Group (SF3_SEGMENT_FILE_x, i, 0);

		if (files == 0) {
			Error ("SF3 Segment Files were Not Found for %s", buffer);
		}
		for (j=1; j <= files; j++) {
			if (i == 0) {
				str_ptr = Get_Control_String (SF3_SEGMENT_FILE_x, j);
			} else {
				str_ptr = Get_Control_String (SF3_SEGMENT_FILE_x, i, j);
			}
			if (str_ptr == NULL) continue;

			segment.number = j;
			segment.file = new Db_File ();

			if (i == 0) {
				str_fmt (buffer, sizeof (buffer), "SF3 Segment File #%d", j);
			} else {
				str_fmt (buffer, sizeof (buffer), "SF3 Segment File #%d-%d", i, j);
			}
			segment.file->File_Type (buffer);
			segment.file->Record_Format (COMMA_DELIMITED);

			if (!segment.file->Open (Project_Filename (str_ptr))) {
				Error ("Opening %s", str_ptr);
			}

			//---- check the file type ----

			if (!segment.file->Read ()) goto eof_error;
			str_ptr = segment.file->Record ();
			if (str_cmp (str_ptr, file_key, 7) != 0) goto seg_error;
			segment.file->Get_Field_Number (4, buffer, sizeof (buffer));
			segment.number = atoi (buffer);
			if (segment.number < 1 || segment.number > 76) {
				Error ("SF3 Segment Number %d is Out of Range (1..76)", segment.number);
			}
			segment.file->Rewind ();

			if (!group.segment->Add (&segment)) goto mem_error;
		}
		if (!sf3_group.Add (&group)) goto mem_error;
	}

	//---- state county list ----

	str_ptr = Get_Control_String (STATE_COUNTY_LIST);

	if (str_ptr != NULL) {
		Print (2, "State County List = %s", str_ptr);

		while (str_ptr) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
			if (*buffer == '\0') break;

			if (buffer [1] >= '0' && buffer [1] <= '9') {
				type_flag = true;
			}
			str_cpy (select_rec.state, sizeof (select_rec.state), buffer, 2);

			select_rec.county = atoi (buffer+2);

			if (!state_county.Add (&select_rec)) {
				Error ("Adding to State County List");
			}
		}
		select_flag = (state_county.Num_Records () > 0);
	}

	//---- SF3 summary level ----

	str_ptr = Get_Control_String (SF3_SUMMARY_LEVEL);

	if (str_ptr != NULL) {
		Print (2, "SF3 Summary Level = %s", str_ptr);

		sum_level = atoi (str_ptr);

		if (sum_level < 10 || sum_level > 100 || (sum_level % 10) != 0) {
			Error ("SF3 Summary Level %s is Out of Range (10..100)", sum_level);
		}
	}

	//---- open the zone data file ----

	str_ptr = Get_Control_String (ZONE_DATA_FILE);

	if (str_ptr != NULL) {
		input_flag = true;

		Print (1);
		zone_file.File_Type ("Zone Data File");

		format_ptr = Get_Control_String (ZONE_DATA_FORMAT);

		if (format_ptr != NULL) {
			zone_file.Dbase_Format (format_ptr);
		}
		zone_file.Open (Project_Filename (str_ptr));
	}

	//---- open the new zone data file ----

	str_ptr = Get_Control_String (NEW_ZONE_DATA_FILE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	new_zone_file.File_Type ("New Zone Data File");

	format_ptr = Get_Control_String (NEW_ZONE_DATA_FORMAT);

	if (format_ptr != NULL) {
		new_zone_file.Dbase_Format (format_ptr);
	} else {
		new_zone_file.Dbase_Format (DEFAULT_FORMAT);
	}
	new_zone_file.Create (Project_Filename (str_ptr));

	//---- create the data fields ----

	num = Highest_Control_Group (ZONE_DATA_FIELD_RANGE_x, 0);

	if (num < 1) {
		Error ("Zone Data Fields are Required");
	}

	//---- add the zone index and coordinate fields ----

	new_zone_file.Add_Field ("ZONE", INTEGER, 10);
	new_zone_file.Add_Field ("X_COORD", DOUBLE, 12, 1);
	new_zone_file.Add_Field ("Y_COORD", DOUBLE, 12, 1);
	fld = 3;
	Print (1);

	for (i=1; i <= num; i++) {
		str_ptr = Get_Control_String (ZONE_DATA_FIELD_RANGE_x, i);

		if (str_ptr == NULL) continue;

		Print (1, "Zone Data Field Range #%d = %s", i, str_ptr);

		//---- get the SF3 field name and range ----

		str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
		if (buffer [0] == '\0') goto data_error;

		ptr = strchr (buffer, '.');
		if (ptr != NULL) {
			*ptr++ = '\0';
			while (*ptr == '.') {
				ptr++;
			}
			end = atoi (ptr);
		} else {
			end = 0;
		}
		ptr = buffer + strlen (buffer) - 1;

		if (*ptr >= '0' && *ptr <= '9') {
			ptr -= 2;
			if (*ptr >= '0' && *ptr <= '9') {
				start = atoi (ptr);
				*ptr = '\0';
				if (end == 0) {
					end = start;
				}
			} else {
				start = 0;
			}
		} else {
			start = 0;
		}

		//---- find the field name ----

		for (fld_ptr = sf3_field; fld_ptr->table [0] != '\0'; fld_ptr++) {
			if (!str_cmp (fld_ptr->table, buffer)) break;
		}
		if (fld_ptr->table [0] == '\0') {
			Error ("SF3 Table Name %s was Not Found", buffer);
		}
		fld_rec.field = ++fld;
		fld_rec.segment = fld_ptr->segment;
		fld_rec.offset = fld_ptr->offset;

		if (start > 0) {
			fld_rec.offset += start - 1;
			fld_rec.count = end - start + 1;
			fld += end - start;
		} else if (fld_ptr->segment > 0) {
			fld_rec.count = 1;
		} else {
			fld_rec.count = fld_ptr->fields;
		}
		if (end < start) {
			Error ("SF3 Field Range is Illogical");
		}
		if (fld_ptr->segment > 0) {
			if ((start == 0 && fld_ptr->fields > 0) || end > fld_ptr->fields) {
				Error ("SF3 Field %s Range %d-%d is Out of Range (1..%d)", fld_ptr->table, start, end, fld_ptr->fields);
			}
		} else {
			if (start > 0 || end > 0) {
				Error ("SF3 Field %s does Not Accept Range %d-%d", fld_ptr->table, start, end);
			}
		}

		//---- check the groups ----

		for (group_ptr = (SF3_GROUP *) sf3_group.First (); group_ptr; group_ptr = (SF3_GROUP *) sf3_group.Next ()) {
			for (seg_ptr = (SEGMENT *) group_ptr->segment->First (); seg_ptr; seg_ptr = (SEGMENT *) group_ptr->segment->Next ()) {
				if (seg_ptr->number == fld_rec.segment) break;
			}
			if (seg_ptr == NULL) {
				Error ("SF3 Segment File %d is needed for %s data", fld_rec.segment, buffer);
			}
		}

		//---- get the output field names ----

		str_ptr = Get_Token (str_ptr, buffer);

		if (buffer [0] == '\0') {
			str_cpy (buffer, sizeof (buffer), fld_ptr->table);
		} else {
			ptr = strchr (buffer, '.');
			if (ptr != NULL) {
				*ptr++ = '\0';
				while (*ptr == '.') {
					ptr++;
				}
				end = atoi (ptr);
			} else {
				end = 0;
			}
			ptr = buffer + strlen (buffer) - 1;

			if (*ptr >= '0' && *ptr <= '9') {
				ptr--;
				if (*ptr < '0' || *ptr > '9') {
					ptr++;
				}
				start = atoi (ptr);
				*ptr = '\0';
				if (end == 0) {
					end = start;
				}
			} else {
				start = 0;
			}
			if (end < start) {
				Error ("Zone Field Range is Illogical");
			}
			if (start == 0) {
				if (fld_rec.count > 1) {
					Error ("Data Field %s Range %d-%d is Out of Range", buffer, start, end);
				}
			} else if (fld_rec.count != end - start + 1) {
				Error ("Data Field %s Range %d-%d does not have %d fields", buffer, start, end, fld_rec.count);
			}
		}
		if (!field_range.Add (&fld_rec)) {
			Error ("Adding Zone Field Data");
		}

		//---- add fields to the output file ----

		if (start == 0) {
			if (!str_cmp (fld_ptr->table, "STUSAB")) {
				new_zone_file.Add_Field (buffer, STRING, 3);
			} else {
				new_zone_file.Add_Field (buffer, INTEGER, 10);
			}
		} else {
			for (j=start; j <= end; j++) {
				str_fmt (name, sizeof (name), "%s%d", buffer, j);
				new_zone_file.Add_Field (name, INTEGER, 10);
			}
		}
	}
	new_zone_file.Write_Header ();

	//---- get the projection data ----

	proj_service.Read_Control ();
	projection.Set_Projection (proj_service.Input_Projection (), proj_service.Output_Projection ());
	return;

eof_error:
	Error ("Scanning %s", segment.file->Filename ());

sf3_error:
	Error ("%s is not an SF3 Data File", segment.file->Filename ());

geo_error:
	Error ("%s is not a SF3 Geography File", segment.file->Filename ());

seg_error:
	Error ("%s is not part of the SF3 File Group", segment.file->Filename ());

mem_error:
	Error ("Insufficient Memory for SF3 File Processing");

data_error:
	Error ("Zone Data Field is Improperly Specified");

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
}
