//*********************************************************
//	Skim_Group.cpp - zone skim control parameters
//*********************************************************

#include "Skim_Group.hpp"

#include "Utility.hpp"

//---------------------------------------------------------
//	Skim_Group constructor
//---------------------------------------------------------

Skim_Group::Skim_Group (void)
{
	Group (0);
	Num_Zones (0);

	skim_a = new Db_Base (Db_Code::MODIFY, Db_Code::BINARY);
	skim_b = new Db_Base (Db_Code::MODIFY, Db_Code::BINARY);

	skim_data = new Index_Array ();

	skim_file = new Skim_File ();
	time_equiv = NULL;

	key_a = key_b = -1;
	dump_field = 0;
	mem_factor = 1.0;
}

//---- Skim_Filename ----

bool Skim_Group::Skim_Filename (char *filename)
{
	int i, num_field;
	bool binary_flag;

	Db_Field *fld;

	if (skim_file == NULL || skim_a == NULL || skim_b == NULL) return (false);

	//---- open the skim file ----

	char buffer [FIELD_BUFFER];

	str_fmt (buffer, sizeof (buffer), "Zone Skim File #%d", Group ());

	skim_file->File_Type (buffer);
	skim_a->File_Type (buffer);
	skim_b->File_Type (buffer);

	skim_file->File_ID (str_fmt (buffer, sizeof (buffer), "Skim%d", Group ()));
	skim_a->File_ID (str_fmt (buffer, sizeof (buffer), "Skim%dA", Group ()));
	skim_b->File_ID (str_fmt (buffer, sizeof (buffer), "Skim%dB", Group ()));

	if (!skim_file->Open (filename)) return (false);

	if (!skim_a->Add_Field ("KEY", Db_Code::INTEGER, sizeof (int), 0, 0, true)) return (false);
	if (!skim_b->Add_Field ("KEY", Db_Code::INTEGER, sizeof (int), 0, 0, true)) return (false);

	num_field = skim_file->Num_Fields ();
	binary_flag = (skim_file->Record_Format () == Db_Code::BINARY);

	for (i=1; i <= num_field; i++) {
		if (i == skim_file->Origin_Field ()) continue;
		if (i == skim_file->Destination_Field ()) continue;
		if (i == skim_file->Period_Field ()) continue;

		fld = skim_file->Field (i); 
		if (fld == NULL) continue;
		if (!str_cmp (fld->Name (), "NOTES")) continue;

		if (!skim_a->Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), END_OF_RECORD, binary_flag)) return (false);
		if (!skim_b->Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Decimal (), END_OF_RECORD, binary_flag)) return (false);
	}
	dump_field = skim_a->Optional_Field ("TIME", "DRIVE", "TRANSIT", "WALK", "COST");
	return (true);
}

//---- Skim_Format ----

bool Skim_Group::Skim_Format (char *format)
{
	if (skim_file == NULL) return (false);

	skim_file->Dbase_Format (format);
	return (true);
}

//---- Time_Period_File ----

bool Skim_Group::Time_Period_File (char *filename)
{
	if (time_equiv != NULL) return (false);

	time_equiv = new Time_Equiv ();
	if (time_equiv == NULL) return (false);

	char buffer [FIELD_BUFFER];

	str_fmt (buffer, sizeof (buffer), "Time Period Equivalence #%d", Group ());

	time_equiv->File_Type (buffer);

	if (!time_equiv->Open (filename)) return (false);

	return (true);
}

//---- Read_Skim ----

bool Skim_Group::Read_Skim (bool equiv_report)
{	
	if (skim_file == NULL || skim_a == NULL || skim_b == NULL) return (false);

	int num_period, org, des, period, key;

	//---- read the time period equivalence ----

	if (time_equiv != NULL) {
		if (!time_equiv->Read (equiv_report)) return (false);

		num_period = time_equiv->Num_Periods ();

		skim_key.Set_Key (num_period);
	} else {
		num_period = 1;
	}

	//---- set the data size ----

	skim_data->Data_Size (skim_a->Max_Size ());

	//---- allocate memory ----

	if (zones > 0) {
		skim_data->Max_Records ((int) (zones * zones * num_period * mem_factor + 0.5));
	}

	//---- read the zone skim file ----

	exe->Show_Message ("Reading %s -- Record", skim_file->File_Type ());
	if (zones > 0) {
		exe->Set_Progress (100000);
	} else {
		exe->Set_Progress (10000);
	}
	while (skim_file->Read ()) {
		exe->Show_Progress ();

		org = skim_file->Origin ();
		des = skim_file->Destination ();
		period = skim_file->Period ();
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
		skim_a->Copy_Fields (skim_file);
		skim_a->Put_Field (1, key);

		if (!skim_data->Add (skim_a->Record ())) {
			exe->Error ("Writing Zone Skim Data #%d", Group ());
		}		
	}
	exe->End_Progress ();
	skim_file->Close ();

	exe->Print (2, "Number of %s Records = %d", skim_file->File_Type (), exe->Progress_Count ());

	return (skim_data->Optimize ());
}

//---- Get_Record_A ----

bool Skim_Group::Get_Record_A (int org, int des, int time) 
{
	int key;

	if (skim_data == NULL || skim_a == NULL) return (false);

	if (time_equiv != NULL) {
		time = time_equiv->Period (time);
		if (time == 0) goto null_record;
	} else {
		time = 1;
	}
	key = skim_key.Key (org, des, time);

	if (key == key_a) return (true);
	key_a = key;

	if (key > 0) {
		char *rec = (char *) skim_data->Get (&key);

		if (rec != NULL) {
			return (skim_a->Record (rec));
		}
	}

null_record:
	skim_a->Reset_Record ();
	return (false);
}


//---- Get_Record_B ----

bool Skim_Group::Get_Record_B (int org, int des, int time) 
{
	int key;

	if (skim_data == NULL || skim_b == NULL) return (false);

	if (time_equiv != NULL) {
		time = time_equiv->Period (time);
		if (time == 0) goto null_record;
	} else {
		time = 1;
	}
	key = skim_key.Key (org, des, time);

	if (key == key_b) return (true);
	key_b = key;

	if (key > 0) {
		char *rec = (char *) skim_data->Get (&key);

		if (rec != NULL) {
			return (skim_b->Record (rec));
		}
	}

null_record:
	skim_b->Reset_Record ();
	return (false);
}

//---- Clear ----

void Skim_Group::Clear (void)
{
	if (skim_data != NULL) {
		delete skim_data;
		skim_data = NULL;
	}
	if (skim_a != NULL) {
		delete skim_a;
		skim_a = NULL;
	}
	if (skim_b != NULL) {
		delete skim_b;
		skim_b = NULL;
	}
	if (skim_file != NULL) {
		delete skim_file;
		skim_file = NULL;
	}
}

//---------------------------------------------------------
//	Skim_Group_Array constructor
//---------------------------------------------------------

Skim_Group_Array::Skim_Group_Array (int max_records) : 
	Data_Array (sizeof (Skim_Group), max_records)
{
}

Skim_Group_Array::~Skim_Group_Array (void)
{
	for (Skim_Group *group = First (); group != NULL; group = Next ()) {
		group->Clear ();
	}
}

