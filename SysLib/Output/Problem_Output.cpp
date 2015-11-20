//*********************************************************
//	Problem_Output.cpp - Output Interface Class
//*********************************************************

#include "Problem_Output.hpp"

#include "Network_Service.hpp"
#include "Utility.hpp"

char * Problem_Output::OUTPUT_PROBLEM_TYPE_x		= "OUTPUT_PROBLEM_TYPE_*";
char * Problem_Output::OUTPUT_PROBLEM_FILE_x		= "OUTPUT_PROBLEM_FILE_*";
char * Problem_Output::OUTPUT_PROBLEM_FORMAT_x		= "OUTPUT_PROBLEM_FORMAT_*";
char * Problem_Output::OUTPUT_PROBLEM_FILTER_x		= "OUTPUT_PROBLEM_FILTER_*";
char * Problem_Output::OUTPUT_PROBLEM_TIME_FORMAT_x	= "OUTPUT_PROBLEM_TIME_FORMAT_*";
char * Problem_Output::OUTPUT_PROBLEM_INCREMENT_x	= "OUTPUT_PROBLEM_INCREMENT_*";
char * Problem_Output::OUTPUT_PROBLEM_TIME_RANGE_x	= "OUTPUT_PROBLEM_TIME_RANGE_*";
char * Problem_Output::OUTPUT_PROBLEM_LINK_RANGE_x	= "OUTPUT_PROBLEM_LINK_RANGE_*";

//---------------------------------------------------------
//	Problem_Output constructor
//---------------------------------------------------------

Problem_Output::Problem_Output (void) : Problem_Service ()
{
	output_data.Initialize (sizeof (Output_Data), 10);

	output_flag = false;
	problem = link_dir = 0;
}

//---------------------------------------------------------
//	Problem_Output destructor
//---------------------------------------------------------

Problem_Output::~Problem_Output (void)
{
	Close ();
}

//---------------------------------------------------------
//	Add_Keys
//---------------------------------------------------------

void Problem_Output::Add_Keys (void)
{
	char *keys [] = {
		OUTPUT_PROBLEM_TYPE_x,
		OUTPUT_PROBLEM_FILE_x,
		OUTPUT_PROBLEM_FORMAT_x,
		OUTPUT_PROBLEM_FILTER_x,
		OUTPUT_PROBLEM_TIME_FORMAT_x,
		OUTPUT_PROBLEM_INCREMENT_x,
		OUTPUT_PROBLEM_TIME_RANGE_x,
		OUTPUT_PROBLEM_LINK_RANGE_x,
		NULL
	};

	if (exe != NULL) {
		exe->Key_List (keys);
	}
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

void Problem_Output::Read_Control (int steps_per_second)
{
	int i, j;
	Problem_Type type;

	if (exe == NULL || steps_per_second < 1) return;

	//---- read the output delay data ----

	int num = exe->Highest_Control_Group (OUTPUT_PROBLEM_FILE_x, 0);

	if (num != 0) {
		Output_Data *output;
		char *str_ptr, buffer [FIELD_BUFFER];
		int increment;

		//---- allocate array space ----

		if (!output_data.Max_Records (num)) {
			exe->Error ("Insufficient Memory for %d Output Summary Groups", num);
		}

		//---- process each file ----

		for (i=1; i <= num; i++) {

			str_ptr = exe->Get_Control_String (OUTPUT_PROBLEM_FILE_x, i);

			if (str_ptr == NULL) continue;

			char *name_ptr = exe->Project_Filename (str_ptr);

			output = (Output_Data *) output_data.New_Record ();

			output->time_range = new Time_Range (steps_per_second);
			output->link_range = NULL;
			output->problem_data = NULL;

			output->file = new Problem_Link_File (Db_Code::CREATE);

			exe->Break_Check (9);
			exe->Print (1);	
			str_fmt (buffer, sizeof (buffer), "Output Problem File #%d", i);

			output->file->File_Type (buffer);

			//---- get the file type ----

			str_ptr = exe->Get_Control_String (OUTPUT_PROBLEM_TYPE_x, i);

			memset (output->type, '\0', sizeof (output->type));
			output->ntype = 0;

			if (str_ptr != NULL) {
				exe->Print (1, "Output Problem Type #%d = %s", i, str_ptr);

				while (str_ptr != NULL) {
					str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));

					if (buffer [0] != '\0') {
						type = Problem_Code (buffer);
						output->ntype++;
						output->type [type] = output->ntype;
					}
				}
			} else {
				for (j=0; j < MAX_PROBLEM; j++) {
					output->ntype++;
					output->type [j] = output->ntype;
				}
			}

			//---- set the output format ----

			str_ptr = exe->Get_Control_String (OUTPUT_PROBLEM_FORMAT_x, i);

			if (str_ptr != NULL) {
				output->file->Dbase_Format (str_ptr);
			}
			output->file->Open (name_ptr);

			//---- get the filter ----

			str_ptr = exe->Get_Control_String (OUTPUT_PROBLEM_FILTER_x, i);

			if (str_ptr != NULL) {
				output->filter = atoi (str_ptr);

				if (output->filter < 1) {
					exe->Error ("Output Problem Filter %d is Out of Range (>= 1)", output->filter);
				}
			} else {
				output->filter = 100;
			}

			exe->Print (1, "Output Problem Filter #%d = %d problem%c", i, output->filter,
				((output->filter != 1) ? 's' : ' '));

			//---- get the time format ----

			str_ptr = exe->Get_Control_String (OUTPUT_PROBLEM_TIME_FORMAT_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Format (str_ptr)) {
					exe->Error ("Output Problem Time Format %s was Unrecognized", str_ptr);
				}
				exe->Print (1, "Output Problem Time Format #%d = %s", i, output->time_range->Time_Code ());
			}

			//---- get the increment ----

			str_ptr = exe->Get_Control_String (OUTPUT_PROBLEM_INCREMENT_x, i);

			if (str_ptr != NULL) {
				output->time_range->Increment (str_ptr);
				increment = output->time_range->Increment ();

				if (increment < 1) {
					exe->Error ("Output Problem Increment %s is Out of Range", 
						output->time_range->Format_Duration (increment));
				}
			} else {
				output->time_range->Increment ("24:00");
				increment = output->time_range->Increment ();
			}
			exe->Print (1, "Output Problem Increment #%d = %s", i, 
				output->time_range->Format_Duration (increment));

			if (output->time_range->Format () == Time_Step::SECONDS) {
				exe->Print (0, " second%c", (increment != output->time_range->Duration (1) ? 's' : ' '));
			} else if (output->time_range->Format () == Time_Step::HOURS) {
				exe->Print (0, " hour%c", (increment != output->time_range->Duration (1.0) ? 's' : ' '));
			}

			//---- get the time range ----

			str_ptr = exe->Get_Control_String (OUTPUT_PROBLEM_TIME_RANGE_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Problem Time Range", str_ptr);
				}
				exe->Print (1, "Output Problem Time Range #%d = ", i);

				for (j=1; j <= output->time_range->Num_Ranges (); j++) {
					str_ptr = output->time_range->Range_Format (j);

					if (str_ptr != NULL) {
						exe->Print (0, "%s ", str_ptr);
					}
				}
				if (output->time_range->Format () == Time_Step::SECONDS) {
					exe->Print (0, "seconds");
				} else if (output->time_range->Format () == Time_Step::HOURS) {
					exe->Print (0, "hours");
				}
			} else {
				output->time_range->Add_Ranges ("All");
			}

			//---- get the link range ----

			str_ptr = exe->Get_Control_String (OUTPUT_PROBLEM_LINK_RANGE_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Problem Link Range #%d = %s", i, str_ptr);

				output->link_range = new Data_Range ();

				if (!output->link_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Problem Link Range", str_ptr);
				}
			}

			//---- initialize the summary data ----

			output->problem_data = new Data_Array (output->ntype * sizeof (int));

			if (output->problem_data == NULL) {
				exe->Error ("Creating Problem Data");
			}
			output->step = -1;

			//---- save the output record ----

			if (!output_data.Add ()) {
				exe->Error ("Adding Problem Output Group");
			}
			output_flag = true;
		}
	}
}

//---------------------------------------------------------
//	In_Range
//---------------------------------------------------------

bool Problem_Output::In_Range (int step, int dir)
{
	if (output_flag) {
		Output_Data *output;

		for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {
			if (output->time_range->In_Range (step)) {
				if (dir > 0 && output->link_range != NULL && output->link_range->Num_Ranges () > 0) {
					Network_Service *net = (Network_Service *) exe;
					Dir_Data *dir_ptr = net->dir_array->Record (dir);
					if (dir_ptr != NULL) {
						Link_Data *link_ptr = net->link_array->Record (dir_ptr->Link ());

						if (link_ptr != NULL) {
							if (!output->link_range->In_Range (link_ptr->Link ())) continue;
						}
					}
				}
				return (true);
			}
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

void Problem_Output::Output_Check (int step)
{
	if (!output_flag) return;

	Output_Data *output;

	for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {
		if (output->time_range->In_Range (step)) {

			//---- initialize the link data ----

			if (output->step < 0) {
				Network_Service *net = (Network_Service *) exe;
				if (!output->problem_data->Num_Records (net->dir_array->Num_Records ())) {
					exe->Error ("Insufficient Memory for Problem Data");
				}
			} else if (output->time_range->At_Increment (step)) {
				Write_Problem (step, output);
			}
			output->step = step;
		}
	}
}

//---------------------------------------------------------
//	Summarize
//---------------------------------------------------------

void Problem_Output::Summarize (int step)
{
	if (output_flag) {
		int *ptr, i, j;

		if (link_dir <= 0) {
			exe->Error ("Problem Output Link Undefined");
		}

		Output_Data *output = (Output_Data *) output_data.Record ();

		for (; output; output = (Output_Data *) output_data.Next ()) {

			if (output->time_range->In_Range (step)) {
				if (output->link_range != NULL && output->link_range->Num_Ranges () > 0) {
					Network_Service *net = (Network_Service *) exe;
					Dir_Data *dir_ptr = net->dir_array->Record (link_dir);
					if (dir_ptr != NULL) {
						Link_Data *link_ptr = net->link_array->Record (dir_ptr->Link ());

						if (link_ptr != NULL) {
							if (!output->link_range->In_Range (link_ptr->Link ())) continue;
						}
					}
				}
				i = output->type [TOTAL_PROBLEM];
				j = output->type [problem];

				if (i > 0 || j > 0) {
					ptr = (int *) output->problem_data->Record (link_dir);

					if (ptr != NULL) {
						if (i > 0) ptr [--i]++;
						if (j > 0) ptr [--j]++;
					}
				}
			}
		}
	}
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

void Problem_Output::Close (void)
{
	if (!output_flag) return;

	Output_Data *output;

	for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {
		if (output->step >= 0 && !output->time_range->At_Increment (output->step)) {
			output->step = (output->step / output->time_range->Increment () + 1) * output->time_range->Increment ();

			Write_Problem (output->step, output);
		}
		if (output->file != NULL) {
			output->file->Close ();
			delete output->file;
			output->file = NULL;
		}
		if (output->time_range != NULL) {
			delete output->time_range;
			output->time_range = NULL;
		}
		if (output->link_range != NULL) {
			delete output->link_range;
			output->link_range = NULL;
		}
		if (output->problem_data != NULL) {
			delete output->problem_data;
			output->problem_data = NULL;
		}
		output->step = -1;
	}
	output_flag = false;
}

//---------------------------------------------------------
//	Write_Problem
//---------------------------------------------------------

void Problem_Output::Write_Problem (int step, Output_Data *output)
{
	int dir, i, j, *ptr;
	bool first;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Network_Service *net = (Network_Service *) exe;

	dir = 0;

	for (ptr = (int *) output->problem_data->First (); ptr; ptr = (int *) output->problem_data->Next ()) {
		dir++;
		first = true;

		for (i=0; i < MAX_PROBLEM; i++) {
			j = output->type [i];

			if (j > 0 && ptr [--j] >= output->filter) {
				if (first) {
					dir_ptr = net->dir_array->Record (dir);
					link_ptr = net->link_array->Record (dir_ptr->Link ());

					output->file->Link (link_ptr->Link ());

					if (output->file->Dbase_Format () == Db_Code::VERSION3) {
						Node_Data *node_ptr;

						if (dir_ptr->Dir () > 0) {
							node_ptr = net->node_array->Record (link_ptr->Anode ());
						} else {
							node_ptr = net->node_array->Record (link_ptr->Bnode ());
						}
						output->file->Node (node_ptr->Node ());
					} else {
						output->file->Node (dir_ptr->Dir ());
					}
					first = false;
				}
				output->file->Start_Time (output->time_range->Format_Step (step - output->time_range->Increment ()));
				output->file->End_Time (output->time_range->Format_Step (step));

				output->file->Problem (i);
				output->file->Count (ptr [j]);

				if (!output->file->Write ()) {
					exe->Error ("Writing Problem Link File");
				}
			}
		}
		memset (ptr, '\0', output->ntype * sizeof (int));
	}
}

