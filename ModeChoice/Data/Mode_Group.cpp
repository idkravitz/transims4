//*********************************************************
//	Mode_Group.cpp - mode database parameters
//*********************************************************

#include "Mode_Group.hpp"

//---------------------------------------------------------
//	Mode_Group constructor
//---------------------------------------------------------

Mode_Group::Mode_Group (int group)
{
	Group (group);
	mode = 0;
	field = -1;

	str_fmt (label, sizeof (label), "Mode%d", group);

	data.File_ID (label);
	data.File_Access (Db_Code::MODIFY);
	data.Dbase_Format (Db_Code::BINARY);
}
	
void Mode_Group::Label (char *value)
{ 
	str_cpy (label, sizeof (label), value, sizeof (label)-1); 
	data.File_ID (label);
}

bool Mode_Group::Add_Field (char *name, Db_Code::Field_Type type, int size, int decimal)
{
	if (data.Field_Number (name) != 0) {
		exe->Error ("Mode Data Field %s already Exists", name);
	}
	return (data.Add_Field (name, type, size, decimal)); 
}

double Mode_Group::Share (void)
{ 
	if (field < 0) {
		field = data.Optional_Field ("SHARE", "PROBABILITY", "PERCENT", "PROB", "UTILITY");
	}
	if (field > 0) {
		double dvalue;
		data.Get_Field (field, &dvalue); 
		return (dvalue); 
	} else {
		return (0.0);
	}
}

//---------------------------------------------------------
//	Mode_Group_Array constructor
//---------------------------------------------------------

Mode_Group_Array::Mode_Group_Array (int max_records) : 
	Data_Pointer_Array (max_records)
{
}

void Mode_Group_Array::Delete_Pointers (void)
{
	for (Mode_Group *group = First (); group != NULL; group = Next ()) {
		delete group;
	}
	Data_Array::Reset ();
}
