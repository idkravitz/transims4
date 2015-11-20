//*********************************************************
//	File_Group.cpp - trip purpose control parameters
//*********************************************************

#include "File_Group.hpp"

//---------------------------------------------------------
//	File_Group constructor
//---------------------------------------------------------

File_Group::File_Group (void) : Class_Index (0)
{
	Time_File (NULL);
	Trip_Factor (NULL);

	Purpose (0);
	Mode (0);
	Method (0);
	Type (0);
	SubType (0);
	Org_Wt (0);
	Des_Wt (0);
	Dist_Wt (true);
	Stop_Wt (0);
	Duration (5*60);
	Speed (0);
	Time_Field (0);
	Scaling_Factor (1.0);

	diurnal_data = NULL;
	period = NULL;	
	time_equiv = NULL;
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
	if (num > 0 && num <= time_file->Num_Fields ())  {
		double dvalue;
		time_file->Get_Field (num, &dvalue);
		return (dvalue);
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

	str_fmt (buffer, sizeof (buffer), "Time Period Equivalence #%d", Purpose ());

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


//---- Clear ----

void File_Group::Clear (void)
{
	if (time_file != NULL) {
		delete time_file;
		time_file = NULL;
	}
	if (period != NULL) {
		delete period;
		period = NULL;
	}
	if (diurnal_data != NULL) {
		delete diurnal_data;
		diurnal_data = NULL;
	}
}

//---------------------------------------------------------
//	File_Group_Array constructor
//---------------------------------------------------------

File_Group_Array::File_Group_Array (int max_records) : 
	Class_Array (sizeof (File_Group), max_records, true)
{
}

File_Group_Array::~File_Group_Array (void)
{
	for (File_Group *group = First (); group != NULL; group = Next ()) {
		group->Clear ();
	}
}
