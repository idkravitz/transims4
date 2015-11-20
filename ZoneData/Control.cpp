//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "ZoneData.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ZoneData::Program_Control (void)
{
	int i, size, num, decimal, existing;
	char *str_ptr, *format_ptr, name [FIELD_BUFFER], buffer [FIELD_BUFFER];
	Db_Code::Field_Type type;

	Db_Header *file;

	//---- open network files ----

	Network_Service::Program_Control ();

	input_file = (Zone_File *) Network_Db_File (ZONE);
	input_file->File_ID ("In");

	output_file = (Zone_File *) Network_Db_File (NEW_ZONE);
	output_file->File_ID ("Out");

	//---- copy existing fields ----

	str_ptr = Get_Control_String (COPY_EXISTING_FIELDS);

	if (str_ptr != NULL) {
		Print (2, "Copy Existing Fields = %s", str_ptr);

		copy_flag = Get_Control_Flag (COPY_EXISTING_FIELDS);

		if (copy_flag) {
			output_file->Clear_Fields ();
			output_file->Replicate_Fields (input_file, false);
		}
	}

	//---- get the new zone fields ----

	num = Highest_Control_Group (NEW_ZONE_FIELD_x, 0);

	if (num == 0) {
		if (Get_Control_String (NEW_ZONE_FIELD) != NULL) {
			num = 1;
		}
	} else {
		num++;
	}

	if (num == 0) {
		if (!copy_flag) {
			Warning ("No New Zone Fields");
		}
	} else {
		Print (1);

		for (i=0; i < num; i++) {
			if (i == 0) {
				str_ptr = Get_Control_String (NEW_ZONE_FIELD);
			} else {
				str_ptr = Get_Control_String (NEW_ZONE_FIELD_x, i);
			}
			if (str_ptr == NULL) continue;

			if (i == 0) {
				Print (1, "New Zone Field = %s", str_ptr);
			} else {
				Print (1, "New Zone Field #%d = %s", i, str_ptr);
			}
			str_ptr = Get_Token (str_ptr, name, sizeof (name));
			if (name [0] == '\0') goto data_error;

			existing = output_file->Field_Number (name);
			if (existing != 0) goto field_error;

			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
			if (buffer [0] == '\0' || buffer [0] == 'I') {
				type = INTEGER;
			} else if (buffer [0] == 'R' || buffer [0] == 'D') {
				type = DOUBLE;
			} else if (buffer [0] == 'S' || buffer [0] == 'C') {
				type = STRING;
			} else {
				goto data_error;
			}
			str_ptr = Get_Token (str_ptr, buffer);
			if (buffer [0] == '\0') {
				size = 10;
				if (type == DOUBLE) {
					decimal = 2;
				} else {
					decimal = 0;
				}
			} else {
				size = atoi (buffer);

				if (type == DOUBLE) {
					str_ptr = strchr (buffer, '.');
					if (str_ptr != NULL) {
						decimal = atoi (str_ptr+1);
					} else {
						decimal = 2;
					}
				} else {
					decimal = 0;
				}
			}
			output_file->Add_Field (name, type, size, decimal);
		}
	}
	if (copy_flag && output_file->Notes_Flag ()) {
		if (output_file->Field_Number ("NOTES") == 0) {
			output_file->Add_Field ("NOTES", STRING, FIELD_BUFFER);
		}
	}
	output_file->Write_Header ();
	
	//---- check for data files ----

	num_data_files = Highest_Control_Group (DATA_FILE_x, 0);

	if (num_data_files == 0) {
		if (Get_Control_String (DATA_FILE) != NULL) {
			num_data_files = 1;
		}
	} else {
		num_data_files++;
	}
	if (num_data_files > 0) {
		data_flag = true;
	}

	//---- check for polygon files ----

	num_polygons = Highest_Control_Group (BOUNDARY_POLYGON_x, 0);

	if (num_polygons == 0) {
		if (Get_Control_String (BOUNDARY_POLYGON) != NULL) {
			num_polygons = 1;
		}
	} else {
		num_polygons++;
	}
	if (num_polygons > 0) {
		polygon_flag = true;
	}

	//---- read conversion script ----

	str_ptr = Get_Control_String (CONVERSION_SCRIPT);

	if (str_ptr == NULL) {
		if (data_flag || polygon_flag) {
			Error ("A Convertion Script is needed for Data Processing");
		}
	} else {
		script_flag = true;
		Print (1);
		program_file.File_Type ("Conversion Script");
		program_file.Open (Project_Filename (str_ptr));
		Print (1);
	}

	//---- read data files ----

	if (data_flag) {

		//---- allocate data files ----

		data_files = new Db_Header * [num_data_files];
		data_field = new int [num_data_files];
		zone_field = new int [num_data_files];

		if (data_files == NULL || data_field == NULL || zone_field == NULL) {
			Error ("Insufficient Memory for %d Data Files", num_data_files);
		}
		memset (data_files, '\0', num_data_files * sizeof (Db_Header *));
		memset (data_field, '\0', num_data_files * sizeof (int));
		memset (zone_field, '\0', num_data_files * sizeof (int));

		//---- open each file ----

		for (i=0; i < num_data_files; i++) {
			if (i == 0) {
				str_ptr = Get_Control_String (DATA_FILE);
			} else {
				str_ptr = Get_Control_String (DATA_FILE_x, i);
			}
			if (str_ptr == NULL) continue;

			//---- create a header file and file labels ----

			Print (1);
			data_files [i] = file = new Db_Header ();

			if (i == 0) {
				file->File_Type ("Data File");
				file->File_ID ("Data");
				format_ptr = Get_Control_String (DATA_FORMAT);
			} else {
				str_fmt (buffer, sizeof (buffer), "Data File #%d", i);
				file->File_Type (buffer);

				str_fmt (buffer, sizeof (buffer), "Data%d", i);
				file->File_ID (buffer);

				format_ptr = Get_Control_String (DATA_FORMAT_x, i);
			}

			//---- open the file ----

			if (format_ptr != NULL) {
				file->Dbase_Format (format_ptr);
			}
			file->Open (Project_Filename (str_ptr));

			//---- find the data join field ----

			if (i == 0) {
				str_ptr = Get_Control_String (DATA_JOIN_FIELD);
			} else {
				str_ptr = Get_Control_String (DATA_JOIN_FIELD_x, i);
			}
			if (str_ptr == NULL) goto control_error;

			data_field [i] = file->Field_Number (str_ptr);

			if (data_field [i] == 0) {
				Error ("Data Join Field %s was Not Found", str_ptr);
			}
			if (i == 0) {
				Print (1, "Data Join Field = %s, Number = %d", str_ptr, data_field [i]);
			} else {
				Print (1, "Data Join Field #%d = %s, Number = %d", i, str_ptr, data_field [i]);
			}

			//---- find the zone join field ----

			if (i == 0) {
				str_ptr = Get_Control_String (ZONE_JOIN_FIELD);
			} else {
				str_ptr = Get_Control_String (ZONE_JOIN_FIELD_x, i);
			}
			if (str_ptr == NULL) goto control_error;

			zone_field [i] = input_file->Field_Number (str_ptr);

			if (zone_field [i] == 0) {
				Error ("Zone Join Field %s was Not Found", str_ptr);
			}
			if (i == 0) {
				Print (1, "Zone Join Field = %s, Number = %d", str_ptr, zone_field [i]);
			} else {
				Print (1, "Zone Join Field #%d = %s, Number = %d", i, str_ptr, zone_field [i]);
			}
		}
	}

	//---- read the boundary polygons ----

	if (polygon_flag) {
		Arcview_File *poly_file;

		projection.Read_Control ();

		//---- allocate data files ----

		polygon_files = new Arcview_File * [num_polygons];
		polygons = new Index_Array * [num_polygons];
		polygon_db = new Db_Index_Array * [num_polygons];

		if (polygon_files == NULL || polygons == NULL || polygon_db == NULL) {
			Error ("Insufficient Memory for %d Polygon Files", num_polygons);
		}
		memset (polygon_files, '\0', num_polygons * sizeof (Arcview_File *));
		memset (polygons, '\0', num_polygons * sizeof (Index_Array *));
		memset (polygon_db, '\0', num_polygons * sizeof (Db_Index_Array *));

		//---- open each file ----

		for (i=0; i < num_polygons; i++) {
			if (i == 0) {
				str_ptr = Get_Control_String (BOUNDARY_POLYGON);
			} else {
				str_ptr = Get_Control_String (BOUNDARY_POLYGON_x, i);
			}
			if (str_ptr == NULL) continue;

			//---- open the file ----

			polygon_files [i] = poly_file = new Arcview_File ();
			polygons [i] = new Index_Array (sizeof (Boundary));

			poly_file->Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

			Print (1);
			if (i == 0) {
				poly_file->File_Type ("Boundary Polygon File");
				poly_file->File_ID ("Polygon");
			} else {
				str_fmt (buffer, sizeof (buffer), "Boundary Polygon File #%d", i);
				poly_file->File_Type (buffer);

				str_fmt (buffer, sizeof (buffer), "Polygon%d", i);
				poly_file->File_ID (buffer);
			}
			poly_file->Open (Project_Filename (str_ptr));
		}
	}
	Show_Message (1);
	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());

data_error:
	Error ("New Zone Field %d is Improperly Specified", i);

field_error:
	Error ("New Zone Field %s already exists as Field %d", name, existing);
}
