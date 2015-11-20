//*********************************************************
//	Set_Files.cpp - Setup Data Files for User Program
//*********************************************************

#include "ZoneData.hpp"

//---------------------------------------------------------
//	Set_Files
//---------------------------------------------------------

void ZoneData::Set_Files (void)
{
	int i, j, k, nfiles, nfld, key;
	bool binary;

	Db_Field *fld;
	Db_Base *file;
	Db_Index_Array *data;

	//---- create the program data structure ----

	nfiles = 2 + num_data_files + num_polygons;

	data_rec = new Db_Base * [nfiles];
	data_db = new Db_Index_Array * [num_data_files];

	if (data_rec == NULL || data_db == NULL) {
		Error ("Insufficient Memory for %d Database Files", nfiles);
	}
	memset (data_rec, '\0', nfiles * sizeof (Db_Base *));
	memset (data_db, '\0', num_data_files * sizeof (Db_Index_Array *));

	j = 0;
	data_rec [j++] = input_file;
	data_rec [j++] = output_file;

	//---- convert each data file to binary ----
	
	for (i=0; i < num_data_files; i++) {
		file = data_files [i];
		if (file == NULL) continue;

		data = new Db_Index_Array ();

		if (data == NULL) {
			Error ("Creating Database Array");
		}
		data_db [i] = data;

		data_rec [j++] = (Db_Base *) data;

		data->File_ID (file->File_ID ());
		data->File_Type (file->File_Type ());

		nfld = file->Num_Fields ();
		key = data_field [i];
	
		binary = (file->Record_Format () == BINARY);

		fld = file->Field (key);

		data->Add_Field (fld->Name (), INTEGER, 10);
		data->Add_Field ("Z_COUNT", INTEGER, 10);

		for (k=1; k <= nfld; k++) {
			if (k == key) continue;

			fld = file->Field (k);
			if (fld != NULL) {
				data->Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary);
			}
		}
	}

	//---- read the arcview boundary file ----

	for (i=0; i < num_polygons; i++) {
		file = polygon_files [i];
		if (file == NULL) continue;

		//---- initialize the database ----

		data = new Db_Index_Array ();

		if (data == NULL) {
			Error ("Creating Boundary Polygon Array");
		}
		polygon_db [i] = data;

		data_rec [j++] = (Db_Base *) data;

		data->File_ID (file->File_ID ());
		data->File_Type (file->File_Type ());

		nfld = file->Num_Fields ();
		binary = (file->Record_Format () == BINARY);

		data->Add_Field ("INDEX", INTEGER, 10);

		for (k=1; k <= nfld; k++) {
			fld = file->Field (k);
			if (fld != NULL) {
				data->Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), -1, binary);
			}
		}
	}
	nfiles = j;

	Write (1, "Compiling Conversion Script");

	if (Report_Flag (PRINT_SCRIPT)) {
		Header_Number (PRINT_SCRIPT);

		if (!Break_Check (10)) {
			Print (1);
			Page_Header ();
		}
	}
	program.Set_Files (nfiles, data_rec);

	if (!program.Compile (&program_file, Report_Flag (PRINT_SCRIPT))) {
		Error ("Compiling Conversion Script");
	}
	if (Report_Flag (PRINT_STACK)) {
		Header_Number (PRINT_STACK);

		program.Print_Commands (false);
	}
} 
