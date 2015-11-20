//*********************************************************
//	Read_Data.cpp - Read the Data File into Memory
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Read_Data
//---------------------------------------------------------

void LocationData::Read_Data (int num)
{
	int id, field, count;

	Db_Index_Array *data;
	Db_Header *file;

	//---- set the file pointers ----

	file = data_files [num];
	if (file == NULL) return;

	data = data_db [num];
	field = data_field [num];

	if (!data->Max_Records (file->Num_Records ())) {
		Error ("Insufficient Memory for Data File Database");
	}

	//---- read the data file ----

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress (1000);

	count = 0;

	while (file->Read ()) {
		Show_Progress ();

		//---- get the join field id ----

		file->Get_Field (field, &id);
		if (id == 0) continue;

		//---- copy the data fields ----

		data->Copy_Fields (file);

		//---- set the record index and location count ----

		data->Put_Field (1, id);
		data->Put_Field (2, 0);

		//---- save the database record ----

		if (!data->Add_Record ()) {
			Error ("Writing Data File Database");
		}
		count++;
	}
	End_Progress ();

	Print (2, "Number of Data #%d Records = %d", num+1, count);
	
	file->Close ();
}
