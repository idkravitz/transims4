//*********************************************************
//	File_Group.cpp - trip table control parameters
//*********************************************************

#include "File_Group.hpp"

//---------------------------------------------------------
//	File_Group constructor
//---------------------------------------------------------

File_Group::File_Group (void)
{
	Trip_File (NULL);
	Time_File (NULL);
	Trip_Factor (NULL);
	Script_File (NULL);

	Purpose (0);
	Mode (0);
	Method (0);
	Duration (0);
	Type (0);
	SubType (0);
	Org_Wt (0);
	Des_Wt (0);
	Dist_Wt (true);
	Speed (0);
	Time_Field (0);
	Scaling_Factor (1.0);

	period = NULL;	
	time_equiv = NULL;
	program = NULL;
	header = NULL;
}

//---- Time Field ----

bool File_Group::Time_Field (char *field)
{
	if (time_file == NULL || field == NULL) return (false);

	time_file->Share_Field (time_file->Field_Number (field));

	return (Time_Field () != 0);
}

//---- Share ----

double File_Group::Share (int num) 
{
	if (num > 0 && num <= Num_Shares ())  {
		Db_Field *fld = header->Field (num);

		if (fld != NULL) {
			double dvalue;
			time_file->Get_Field (fld->Name (), &dvalue);
			return (dvalue);
		}
	}
	return (0.0);
}

//---- Time_Periods ----

bool File_Group::Time_Periods (char *str) 
{
	period = new Time_Range ();
	if (period == NULL) return (false);

	return (period->Add_Ranges (str));
}

//---- Time_Equiv_File ----

bool File_Group::Time_Equiv_File (char *filename)
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

//---- Factor_Periods ----

int File_Group::Factor_Periods (bool equiv_report)
{
	if (time_equiv == NULL) return (0);

	int num_period = time_equiv->Num_Periods ();

	if (num_period == 0) {
		if (!time_equiv->Read (equiv_report)) return (0);

		num_period = time_equiv->Num_Periods ();
	}
	return (num_period);
}

//---- Execute ----

int File_Group::Execute (void) 
{
	if (program != NULL) {
		return (program->Execute ());
	}
	return (1);
}

//---- Compile ----

bool File_Group::Compile (bool report) 
{
	if (trip_file == NULL || time_file == NULL || script_file == NULL) return (false);

	int i, offset;
	Db_Field *fld_ptr, fld_rec;

	header = new Db_Base (Db_Code::READ, Db_Code::BINARY);
	header->File_ID (time_file->File_ID ());
	header->File_Type (time_file->File_Type ());
	offset = 0;

	for (i=1; i <= time_file->Num_Fields (); i++) {
		if (i == time_file->Start_Field () || i == time_file->End_Field ()) continue;
	
		fld_ptr = time_file->Field (i);

		header->Add_Field (fld_ptr->Name (), Db_Code::INTEGER, sizeof (int), 0, offset, true);

		offset += sizeof (int);
	}
	for (i=1; i <= header->Num_Fields (); i++) {
		header->Put_Field (i, i);
	}
	program = new User_Program (2, trip_file, header);
	if (program == NULL) return (false);

	return (program->Compile (script_file, report));
}

//---- Clear ----

void File_Group::Clear (void)
{
	if (trip_file != NULL) {
		delete trip_file;
		trip_file = NULL;
	}
	if (time_file != NULL) {
		delete time_file;
		time_file = NULL;
	}
	if (period != NULL) {
		delete period;
		period = NULL;
	}
	if (program != NULL) {
		delete program;
		program = NULL;
	}
}

//---------------------------------------------------------
//	File_Group_Array constructor
//---------------------------------------------------------

File_Group_Array::File_Group_Array (int max_records) : 
	Data_Array (sizeof (File_Group), max_records)
{
}

File_Group_Array::~File_Group_Array (void)
{
	for (File_Group *group = First (); group != NULL; group = Next ()) {
		group->Clear ();
	}
}
