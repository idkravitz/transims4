//*********************************************************
//	Purpose_Group.cpp - activity purpose control parameters
//*********************************************************

#include "Purpose_Group.hpp"

//---------------------------------------------------------
//	Purpose_Group constructor
//---------------------------------------------------------

Purpose_Group::Purpose_Group (void)
{
	Anchor (false);
	Constraint (Demand_Code::NO_CONSTRAINT);
	Script_File (NULL);
	program = NULL;
	purpose = NULL;

	max_zone = 0;
	balance_field = 0;
	balance_factor = NULL;

	Zone_Field (0);
	Zone_Factor (0.0);
	Location_Field (0);
	Location_Factor (0.0);
	Time_Skim (0);
	Time_Field (0);
	Skim_Field ("");

	for (int m=1; m < Network_Code::MAX_MODE; m++) {
		Distance_Factor (m, 0.0);
		Time_Factor (m, 0.0);
	}
}

bool Purpose_Group::Purpose_Range (char *str) 
{
	purpose = new Data_Range ();
	if (purpose == NULL) return (false);

	return (purpose->Add_Ranges (str));
}

int Purpose_Group::Max_Purpose (void) 
{
	int max_purpose = 0;

	if (purpose != NULL) {
		Range_Data *range_ptr;

		for (range_ptr = purpose->First (); range_ptr; range_ptr = purpose->Next ()) {
			if (range_ptr->High () > max_purpose) max_purpose = range_ptr->High ();
		}
	}
	return (max_purpose);
}

bool Purpose_Group::Max_Zone (int value)
{
	if (value < 1) {
		if (balance_factor != NULL) {
			delete balance_factor;
			balance_factor = NULL;
		}
	} else {
		max_zone = value;
		balance_factor = new double [value + 1]; 

		if (balance_factor == NULL) return (false);

		for (int i=0; i <= max_zone; i++) {
			balance_factor [i] = 1.0;
		}
	}
	return (true); 
}

int Purpose_Group::Execute (void) 
{
	if (program != NULL) {
		return (program->Execute ());
	}
	return (0);
}

bool Purpose_Group::Compile (int count, Db_Base **files, bool report) 
{
	if (script_file == NULL) return (false);

	program = new User_Program (count, files);
	if (program == NULL) return (false);

	return (program->Compile (script_file, report));
}

void Purpose_Group::Clear (void)
{
	if (purpose != NULL) {
		delete purpose;
		purpose = NULL;
	}
	if (program != NULL) {
		delete program;
		program = NULL;
	}
	if (balance_factor != NULL) {
		delete balance_factor;
		balance_factor = NULL;
	}
}

//---------------------------------------------------------
//	Purpose_Group_Array constructor
//---------------------------------------------------------

Purpose_Group_Array::Purpose_Group_Array (int max_records) : 
	Data_Array (sizeof (Purpose_Group), max_records)
{
}

Purpose_Group_Array::~Purpose_Group_Array (void)
{
	for (Purpose_Group *group = First (); group != NULL; group = Next ()) {
		group->Clear ();
	}
}

int Purpose_Group_Array::Max_Purpose (void) 
{
	int max, max_purpose;
	Purpose_Group *group_ptr;

	max_purpose = 0;

	for (group_ptr = First (); group_ptr; group_ptr = Next ()) {
		max = group_ptr->Max_Purpose ();
		if (max > max_purpose) max_purpose = max;
	}
	return (max_purpose);
}
