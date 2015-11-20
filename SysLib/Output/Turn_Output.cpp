//*********************************************************
//	Turn_Output.cpp - Output Interface Class
//*********************************************************

#include "Turn_Output.hpp"

#include "Network_Service.hpp"
#include "Utility.hpp"

char * Turn_Output::OUTPUT_TURN_FILE_x			= "OUTPUT_TURN_FILE_*";
char * Turn_Output::OUTPUT_TURN_FORMAT_x		= "OUTPUT_TURN_FORMAT_*";
char * Turn_Output::OUTPUT_TURN_FILTER_x		= "OUTPUT_TURN_FILTER_*";
char * Turn_Output::OUTPUT_TURN_TIME_FORMAT_x	= "OUTPUT_TURN_TIME_FORMAT_*";
char * Turn_Output::OUTPUT_TURN_INCREMENT_x		= "OUTPUT_TURN_INCREMENT_*";
char * Turn_Output::OUTPUT_TURN_TIME_RANGE_x	= "OUTPUT_TURN_TIME_RANGE_*";
char * Turn_Output::OUTPUT_TURN_NODE_RANGE_x	= "OUTPUT_TURN_NODE_RANGE_*";

//---------------------------------------------------------
//	Turn_Output constructor
//---------------------------------------------------------

Turn_Output::Turn_Output (void) : Static_Service ()
{
	output_data.Initialize (sizeof (Output_Data), 10);

	output_flag = false;
	node = in_link = out_link = 0;
}

//---------------------------------------------------------
//	Turn_Output destructor
//---------------------------------------------------------

Turn_Output::~Turn_Output (void)
{
	Close ();
}

//---------------------------------------------------------
//	Add_Keys
//---------------------------------------------------------

void Turn_Output::Add_Keys (void)
{
	char *keys [] = {
		OUTPUT_TURN_FILE_x,
		OUTPUT_TURN_FORMAT_x,
		OUTPUT_TURN_FILTER_x,
		OUTPUT_TURN_TIME_FORMAT_x,
		OUTPUT_TURN_INCREMENT_x,
		OUTPUT_TURN_TIME_RANGE_x,
		OUTPUT_TURN_NODE_RANGE_x,
		NULL
	};

	if (exe != NULL) {
		exe->Key_List (keys);
	}
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

void Turn_Output::Read_Control (int steps_per_second)
{
	int i, j;

	if (exe == NULL || steps_per_second < 1) return;

	//---- read the output delay data ----

	int num = exe->Highest_Control_Group (OUTPUT_TURN_FILE_x, 0);

	if (num != 0) {
		Output_Data *output;
		char *str_ptr, buffer [FIELD_BUFFER];
		int increment;

		//---- allocate array space ----

		if (!output_data.Max_Records (num)) {
			exe->Error ("Insufficient Memory for %d Output Turn Groups", num);
		}

		//---- process each file ----

		for (i=1; i <= num; i++) {

			str_ptr = exe->Get_Control_String (OUTPUT_TURN_FILE_x, i);

			if (str_ptr == NULL) continue;

			char *name_ptr = exe->Project_Filename (str_ptr);

			output = (Output_Data *) output_data.New_Record ();

			output->time_range = new Time_Range (steps_per_second);
			output->node_range = NULL;
			output->movement = NULL;

			output->file = new Movement_File (Db_Code::CREATE);

			exe->Break_Check (8);
			exe->Print (1);	
			str_fmt (buffer, sizeof (buffer), "Output Turn File #%d", i);

			output->file->File_Type (buffer);

			//---- set the output format ----

			str_ptr = exe->Get_Control_String (OUTPUT_TURN_FORMAT_x, i);

			if (str_ptr != NULL) {
				output->file->Dbase_Format (str_ptr);
			}
			output->file->Open (name_ptr);

			//---- get the filter ----

			str_ptr = exe->Get_Control_String (OUTPUT_TURN_FILTER_x, i);

			if (str_ptr != NULL) {
				output->filter = atoi (str_ptr);

				if (output->filter < 1) {
					exe->Error ("Output Turn Filter %d is Out of Range (>= 1)", output->filter);
				}
			} else {
				output->filter = 1;
			}

			exe->Print (1, "Output Turn Filter #%d = %d vehicle%c", i, output->filter,
				((output->filter != 1) ? 's' : ' '));

			//---- get the time format ----

			str_ptr = exe->Get_Control_String (OUTPUT_TURN_TIME_FORMAT_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Format (str_ptr)) {
					exe->Error ("Output Turn Time Format %s was Unrecognized", str_ptr);
				}
				exe->Print (1, "Output Turn Time Format #%d = %s", i, output->time_range->Time_Code ());
			}

			//---- get the increment ----

			str_ptr = exe->Get_Control_String (OUTPUT_TURN_INCREMENT_x, i);

			if (str_ptr != NULL) {
				output->time_range->Increment (str_ptr);
				increment = output->time_range->Increment ();

				if (increment < 1) {
					exe->Error ("Output Turn Increment %s is Out of Range", 
						output->time_range->Format_Duration (increment));
				}
			} else {
				output->time_range->Increment ("24:00");
				increment = output->time_range->Increment ();
			}
			exe->Print (1, "Output Turn Increment #%d = %s", i, 
				output->time_range->Format_Duration (increment));

			if (output->time_range->Format () == Time_Step::SECONDS) {
				exe->Print (0, " second%c", (increment != output->time_range->Duration (1) ? 's' : ' '));
			} else if (output->time_range->Format () == Time_Step::HOURS) {
				exe->Print (0, " hour%c", (increment != output->time_range->Duration (1.0) ? 's' : ' '));
			}

			//---- get the time range ----

			str_ptr = exe->Get_Control_String (OUTPUT_TURN_TIME_RANGE_x, i);

			if (str_ptr != NULL) {
				if (!output->time_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Turn Time Range", str_ptr);
				}
				exe->Print (1, "Output Turn Time Range #%d = ", i);

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

			//---- get the node range ----

			str_ptr = exe->Get_Control_String (OUTPUT_TURN_NODE_RANGE_x, i);

			if (str_ptr != NULL) {
				exe->Print (1, "Output Turn Node Range #%d = %s", i, str_ptr);

				output->node_range = new Data_Range ();

				if (!output->node_range->Add_Ranges (str_ptr)) {
					exe->File_Error ("Output Turn Node Range", str_ptr);
				}
			}

			//---- initialize the summary data ----

			output->movement = new Movement_Array ();

			if (output->movement == NULL) {
				exe->Error ("Creating Turn Movement Data");
			}
			output->step = -1;

			//---- save the output record ----

			if (!output_data.Add ()) {
				exe->Error ("Adding Turn Output Group");
			}
			output_flag = true;
		}
	}
}

//---------------------------------------------------------
//	In_Range
//---------------------------------------------------------

bool Turn_Output::In_Range (int step, int dir)
{
	if (output_flag) {
		Output_Data *output;
		node = 0;

		for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {
			if (output->time_range->In_Range (step)) {
				if (node == 0) {
					Network_Service *net = (Network_Service *) exe;
					Dir_Data *dir_ptr = net->dir_array->Record (dir);

					if (dir_ptr != NULL) {
						Link_Data *link_ptr = net->link_array->Record (dir_ptr->Link ());

						if (dir_ptr->Dir () == 1) {
							node = link_ptr->Anode ();
						} else {
							node = link_ptr->Bnode ();
						}
						Node_Data *node_ptr = net->node_array->Record (node);
						node = node_ptr->ID ();
					}
				}
				if (node == 0) return (false);

				if (output->node_range != NULL && output->node_range->Num_Ranges () > 0) {
					if (!output->node_range->In_Range (node)) continue;
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

void Turn_Output::Output_Check (int step)
{
	if (!output_flag) return;

	Output_Data *output;

	for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {
		if (output->time_range->In_Range (step)) {
			if (output->step >= 0 && output->time_range->At_Increment (step)) {
				Write_Turn (step, output);
			}
			output->step = step;
		}
	}
}

//---------------------------------------------------------
//	Summarize
//---------------------------------------------------------

void Turn_Output::Summarize (int step)
{
	if (output_flag) {
		Movement_Data *move_ptr, move_rec;

		if (node <= 0 || in_link <= 0 || out_link <= 0) {
			exe->Error ("Turn Output Keys Undefined");
		}

		Output_Data *output = (Output_Data *) output_data.Record ();

		for (; output; output = (Output_Data *) output_data.Next ()) {

			if (output->time_range->In_Range (step)) {
				if (output->node_range != NULL && output->node_range->Num_Ranges () > 0) {
					if (!output->node_range->In_Range (node)) continue;
				}
				move_rec.node = node;
				move_rec.in_link = in_link;
				move_rec.out_link = out_link;

				move_ptr = output->movement->Get (&move_rec);

				if (move_ptr == NULL) {
					move_rec.volume = 1;
					if (!output->movement->Add (&move_rec)) {
						exe->Error ("Adding Turn Movement Record");
					}
				} else {
					move_ptr->volume++;
				}
			}
		}
	}
}

//---------------------------------------------------------
//	Close
//---------------------------------------------------------

void Turn_Output::Close (void)
{
	if (!output_flag) return;

	Output_Data *output;

	for (output = (Output_Data *) output_data.First (); output; output = (Output_Data *) output_data.Next ()) {
		if (output->step >= 0 && !output->time_range->At_Increment (output->step)) {
			output->step = (output->step / output->time_range->Increment () + 1) * output->time_range->Increment ();

			Write_Turn (output->step, output);
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
		if (output->node_range != NULL) {
			delete output->node_range;
			output->node_range = NULL;
		}
		if (output->movement != NULL) {
			delete output->movement;
			output->movement = NULL;
		}
		output->step = -1;
	}
	output_flag = false;
}

//---------------------------------------------------------
//	Write_Turn
//---------------------------------------------------------

void Turn_Output::Write_Turn (int step, Output_Data *output)
{
	Movement_Data *move_ptr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;

	int last_in, last_out, in_link, out_link;

	last_in = last_out = in_link = out_link = 0;
	Network_Service *net = (Network_Service *) exe;

	for (move_ptr = output->movement->First_Key (); move_ptr; move_ptr = output->movement->Next_Key ()) {
		if (move_ptr->volume >= output->filter) {
			if (last_in != move_ptr->in_link) {
				last_in = move_ptr->in_link;

				dir_ptr = net->dir_array->Record (last_in);

				link_ptr = net->link_array->Record (dir_ptr->Link ());

				in_link = link_ptr->Link ();
			}
			if (last_out != move_ptr->out_link) {
				last_out = move_ptr->out_link;

				dir_ptr = net->dir_array->Record (last_out);

				link_ptr = net->link_array->Record (dir_ptr->Link ());

				out_link = link_ptr->Link ();
			}
			output->file->Node (move_ptr->node);
			output->file->In_Link (in_link);
			output->file->Out_Link (out_link);
			output->file->Start (output->time_range->Format_Step (step - output->time_range->Increment ()));
			output->file->End (output->time_range->Format_Step (step));
			output->file->Volume (move_ptr->volume);

			if (!output->file->Write ()) {
				exe->Error ("Writing Turn Movement File");
			}
		}
		move_ptr->volume = 0;
	}
}

