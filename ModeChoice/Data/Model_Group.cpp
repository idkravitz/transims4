//*********************************************************
//	Model_Group.cpp - mode choice models by purpose
//*********************************************************

#include "Model_Group.hpp"
#include "Data_Range.hpp"

//---------------------------------------------------------
//	Model_Group constructor
//---------------------------------------------------------

Model_Group::Model_Group (int _group)
{
	group = _group;
}

bool Model_Group::Script_File (char *type, char *name)
{
	char buffer [STRING_BUFFER];

	str_fmt (buffer, sizeof (buffer), "%s Mode Model #%d", type, group);
	script_file.File_Type (buffer);

	return (script_file.Open (name));
}

bool Model_Group::Compile (int count, Db_Base **files, bool report) 
{
	if (!program.Set_Files (count, files)) return (false);

	return (program.Compile (&script_file, report));
}

//---------------------------------------------------------
//	Model_Group_Array constructor
//---------------------------------------------------------

Model_Group_Array::Model_Group_Array (int max_records) : 
	Data_Pointer_Array (max_records)
{
	memset (purpose_map, '\0', sizeof (purpose_map));
}

void Model_Group_Array::Delete_Pointers (void)
{
	for (Model_Group *group = First (); group != NULL; group = Next ()) {
		delete group;
	}
	Data_Array::Reset ();
}

Model_Group * Model_Group_Array::Model (int purpose)
{
	if (purpose < 0 || purpose >= MAX_PURPOSE) return (false);

	return (Record (purpose_map [purpose]));
}

bool Model_Group_Array::Purpose_Range (char *str_ptr)
{
	int i, index;
	Data_Range purpose_range;
	Range_Data *range_ptr;

	if (!purpose_range.Add_Ranges (str_ptr)) {
		exe->Write (1, "Purpose Range %s is Illogical", str_ptr);
		return (false);
	}
	index = Num_Records ();

	for (range_ptr = purpose_range.First (); range_ptr; range_ptr = purpose_range.Next ()) {
		for (i=range_ptr->Low (); i <= range_ptr->High (); i++) {
			if (i < 0 || i >= MAX_PURPOSE) {
				exe->Write (1, "Purpose %d is Out of Range (0..%d)", i, MAX_PURPOSE-1);
				return (false);
			}
			if (purpose_map [i] != 0) {
				Model_Group * model_ptr = Model (i);

				exe->Error ("Purpose %d is already assigned to Model #%d", i, model_ptr->Group ());
				return (false);
			}
			purpose_map [i] = index;
		}
	}
	return (true);
}
