//*********************************************************
//	File_Group.cpp - trip table control parameters
//*********************************************************

#include "File_Group.hpp"

//---------------------------------------------------------
//	File_Group constructor
//---------------------------------------------------------

File_Group::File_Group (void)
{
	Input_File (NULL);
	Output_File (NULL);
	Initial_File (NULL);

	input_time.Initialize (1, "HOURS");
	output_time.Initialize (1, "HOURS");

	Group (0);
	Field_Number (0);
	Num_Average (0);
	Num_Iteration (0);
	Increment (0);
	Forward (0);
	Backward (0);
	Loop_Flag (false);
	Replicate_Flag (false);
}

//---- Clear ----

void File_Group::Clear (void)
{
	if (input_file != NULL) {
		delete input_file;
		input_file = NULL;
	}
	if (output_file != NULL) {
		delete output_file;
		output_file = NULL;
	}
	if (initial_file != NULL) {
		delete initial_file;
		initial_file = NULL;
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
