//*********************************************************
//	Pop_Header.cpp - construct the population file header
//*********************************************************

#include "NewFormat.hpp"

#include "Household_File.hpp"
#include "Population_File.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Pop_Header
//---------------------------------------------------------

void NewFormat::Pop_Header (void)
{
	int i, j;
	char *ptr, *str_ptr, buffer [FIELD_BUFFER];
	bool colon_flag, version2;
	Db_Field *fld;

	Household_File *household_file;
	Population_File *population_file;

	household_file = (Household_File *) Demand_Db_Base (NEW_HOUSEHOLD);
	population_file = (Population_File *) Demand_Db_Base (NEW_POPULATION);

	//---- read the header records ----

	pop_file.Nested (true);
	pop_file.Dbase_Format (SPACE_DELIMITED);
	pop_file.File_Format (UNFORMATED);
	pop_file.Record_Format (SPACE_DELIMITED);

	if (!pop_file.Read (false)) goto read_error;

	if (str_cmp (pop_file.Record (), "Version 2", 9) == 0 ||
		str_cmp (pop_file.Record (), "VERSION 2", 9) == 0 ||
		str_cmp (pop_file.Record (), "#VERSION 2", 10) == 0) {
		version2 = true;
		if (!pop_file.Read (false)) goto read_error;
	} else {
		version2 = false;
	}

	if (!pop_file.Read (true)) goto read_error;

	pop_file.First_Offset (pop_file.Offset ());

	//---- process the each header record ----

	for (i=0; i < 2; i++) {

		if (i == 0) {

			//---- add default household fields ----

			pop_file.Nest (false);

			if (version2) {
				if (!pop_file.Add_Field ("STATE", Db_Code::INTEGER, 10)) goto add_error;
				if (!pop_file.Add_Field ("COUNTY", Db_Code::INTEGER, 10)) goto add_error;
			}
			if (!pop_file.Add_Field ("TRACT", Db_Code::INTEGER, 10)) goto add_error;
			if (!pop_file.Add_Field ("BLOCK", Db_Code::INTEGER, 10)) goto add_error;
			if (!pop_file.Add_Field ("H", Db_Code::CHAR, 1)) goto add_error;
			if (!pop_file.Add_Field ("HHOLD", Db_Code::INTEGER, 10)) goto add_error;
			if (!pop_file.Add_Field ("PERSONS", Db_Code::INTEGER, 5)) goto add_error;
			if (!pop_file.Add_Field ("VEHICLES", Db_Code::INTEGER, 5)) goto add_error;
			if (!pop_file.Add_Field ("LOCATION", Db_Code::INTEGER, 10)) goto add_error;

		} else {

			//---- add default population fields ----

			pop_file.Nest (true);

			if (!pop_file.Add_Field ("HHOLD", Db_Code::INTEGER, 10, 0, -1, false, true)) goto add_error;
			if (!pop_file.Add_Field ("P", Db_Code::CHAR, 1, 0, -1, false, true)) goto add_error;
			if (!pop_file.Add_Field ("PERSON", Db_Code::INTEGER, 10, 0, -1, false, true)) goto add_error;
		}
		ptr = pop_file.Record ();
		colon_flag = false;

		while (ptr != NULL) {

			ptr = pop_file.Parse_String (ptr, buffer, sizeof (buffer));

			if (*buffer == '\0') break;

			if (!colon_flag) {
				str_ptr = strchr (buffer, ':');
				if (str_ptr != NULL) {
					colon_flag = true;
				}
				continue;
			}
			if ((buffer [0] >= 'a' && buffer [0] <= 'z') || 
				(buffer [0] >= 'A' && buffer [0] <= 'Z') || buffer [0] == '_') {

				if (!pop_file.Add_Field (buffer, INTEGER, 10, 0, -1, false, pop_file.Nest ())) goto add_error;
			} else {
				Error ("Illegal Population Field Name %s", buffer);
			}
		}

		//---- copy fields to output file ----

		if (i == 0) {
			for (j=1; j <= pop_file.Num_Fields (); j++) {
				fld = pop_file.Field (j);

				if (fld->Nested () || !str_cmp (fld->Name (), "H")) continue;

				if (household_file->Field_Number (fld->Name ()) != 0) continue;

				household_file->Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal ());
			}
			household_file->Write_Header ();
			
		} else {
			for (j=1; j <= pop_file.Num_Fields (); j++) {
				fld = pop_file.Field (j);

				if (!fld->Nested () || !str_cmp (fld->Name (), "P")) continue;

				if (population_file->Field_Number (fld->Name ()) != 0) continue;

				population_file->Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal ());
			}
			population_file->Write_Header ();
		}
	}

	//---- set the file format ----

	pop_file.Dbase_Format (SPACE_DELIMITED);
	pop_file.Record_Number (0);
	return;

read_error:
	Error ("Reading Population Header");

add_error:
	Error ("Adding Population Field");
}
