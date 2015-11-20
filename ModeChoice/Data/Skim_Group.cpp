//*********************************************************
//	Skim_Group.cpp - zone skim control parameters
//*********************************************************

#include "Skim_Group.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Skim_Group constructor
//---------------------------------------------------------

Skim_Group::Skim_Group (int group)
{
	Group (group);
	Num_Zones (0);
	last_key = -1;
	time_flag = false;
	mem_factor = 1.0;

	skim.File_Access (Db_Code::MODIFY);
	skim.Dbase_Format (Db_Code::BINARY);
}

//---- Skim_Filename ----

bool Skim_Group::Skim_Filename (char *filename, char *format)
{
	int i, num_field;
	bool binary_flag;

	Db_Field *fld;

	if (format != NULL) {
		skim_file.Dbase_Format (format);
	}

	//---- open the skim file ----

	char buffer [FIELD_BUFFER];

	str_fmt (buffer, sizeof (buffer), "Zone Skim File #%d", Group ());

	skim_file.File_Type (buffer);
	skim.File_Type (buffer);

	str_fmt (buffer, sizeof (buffer), "Skim%d", Group ());

	skim_file.File_ID (buffer);
	skim.File_ID (buffer);

	if (!skim_file.Open (filename)) return (false);

	if (!skim.Add_Field ("KEY", Db_Code::INTEGER, sizeof (int), 0, 0, true)) return (false);

	num_field = skim_file.Num_Fields ();
	binary_flag = (skim_file.Record_Format () == Db_Code::BINARY);

	for (i=1; i <= num_field; i++) {
		if (i == skim_file.Origin_Field ()) continue;
		if (i == skim_file.Destination_Field ()) continue;
		if (i == skim_file.Period_Field ()) continue;

		fld = skim_file.Field (i); 
		if (fld == NULL) continue;
		if (!str_cmp (fld->Name (), "NOTES")) continue;

		if (!skim.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), END_OF_RECORD, binary_flag)) return (false);
	}
	return (true);
}

//---- Time_Period_File ----

bool Skim_Group::Time_Period_File (char *filename)
{
	char buffer [FIELD_BUFFER];
	time_flag = false;

	str_fmt (buffer, sizeof (buffer), "Time Period Equivalence #%d", Group ());

	time_equiv.File_Type (buffer);

	if (!time_equiv.Open (filename)) return (false);

	time_flag = true;

	return (true);
}

//---- Read_Skim ----

bool Skim_Group::Read_Skim (bool equiv_report)
{	
	int num_period, org, des, period, key;

	//---- read the time period equivalence ----

	if (time_flag) {
		if (!time_equiv.Read (equiv_report)) return (time_flag = false);

		num_period = time_equiv.Num_Periods ();

		skim_key.Set_Key (num_period);
	} else {
		num_period = 1;
	}

	//---- set the data size ----

	skim_data.Data_Size (skim.Max_Size ());

	//---- allocate memory ----

	if (zones > 0) {
		skim_data.Max_Records ((int) (zones * zones * num_period * mem_factor + 0.5));
	}

	//---- read the zone skim file ----

	exe->Show_Message ("Reading %s -- Record", skim_file.File_Type ());
	exe->Set_Progress (10000);
	
	while (skim_file.Read ()) {
		exe->Show_Progress ();

		org = skim_file.Origin ();
		des = skim_file.Destination ();
		period = skim_file.Period ();
		if (period == 0) period = 1;

		if (period < 1 || period > num_period) {
			exe->Warning ("Skim %d Time Period %d is Out of Range (1..%d) (record=%d)", 
				Group (), period, num_period, exe->Progress_Count ());
			continue;
		}
		key = skim_key.Key (org, des, period);
		if (key == 0) {
			exe->Warning ("Skim %d Index Key %d-%d-%d is Out of Range (record=%d)", 
				Group (), org, des, period, exe->Progress_Count ());
			continue;
		}
		skim.Copy_Fields (&skim_file);
		skim.Put_Field (1, key);

		if (!skim_data.Add (skim.Record ())) {
			exe->Error ("Writing Zone Skim Data #%d", Group ());
		}		
	}
	exe->End_Progress ();
	skim_file.Close ();

	exe->Print (2, "Number of %s Records = %d", skim_file.File_Type (), exe->Progress_Count ());

	return (skim_data.Optimize ());
}

//---- Get_Record ----

bool Skim_Group::Get_Record (int org, int des, int time) 
{
	int key;

	if (time_flag) {
		time = time_equiv.Period (time);
		if (time == 0) goto null_record;
	} else {
		time = 1;
	}
	key = skim_key.Key (org, des, time);

	if (key == last_key) return (true);
	last_key = key;

	if (key > 0) {
		char *rec = (char *) skim_data.Get (&key);

		if (rec != NULL) {
			return (skim.Record (rec));
		}
	}

null_record:
	skim.Reset_Record ();
	return (false);
}

//---------------------------------------------------------
//	Skim_Group_Array constructor
//---------------------------------------------------------

Skim_Group_Array::Skim_Group_Array (int max_records) : 
	Data_Pointer_Array (max_records)
{
}

void Skim_Group_Array::Delete_Pointers (void)
{
	for (Skim_Group *group = First (); group != NULL; group = Next ()) {
		delete group;
	}
	Data_Array::Reset ();
}
