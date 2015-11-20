//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "PUMSPrep.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PUMSPrep::Program_Control (void)
{
	int i, num, fld, decimal;
	char *str_ptr, *format_ptr, buffer [FIELD_BUFFER];

	PUMS_FIELD fld_rec;
	STATE_PUMA select_rec;

	//---- get the number of pums files ----

	num_pums = Highest_Control_Group (PUMS_DATA_FILE_x, 0);

	if (num_pums == 0) {
		if (Get_Control_String (PUMS_DATA_FILE) == NULL) goto control_error;
	}
	pums_file = new Db_File * [++num_pums];
	if (pums_file == NULL) {
		Error ("Insufficient Memory for %d PUMS files", num_pums);
	}

	//---- open the PUMS data file ----

	for (i=0; i < num_pums; i++) {
		if (i == 0) {
			str_ptr = Get_Control_String (PUMS_DATA_FILE);
		} else {
			str_ptr = Get_Control_String (PUMS_DATA_FILE_x, i);
		}
		pums_file [i] = NULL;
		if (str_ptr == NULL) continue;

		pums_file [i] = new Db_File;

		Print (1);
		if (i == 0) {
			str_cpy (buffer, sizeof (buffer), "PUMS Data File");
		} else {
			str_fmt (buffer, sizeof (buffer), "PUMS Data File #%d", i);
		}
		pums_file [i]->File_Type (buffer);
		pums_file [i]->Open (Project_Filename (str_ptr));
	}

	//---- open the pums household file ----

	str_ptr = Get_Control_String (PUMS_HOUSEHOLD_FILE);

	if (str_ptr != NULL) {
		input_flag = true;

		Print (1);
		hh_file.File_Type ("PUMS Household File");

		format_ptr = Get_Control_String (PUMS_HOUSEHOLD_FORMAT);

		if (format_ptr != NULL) {
			hh_file.Dbase_Format (format_ptr);
		}
		hh_file.Open (Project_Filename (str_ptr));

		//---- open the pums population file ----

		str_ptr = Get_Control_String (PUMS_POPULATION_FILE);

		if (str_ptr == NULL) goto control_error;

		Print (1);
		pop_file.File_Type ("PUMS Population File");

		format_ptr = Get_Control_String (PUMS_POPULATION_FORMAT);

		if (format_ptr != NULL) {
			pop_file.Dbase_Format (format_ptr);
		}
		pop_file.Open (Project_Filename (str_ptr));
	}

	//---- open the new pums household file ----

	str_ptr = Get_Control_String (NEW_PUMS_HOUSEHOLD_FILE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	new_hh_file.File_Type ("New PUMS Household File");
	new_hh_file.File_ID ("New_HH");

	format_ptr = Get_Control_String (NEW_PUMS_HOUSEHOLD_FORMAT);

	if (format_ptr != NULL) {
		new_hh_file.Dbase_Format (format_ptr);
	} else {
		new_hh_file.Dbase_Format (DEFAULT_FORMAT);
	}
	new_hh_file.Create (Project_Filename (str_ptr));

	//---- create the data fields ----

	num = Highest_Control_Group (HOUSEHOLD_DATA_FIELD_x, 0);

	if (num < 1) {
		Error ("Household Data Fields are Required");
	}

	for (i=fld=1; i <= num; i++) {
		str_ptr = Get_Control_String (HOUSEHOLD_DATA_FIELD_x, i);

		if (str_ptr == NULL) continue;

		Print (1, "\tHousehold Data Field #%d = %s", i, str_ptr);

		fld_rec.field = fld++;

		str_ptr = Get_Token (str_ptr, fld_rec.name, sizeof (fld_rec.name));
		if (fld_rec.name [0] == '\0') goto data_error;

		str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
		if (buffer [0] == '\0' || buffer [0] == 'I') {
			fld_rec.type = INTEGER;
		} else if (buffer [0] == 'R' || buffer [0] == 'D') {
			fld_rec.type = DOUBLE;
		} else if (buffer [0] == 'S' || buffer [0] == 'C') {
			fld_rec.type = STRING;
		} else {
			goto data_error;
		}
		str_ptr = Get_Token (str_ptr, buffer);
		if (buffer [0] == '\0') goto data_error;

		fld_rec.start = atoi (buffer) - 1;
		if (fld_rec.start < -1) goto data_error;

		str_ptr = Get_Token (str_ptr, buffer);
		if (buffer [0] == '\0') goto data_error;

		fld_rec.length = atoi (buffer);
		if (fld_rec.length < 1) goto data_error;

		decimal = 0;

		if (fld_rec.type == DOUBLE) {
			str_ptr = Get_Token (str_ptr, buffer);
			if (buffer [0] != '\0') {
				decimal = atoi (buffer);
				if (decimal < 0 || decimal > fld_rec.length) goto data_error;
			}
		}

		if (!hh_fields.Add (&fld_rec)) {
			Error ("Adding Household Field Data");
		}
		new_hh_file.Add_Field (fld_rec.name, (Field_Type) fld_rec.type, fld_rec.length, decimal);
	}
	new_hh_file.Write_Header ();

	hhold_field = new_hh_file.Required_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");
	persons_field = new_hh_file.Required_Field ("PERSONS", "HHSIZE", "HH_SIZE", "SIZE");
	state_field = new_hh_file.Required_Field ("STATE");
	puma_field = new_hh_file.Required_Field ("PUMA");
	type_field = new_hh_file.Optional_Field ("TYPE", "HHTYPE", "HH_TYPE", "HHT");

	//---- open the new pums population file ----

	str_ptr = Get_Control_String (NEW_PUMS_POPULATION_FILE);

	if (str_ptr == NULL) goto control_error;

	Print (1);
	new_pop_file.File_Type ("New_PUMS Population File");
	new_pop_file.File_ID ("New_Pop");

	format_ptr = Get_Control_String (NEW_PUMS_POPULATION_FORMAT);

	if (format_ptr != NULL) {
		new_pop_file.Dbase_Format (format_ptr);
	} else {
		new_pop_file.Dbase_Format (DEFAULT_FORMAT);
	}
	new_pop_file.Create (Project_Filename (str_ptr));

	//---- create the data fields ----

	num = Highest_Control_Group (POPULATION_DATA_FIELD_x, 0);

	if (num < 1) {
		Error ("Population Data Fields are Required");
	}

	for (i=fld=1; i <= num; i++) {
		str_ptr = Get_Control_String (POPULATION_DATA_FIELD_x, i);

		if (str_ptr == NULL) continue;

		Print (1, "\tPopulation Data Field #%d = %s", i, str_ptr);

		fld_rec.field = fld++;

		str_ptr = Get_Token (str_ptr, fld_rec.name, sizeof (fld_rec.name));
		if (fld_rec.name [0] == '\0') goto data_error;

		str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
		if (buffer [0] == '\0' || buffer [0] == 'I') {
			fld_rec.type = INTEGER;
		} else if (buffer [0] == 'R' || buffer [0] == 'D') {
			fld_rec.type = DOUBLE;
		} else if (buffer [0] == 'S' || buffer [0] == 'C') {
			fld_rec.type = STRING;
		} else {
			goto data_error;
		}
		str_ptr = Get_Token (str_ptr, buffer);
		if (buffer [0] == '\0') goto data_error;

		fld_rec.start = atoi (buffer) - 1;
		if (fld_rec.start < -1) goto data_error;

		str_ptr = Get_Token (str_ptr, buffer);
		if (buffer [0] == '\0') goto data_error;

		fld_rec.length = atoi (buffer);
		if (fld_rec.length < 1) goto data_error;

		decimal = 0;

		if (fld_rec.type == DOUBLE) {
			str_ptr = Get_Token (str_ptr, buffer);
			if (buffer [0] != '\0') {
				decimal = atoi (buffer);
				if (decimal < 0 || decimal > fld_rec.length) goto data_error;
			}
		}
		if (!pop_fields.Add (&fld_rec)) {
			Error ("Adding Population Field Data");
		}
		new_pop_file.Add_Field (fld_rec.name, (Field_Type) fld_rec.type, fld_rec.length, decimal);
	}
	new_pop_file.Write_Header ();
	
	new_pop_file.Required_Field ("HOUSEHOLD", "HHOLD", "HH_ID", "HHID", "HH");
	person_field = new_pop_file.Optional_Field ("PERSON", "MEMBER", "PER_ID", "PERID", "PER");
	
	//---- state puma list ----

	str_ptr = Get_Control_String (STATE_PUMA_LIST);

	if (str_ptr != NULL) {
		Print (2, "State PUMA List = %s", str_ptr);

		while (str_ptr) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
			if (*buffer == '\0') break;

			str_cpy (select_rec.state, sizeof (select_rec.state), buffer, 2);

			select_rec.puma = atoi (buffer+2);

			if (!state_puma.Add (&select_rec)) {
				Error ("Adding to State PUMA List");
			}
		}
		select_flag = (state_puma.Num_Records () > 0);
	}
	
	//---- select household types ----

	str_ptr = Get_Control_String (SELECT_HOUSEHOLD_TYPES);

	if (str_ptr != NULL) {
		if (type_field == 0) {
			Error ("Household Type Selections required a Data Field");
		}
		Print (2, "Select Household Types = %s", str_ptr);

		if (!select_type.Add_Ranges (str_ptr)) {
			File_Error ("Household Type Range", str_ptr);
		}
		type_flag = true;
	}

	//---- read conversion script ----

	str_ptr = Get_Control_String (CONVERSION_SCRIPT);

	if (str_ptr != NULL) {
		script_flag = true;
		Print (1);
		program_file.File_Type ("Conversion Script");
		program_file.Open (Project_Filename (str_ptr));
	}
	return;

data_error:
	Error ("PUMS Data Field is Improperly Specified");

control_error:
	Error ("Missing Control Key = %s", Current_Key ());
}
