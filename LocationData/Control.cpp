//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "LocationData.hpp"

#include "Zone_File.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void LocationData::Program_Control (void)
{
	int i, size, num, decimal, distance, field;
	char *str_ptr, *format_ptr, name [FIELD_BUFFER], buffer [FIELD_BUFFER];
	Db_Code::Field_Type type;

	Db_Header *file;
	Subzone *sub_ptr;
	Zone_File *zone_file;

	//---- check for use flag processing ----

	use_flag = Get_Control_Flag (NEW_USE_FLAG_FIELD);

	if (!use_flag) {
		use_flag = (Highest_Control_Group (NEW_USE_FLAG_FIELD_x, 0) > 0);
	}
	if (use_flag) {
		Network_File_True (LINK);
	} 

	//---- check for walk access processing ----

	str_ptr = Get_Control_String (NEW_WALK_ACCESS_FIELD);
	
	if (str_ptr != NULL) {
		walk_access_flag = true;

		Network_File_True (LINK);
		Network_File_True (NODE);
		Network_File_True (PROCESS_LINK);
		Network_File_True (TRANSIT_STOP);
		Network_File_True (TRANSIT_ROUTE);
		Network_File_True (TRANSIT_SCHEDULE);
	} else {
		if (!use_flag) Network_Option_False (LINK);
		Network_Option_False (NODE);
		Network_Option_False (PROCESS_LINK);
		Network_Option_False (TRANSIT_STOP);
		Network_Option_False (TRANSIT_ROUTE);
		Network_Option_False (TRANSIT_SCHEDULE);
	}

	if (Get_Control_String (SUBZONE_ZONE_FIELD) != NULL || 
		Highest_Control_Group (SUBZONE_ZONE_FIELD_x, 0) > 0) {
		Network_File_True (ZONE);
		zone_file_flag = true;
	} else {
		Network_Option_False (ZONE);
	}

	//---- open network files ----

	Network_Service::Program_Control ();

	if (zone_file_flag) {
		zone_file = (Zone_File *) Network_Db_File (ZONE);
	}
	input_file = (Location_File *) Network_Db_File (ACTIVITY_LOCATION);
	input_file->File_ID ("In");

	output_file = (Location_File *) Network_Db_File (NEW_ACTIVITY_LOCATION);
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

	//---- add the walk access field ----
	
	if (walk_access_flag) {
		str_ptr = Get_Control_String (NEW_WALK_ACCESS_FIELD);

		Print (2, "New Walk Access Field = %s", str_ptr);

		walk_access_field = output_file->Field_Number (str_ptr);

		if (walk_access_field != 0) {
			Warning ("New Walk Access Field %s already exists as Field %d", str_ptr, walk_access_field);
		} else {
			output_file->Add_Field (str_ptr, INTEGER, 10);

			walk_access_field = output_file->Field_Number (str_ptr);

			Print (0, ", Number = %d", walk_access_field);
		}

		//---- maximum walk distance ----

		str_ptr = Get_Control_String (MAX_WALK_DISTANCE);

		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &walk_distance);

			if (walk_distance < 10 || walk_distance > 3000) {
				Error ("Maximum Walk Distance %d is Out of Range (10..3000)", walk_distance);
			}
		}
		Print (1, "Maximum Walk Distance = %d meters", walk_distance);

		//---- walk access time range ----

		str_ptr = Get_Control_String (WALK_ACCESS_TIME_RANGE);

		if (str_ptr != NULL) {
			Print (1, "Walk Access Time Range = %s", str_ptr);

			//---- get the time of day format ----
			
			format_ptr = Get_Control_String (TIME_OF_DAY_FORMAT);

			if (format_ptr == NULL) {
				format_ptr = "24_HOUR_CLOCK";
			}
			if (!walk_time.Format (format_ptr)) {
				Error ("Time of Day Format %s was Unrecognized", format_ptr);
			}
			walk_time_flag = true;
			if (!walk_time.Add_Ranges (str_ptr)) {
				Error ("Adding Walk Access Time Ranges");
			}
			Print (1, "Time of Day Format = %s", format_ptr);
		}
	}

	//---- add the use flag fields ----
	
	if (use_flag) {
		num = Highest_Control_Group (NEW_USE_FLAG_FIELD_x, 0);

		if (num == 0) {
			if (Get_Control_String (NEW_USE_FLAG_FIELD) != NULL) {
				num = 1;
			}
		} else {
			num++;
		}

		//---- process each flag ----

		for (i=0; i <= num; i++) {
			if (i == 0) {
				str_ptr = Get_Control_String (NEW_USE_FLAG_FIELD);
			} else {
				str_ptr = Get_Control_String (NEW_USE_FLAG_FIELD_x, i);
			}
			if (str_ptr == NULL) continue;

			if (i == 0) {
				Print (2, "New Use Flag Field = %s", str_ptr);
			} else {
				Print (2, "New Use Flag Field #%d = %s", i, str_ptr);
			}
			field = output_file->Field_Number (str_ptr);

			if (field != 0) {
				Warning ("New Use Flag Field %s already exists as Field %d", str_ptr, field);
			} else {
				output_file->Add_Field (str_ptr, INTEGER, 2);

				field = output_file->Field_Number (str_ptr);

				Print (0, ", Number = %d", field);
			}
			if (!use_field.Add (field)) goto use_error;

			if (i == 0) {
				str_ptr = Get_Control_String (LINK_USE_FLAG_TYPES);
			} else {
				str_ptr = Get_Control_String (LINK_USE_FLAG_TYPES_x, i);
			}
			if (str_ptr == NULL) goto control_error;

			if (i == 0) {
				Print (1, "Link Use Flag Types = %s", str_ptr);
			} else {
				Print (1, "Link Use Flag Types #%d = %s", i, str_ptr);
			}
			if (!use_code.Add ((int) Use_Code (str_ptr))) goto use_error;
		}
	}

	//---- get the new subzone fields ----

	num_subzone = Highest_Control_Group (NEW_SUBZONE_FIELD_x, 0);

	if (num_subzone == 0) {
		if (Get_Control_String (NEW_SUBZONE_FIELD) != NULL) {
			num_subzone = 1;
		}
	} else {
		num_subzone++;
	}

	if (num_subzone > 0) {

		//---- allocate subzone points ----

		subzone = new Subzone * [num_subzone];

		if (subzone == NULL) {
			Error ("Insufficient Memory for %d Subzone Files", num_subzone);
		}
		memset (subzone, '\0', num_subzone * sizeof (Subzone *));

		str_ptr = Get_Control_String (SUBZONE_ZONE_FACTOR_FILE);

		if (str_ptr != NULL) {
			subzone_map_flag = true;
			subzone_zone_file.File_Type ("Subzone Zone Factor File");
			subzone_zone_file.Open (Project_Filename (str_ptr));
		}
	}

	//---- open each file ----

	for (i=0; i < num_subzone; i++) {
		if (i == 0) {
			str_ptr = Get_Control_String (NEW_SUBZONE_FIELD);		
		} else {
			str_ptr = Get_Control_String (NEW_SUBZONE_FIELD_x, i);
		}
		if (str_ptr == NULL) continue;

		sub_ptr = subzone [i] = new Subzone;
		if (sub_ptr == NULL) {
			Error ("Insufficient Memory for Subzone Data");
		}
		sub_ptr->loc_weight.Initialize (sizeof (Loc_Weight));
		sub_ptr->zone_field_data.Initialize (sizeof (Zone_Field_Data));

		//---- create the location field ----

		if (i == 0) {
			Print (2, "New Subzone Field = %s", str_ptr);
		} else {
			Print (2, "New Subzone Field #%d = %s", i, str_ptr);
		}
		field = output_file->Field_Number (str_ptr);

		if (field != 0) {
			Error ("New Subzone Field %s already exists as Field %d", str_ptr, field);
		} else {
			output_file->Add_Field (str_ptr, INTEGER, 10);

			field = output_file->Field_Number (str_ptr);
		
			Print (0, ", Number = %d", field);
		}
		sub_ptr->location_field = field;

		if (i == 0) {
			str_ptr = Get_Control_String (MAX_SUBZONE_DISTANCE);
		} else {
			str_ptr = Get_Control_String (MAX_SUBZONE_DISTANCE_x, i);
		}
		if (str_ptr != NULL) {
			Get_Integer (str_ptr, &distance);

			if (distance != 0 && (distance < 10 || distance > 10000)) {
				Error ("Maximum Subzone Distance %d is Out of Range (0, 10..10000)", distance);
			}
		} else {
			distance = 1000;
		}
		if (distance == 0) {
			Print (1, "Data will be Allocated Proportional to Distance");
		} else {
			if (i == 0) {
				Print (1, "Maximum Subzone Distance = %d meters", distance);
			} else {
				Print (1, "Maximum Subzone Distance #%d = %d meters", i, distance);
			}
		}
		sub_ptr->max_distance = distance;

		//---- open the file ----

		if (i == 0) {
			str_ptr = Get_Control_String (SUBZONE_DATA_FILE);

			sub_ptr->file.File_Type ("Subzone Data File");
			sub_ptr->file.File_ID ("Subzone");

			format_ptr = Get_Control_String (SUBZONE_DATA_FORMAT);
		} else {
			str_ptr = Get_Control_String (SUBZONE_DATA_FILE_x, i);

			str_fmt (buffer, sizeof (buffer), "Subzone Data File #%d", i);
			sub_ptr->file.File_Type (buffer);

			str_fmt (buffer, sizeof (buffer), "Subzone%d", i);
			sub_ptr->file.File_ID (buffer);

			format_ptr = Get_Control_String (SUBZONE_DATA_FORMAT_x, i);
		}
		if (str_ptr == NULL) goto control_error;

		if (format_ptr != NULL) {
			sub_ptr->file.Dbase_Format (format_ptr);
		}
		sub_ptr->file.Open (Project_Filename (str_ptr));

		//---- find the data field ----

		if (i == 0) {
			str_ptr = Get_Control_String (SUBZONE_DATA_FIELD);
		} else {
			str_ptr = Get_Control_String (SUBZONE_DATA_FIELD_x, i);
		}
		if (str_ptr == NULL) goto control_error;

		sub_ptr->data_field = sub_ptr->file.Field_Number (str_ptr);

		if (sub_ptr->data_field == 0) {
			Error ("Subzone Data Field %s was Not Found", str_ptr);
		}
		if (i == 0) {
			Print (1, "Subzone Data Field = %s, Number = %d", str_ptr, sub_ptr->data_field);
		} else {
			Print (1, "Subzone Data Field #%d = %s, Number = %d", i, str_ptr, sub_ptr->data_field);
		}

		//---- find the zone field ----

		if (zone_file_flag) {
			if (i == 0) {
				str_ptr = Get_Control_String (SUBZONE_ZONE_FIELD);
			} else {
				str_ptr = Get_Control_String (SUBZONE_ZONE_FIELD_x, i);
			}
			if (str_ptr != NULL) {
				sub_ptr->zone_field = zone_file->Field_Number (str_ptr);

				if (sub_ptr->zone_field == 0) {
					Error ("Subzone Zone Field %s was Not Found", str_ptr);
				}
				if (i == 0) {
					Print (1, "Subzone Zone Field = %s, Number = %d", str_ptr, sub_ptr->zone_field);
				} else {
					Print (1, "Subzone Zone Field #%d = %s, Number = %d", i, str_ptr, sub_ptr->zone_field);
				}
			} else {
				sub_ptr->zone_field = 0;
			}
		}
	}

	//---- get the new activity location fields ----

	num = Highest_Control_Group (NEW_LOCATION_FIELD_x, 0);

	if (num == 0) {
		if (Get_Control_String (NEW_LOCATION_FIELD) != NULL) {
			num = 1;
		}
	} else {
		num++;
	}

	if (num == 0) {
		if (!walk_access_flag && !copy_flag && num_subzone == 0) {
			Warning ("No New Location Fields");
		}
	} else {
		Print (1);

		for (i=0; i < num; i++) {
			if (i == 0) {
				str_ptr = Get_Control_String (NEW_LOCATION_FIELD);
			} else {
				str_ptr = Get_Control_String (NEW_LOCATION_FIELD_x, i);
			}
			if (str_ptr == NULL) continue;

			if (i == 0) {
				Print (1, "New Location Field = %s", str_ptr);
			} else {
				Print (1, "New Location Field #%d = %s", i, str_ptr);
			}
			str_ptr = Get_Token (str_ptr, name, sizeof (name));
			if (name [0] == '\0') goto data_error;

			field = output_file->Field_Number (name);
			if (field != 0) goto field_error;

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
		location_field = new int [num_data_files];

		if (data_files == NULL || data_field == NULL || location_field == NULL) {
			Error ("Insufficient Memory for %d Data Files", num_data_files);
		}
		memset (data_files, '\0', num_data_files * sizeof (Db_Header *));
		memset (data_field, '\0', num_data_files * sizeof (int));
		memset (location_field, '\0', num_data_files * sizeof (int));

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

			//---- find the location join field ----

			if (i == 0) {
				str_ptr = Get_Control_String (LOCATION_JOIN_FIELD);
			} else {
				str_ptr = Get_Control_String (LOCATION_JOIN_FIELD_x, i);
			}
			if (str_ptr == NULL) goto control_error;

			location_field [i] = input_file->Field_Number (str_ptr);

			if (location_field [i] == 0) {
				Error ("Location Join Field %s was Not Found", str_ptr);
			}
			if (i == 0) {
				Print (1, "Location Join Field = %s, Number = %d", str_ptr, location_field [i]);
			} else {
				Print (1, "Location Join Field #%d = %s, Number = %d", i, str_ptr, location_field [i]);
			}
		}
	}

	//---- check for polygon files ----

	str_ptr = Get_Control_String (ZONE_BOUNDARY_POLYGON);

	if (str_ptr != NULL) {
		zone_flag = true;
	}

	//---- read the projection information ----

	if (zone_flag || polygon_flag) {
		projection.Read_Control ();
	}

	//---- get the zone boundary ----

	if (zone_flag) {
		boundary_file.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		Print (1);
		boundary_file.File_Type ("Zone Boundary Polygon");

		boundary_file.Open (Project_Filename (str_ptr));

		//--- get the zone field name ----

		str_ptr = Get_Control_String (ZONE_FIELD_NAME);

		if (str_ptr != NULL) {
			Print (1, "Zone Field Name = %s", str_ptr);

			zone_field = boundary_file.Field_Number (str_ptr);

			if (zone_field == 0) {
				Write (1, "Field %s was Not Found in Zone Boundary File", str_ptr);
			}
		} else {
			zone_field = boundary_file.Required_Field ("TAZ", "ZONE", "CENTROID", "ID", "NUMBER");
		}

		if (input_file->Zone_Field () == 0) {
			Error ("A Zone Field was Not Found in %s", input_file->File_Type ());
		}

		//---- get the zone range ----
		
		str_ptr = Get_Control_String (ZONE_UPDATE_RANGE);

		if (str_ptr != NULL) {
			Print (1, "Zone Update Range = %s", str_ptr);

			if (!zone_range.Add_Ranges (str_ptr)) {
				File_Error ("Zone Update Range", str_ptr);
			}
		}
	}

	//---- read the boundary polygons ----

	if (polygon_flag) {
		Arcview_File *poly_file;

		if (!script_flag) {
			Error ("Boundary Polygon processing Requires a Conversion Script");
		}

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
	coverage_flag = Report_Flag (ZONE_CHECK);

	Show_Message (1);
	return;

control_error:
	Error ("Missing Control Key = %s", Current_Key ());

data_error:
	Error ("New Location Field %d is Improperly Specified", i);

field_error:
	Error ("New Location Field %s already exists as Field %d", name, field);

use_error:
	Error ("Adding Use Flag Field");
}
